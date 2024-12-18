// Standard headers
#include <stdlib.h>
#include <string>
#include <cstdarg>
#include <cstring>

// Kernel headers
#include <FreeRTOS.h>
#include <task.h>

// Config headers
#include "config/timers.h"
#include "config/options.h"

// Hardware headers
#include <pico/stdlib.h>
#include <pico/time.h>

// Libraries
#include <radio.h>
#include <nt/ntinstance.h>
#include <nt/ntentry.h>
#include <math/units.h>
#include <math/random.h>
#include <board/temperature.h>

// Subsystems
#include "subsystems/drivetrain.h"
#include "subsystems/lights.h"
#include "subsystems/battery.h"

// Control
#include "control/udpxbox.h"
#include "control/driverstation.h"

#include "communication.h"

using namespace std::literals;

static Drivetrain *drivetrain;
static Lights *lights;
static Battery *battery;

static void main_task(__unused void *params)
{
    // Create wifi radio
    Radio *radio = new Radio();
    if (!radio->isInitialized())
    {
        printf("Error initializing radio\n");
        delete radio;
        vTaskDelete(NULL);
        return;
    }

    NetworkTableInstance *nt = new NetworkTableInstance();
    nt->startServer();

    // Initialize and create subsystems
    drivetrain = new Drivetrain();
    lights = new Lights();
    battery = new Battery();
    battery->startPingTimer();

    lights->setRingIndicatorPattern(Pattern::Alt1, Pattern::Alt2);

    Driverstation *driverstation = new Driverstation();
    UDPXbox *xbox = new UDPXbox();

    Communication *comm = new Communication(true);

    NTEntry distances = NTEntry(nt, "SmartDashboard/Distance", NTDataValue(std::vector<float>{0, 0, 0, 0, 0, 0}));

    absolute_time_t lastFlush = get_absolute_time();
    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(20));
        if (xbox->isConnected())
        {
            drivetrain->drive(xbox->getForward(), xbox->getRotation());
            lights->setStatusLedPattern(Pattern::Blink);
        }
        else
        {
            drivetrain->stop();
            lights->setStatusLedPattern(Pattern::On);
        }

        CommunicationControl control{};

        CommunicationStatus status{};
        CommunicationDistanceSensors sensors{};
        if (!comm->read(control, &status, &sensors))
        {
            printf("[COMM] Error reading data\n");
        }
        else if (status.version != 0xBADC0DE5 || !status.running)
        {
            printf("[COMM] Invalid status %#010x, %#04x\n", status.version, status.running ? 0xFF : 0x00);
        }
        else
        {
            distances.set(NTDataValue(std::vector<float>{sensors.distance0, sensors.distance1, sensors.distance2, sensors.distance3, sensors.distance4, sensors.distance5}));
        }

        if (absolute_time_diff_us(lastFlush, get_absolute_time()) > 100 * 1000)
        {
            lastFlush = get_absolute_time();
            nt->flush();
        }
    }

    delete comm;

    delete xbox;
    delete driverstation;

    // Deinitialize subsystems
    nt->close();
    delete nt;
    radio->deinit();
    delete radio;
    delete drivetrain;
    delete lights;
    delete battery;

    vTaskDelete(NULL);
}

int main()
{
    stdio_init_all();
    sleep_us(64);

    Config::init_timers();
    Temperature::init();

    printf("[BOOT] Creating MainThread task\n");
    TaskHandle_t task;
    xTaskCreate(main_task, "MainThread", configMAIN_THREAD_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 4UL), &task);

    printf("[BOOT] Starting task scheduler\n");
    vTaskStartScheduler();

    Temperature::deinit();
    Config::deinit_timers();
    return 0;
}

extern "C" void rtos_panic(const char *fmt, ...)
{
    puts("\n*** PANIC ***\n");
    if (fmt)
    {
        va_list args;
        va_start(args, fmt);
        vprintf(fmt, args);
        va_end(args);
        puts("\n");
    }

    exit(1);
}

extern "C" void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    panic("vApplicationStackOverflowHook called (%s)", pcTaskName);
}