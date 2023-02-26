/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"

void vTask1(void *pvParameters)
{
    while(1)
    {
        printf("This is task 1\n");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(100));
    }
}
void vTask2(void *pvParameters)
{
    while (1)
    {
        printf("This is task 2\n");
        fflush(stdout);
        vTaskDelay(pdMS_TO_TICKS(500));
    }
}

int main(void)
{
    xTaskCreate(&vTask1, "Task 1", 1000, NULL, 3, NULL);
    xTaskCreate(&vTask2, "Task 2", 100, NULL, 1, NULL);

    vTaskStartScheduler();

    return 0;
}
