// Standard headers
#include <stdlib.h>
#include <string>
#include <cstdarg>
#include <cstring>

// Kernel headers
#include <FreeRTOS.h>
#include <task.h>

#include "terminal.h"
#include <lwipdebug.h>

TaskHandle_t task;
bool isRunning = false;

void Terminal::start()
{
    isRunning = true;
    printf("[Termnal] Creating task\n");
    xTaskCreate(terminal_task, "Terminal", configMAIN_THREAD_STACK_SIZE, NULL, (tskIDLE_PRIORITY + 4UL), &task);
}

void Terminal::stop()
{
    isRunning = false;
}

void terminal_task(__unused void *params)
{
    static char buffer[256];

    while (isRunning)
    {
        puts("> ");

        // read line from stdin
        if (fgets(buffer, sizeof(buffer), stdin) != NULL)
        {
            // remove newline character
            buffer[strcspn(buffer, "\n")] = 0;
            std::string command(buffer);

            printf("[Terminal] Received command: %*.s\n", command.length(), command.c_str());

            // process command
            if (command == "help")
            {
                printf("Available commands:\n");
                printf("help - Show this help message\n");
                printf("lwip status - Print status of LWIP stack\n");
            }
            else if (command == "lwip status")
            {
                LWIP::PrintLwipTcpPcbStatus();
            }
            else
            {
                printf("Unknown command: %s\n", command.c_str());
            }

            vTaskDelay(pdMS_TO_TICKS(100));
        }

        printf("[Terminal] Stopping task\n");
        vTaskDelete(NULL);
    }