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

// static radio_characteristics_packet robot_characteristics = {
//     vec3(Units::fromInches(13), Units::fromInches(12.5), Units::fromInches(21)), // frameSize

//     Units::fromInches(4.75), // wheelDiameter

//     vec3(Units::fromInches(-7), Units::fromInches(2.375), Units::fromInches(6)),        // wheel0Location
//     vec3(Units::fromInches(7), Units::fromInches(2.375), Units::fromInches(6)),         // wheel1Location
//     vec3(Units::fromInches(7.1875), Units::fromInches(2.375), Units::fromInches(0.25)), // wheel2Location
//     vec3(Units::fromInches(-7.0625), Units::fromInches(2.375), Units::fromInches(0.5)), // wheel3Location
//     vec3(Units::fromInches(7.1875), Units::fromInches(2.375), Units::fromInches(-5)),   // wheel4Location
//     vec3(Units::fromInches(-6.375), Units::fromInches(2.375), Units::fromInches(-5)),   // wheel5Location

//     vec4(Units::fromInches(-5.75), Units::fromInches(3), Units::fromInches(9.75), Units::fromDegrees(315)),  // whisker0
//     vec4(Units::fromInches(0), Units::fromInches(3), Units::fromInches(10.4375), Units::fromDegrees(0)),     // whisker1
//     vec4(Units::fromInches(5.75), Units::fromInches(3), Units::fromInches(9.75), Units::fromDegrees(45)),    // whisker2
//     vec4(Units::fromInches(-5.75), Units::fromInches(3), Units::fromInches(-9.75), Units::fromDegrees(135)), // whisker3
//     vec4(Units::fromInches(0), Units::fromInches(3), Units::fromInches(-10.4375), Units::fromDegrees(180)),  // whisker4
//     vec4(Units::fromInches(-5.75), Units::fromInches(3), Units::fromInches(-9.75), Units::fromDegrees(225))  // whisker5
// };

static Drivetrain *drivetrain;

static void main_task(__unused void *params)
{
    // Initialize and create subsystems
    light_subsystem_init();
    drivetrain = new Drivetrain();

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
    light_subsystem_deinit();

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