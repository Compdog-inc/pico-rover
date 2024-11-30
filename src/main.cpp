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

    NetworkTableInstance *networkTable = new NetworkTableInstance();
    networkTable->startServer();

    // Initialize and create subsystems
    drivetrain = new Drivetrain();
    lights = new Lights();
    battery = new Battery();
    battery->startPingTimer();

    lights->setRingIndicatorPattern(Pattern::Alt1, Pattern::Alt2);

    Driverstation *driverstation = new Driverstation();
    UDPXbox *xbox = new UDPXbox();

    int64_t diff = 0;

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

        auto start = get_absolute_time();
        networkTable->setTestValue(diff);
        diff = absolute_time_diff_us(start, get_absolute_time());
    }

    delete xbox;
    delete driverstation;

    // Deinitialize subsystems
    networkTable->close();
    delete networkTable;
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