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
#include <udpsocket.h>
#include <math/units.h>
#include <board/temperature.h>

// Subsystems
#include "subsystems/lights.h"
#include "subsystems/drivetrain.h"

using namespace std::literals;

static Drivetrain *drivetrain;
static Lights *lights;

static void main_task(__unused void *params)
{
    // Initialize and create subsystems
    drivetrain = new Drivetrain();
    lights = new Lights();

    // Create wifi radio
    Radio *radio = new Radio();
    if (!radio->isInitialized())
    {
        printf("Error initializing radio\n");
        vTaskDelete(NULL);
        return;
    }

    // Create drive command socket
    UdpSocket *driveSocket = new UdpSocket(5001);
    driveSocket->receiveCallback = [](UdpSocket *socket, Datagram *datagram)
    {
        if (datagram->length == 4)
        {
            int16_t speed;
            int16_t rotation;
            std::memcpy(&speed, &((uint8_t *)datagram->data)[0], sizeof(int16_t));
            std::memcpy(&rotation, &((uint8_t *)datagram->data)[2], sizeof(int16_t));
            drivetrain->drive(Units<float>::meters((float)speed / (float)INT16_MAX), Units<float>::radians((float)rotation / (float)INT16_MAX * 10.0f));
        }
    };

    // Broadcast to port 5002 every second
    std::string_view str = "Hello from UDP!\r\n"sv;
    Datagram dg(str.data(), str.length(), 5002);

    while (true)
    {
        vTaskDelay(pdMS_TO_TICKS(1000));
        driveSocket->broadcast(&dg);
    }

    // Clean up socket and radio
    driveSocket->deinit();
    driveSocket->~UdpSocket();

    radio->deinit();
    radio->~Radio();

    // Deinitialize subsystems
    delete drivetrain;
    lights->~Lights();

    vTaskDelete(NULL);
}

int64_t battery_ping_end(alarm_id_t id, __unused void *params)
{
    gpio_put(Config::GPIO::BATTERY_PING_PIN, true);
    return 0;
}

void init()
{
    Config::init_timers();
    Temperature::init();

    gpio_init(Config::GPIO::SETUP_BTN);
    gpio_set_dir(Config::GPIO::SETUP_BTN, GPIO_IN);
    gpio_pull_up(Config::GPIO::SETUP_BTN);

    gpio_init(Config::GPIO::BATTERY_PING_PIN);
    gpio_set_dir(Config::GPIO::BATTERY_PING_PIN, true);
    gpio_put(Config::GPIO::BATTERY_PING_PIN, true);

    repeating_timer_t batteryPingTimer;
    add_repeating_timer_ms(-1000, [](repeating_timer_t *rt) -> bool
                           { gpio_put(Config::GPIO::BATTERY_PING_PIN, false); add_alarm_in_ms(100, battery_ping_end, NULL, true); return true; }, NULL, &batteryPingTimer);
}

void deinit()
{
    Temperature::deinit();
    Config::deinit_timers();
}

int main()
{
    stdio_init_all();
    sleep_us(64);

    init();

    printf("[BOOT] Creating MainThread task\n");
    TaskHandle_t task;
    xTaskCreate(main_task, "MainThread", configMAIN_THREAD_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 4UL), &task);

    printf("[BOOT] Starting task scheduler\n");
    vTaskStartScheduler();

    deinit();
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