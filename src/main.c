/* Standard includes. */
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdarg.h>

/* FreeRTOS kernel includes. */
#include "FreeRTOS.h"
#include "task.h"
#include "stream_buffer.h"
#include "types.h"
#include "drawer.h"
#include "util.h"

// dev
#include <wchar.h>
#include <locale.h>
#include "traffic.h"
#include <signal.h>
#include "memory.h"
#include <pthread.h>
#include <time.h> // for clock_t, clock(), CLOCKS_PER_SEC

StreamBufferHandle_t hsensor_data;
StreamBufferHandle_t hsensor_signal;
StreamBufferHandle_t vsensor_data;
StreamBufferHandle_t vsensor_signal;

void vTaskDrawer(void *pvParameters)
{
    RoadState_t *road_state = (RoadState_t *)pvParameters;
    char c;
    init_drawer();
    WINDOW *win = get_win();
    nodelay(win, true);

    draw_first(road_state);
    vTaskDelay(pdMS_TO_TICKS(500));
    while (1)
    {
        traffic_step(road_state);
        redraw_fully(road_state);
        c = getch();
        if (c == 'q')
            break;
        else if (c == 'w')
            (*get_htraffic()) += .05;
        else if (c == 'W')
            (*get_htraffic()) -= .05;
        else if (c == 'e')
            (*get_vtraffic()) += .05;
        else if (c == 'E')
            (*get_vtraffic()) -= .05;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskEndScheduler();
}

void vTaskhTrafficSensor(void *pvParameters)
{
    RoadState_t *road_state = (RoadState_t *)pvParameters;
    bool sig;
    size_t xReceivedBytes, xSentBytes;
    int want_to_pass;
    while (1)
    {
        // wait for a request
        xReceivedBytes = xStreamBufferReceive(hsensor_signal,
                                              (void *)&sig,
                                              sizeof(bool),
                                              portMAX_DELAY);
        if (xReceivedBytes <= 0 || !sig)
            continue;
        want_to_pass = 0;
        for (int i = 14; i >= 0; i++)
        {
            if (road_state->hb_lane[i] && !entered_junc_zone(i, "hb"))
                want_to_pass++;
            if (road_state->ht_lane[i] && !entered_junc_zone(i, "ht"))
                want_to_pass++;
        }
        // send sensor data
        xSentBytes = xStreamBufferSend(hsensor_data,
                                       (void *)(&want_to_pass),
                                       sizeof(int),
                                       pdMS_TO_TICKS(50));
        LOG("sent\n");
        if (xSentBytes != sizeof(int))
        {
            LOG("failed to send h sensor data\n");
            exit(7);
        }
    }
}
void vTaskvTrafficSensor(void *pvParameters)
{
    RoadState_t *road_state = (RoadState_t *)pvParameters;
    bool sig;
    size_t xReceivedBytes, xSentBytes;
    int want_to_pass;
    while (1)
    {
        // wait for a request
        xReceivedBytes = xStreamBufferReceive(vsensor_signal,
                                              (void *)&sig,
                                              sizeof(bool),
                                              portMAX_DELAY);
        if (xReceivedBytes <= 0 || !sig)
            continue;
        want_to_pass = 0;
        for (int i = 9; i >= 0; i++)
        {
            if (road_state->vl_lane[i] && !entered_junc_zone(i, "vl"))
                want_to_pass++;
            if (road_state->vr_lane[i] && !entered_junc_zone(i, "vr"))
                want_to_pass++;
        }
        // send sensor data
        xSentBytes = xStreamBufferSend(vsensor_data,
                                       (void *)(&want_to_pass),
                                       sizeof(int),
                                       pdMS_TO_TICKS(50));
        LOG("sent\n");

        if (xSentBytes != sizeof(int))
        {
            LOG("failed to send v sensor data\n");
            exit(7);
        }
    }
}
void vTaskTrafficGenerator(void *pvParameters)
{
    RoadState_t *road_state = (RoadState_t *)pvParameters;
    while (1)
    {
        generate_traffic(road_state);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}
void vTaskLightTimersTweaker(void *pvParameters)
{
    RoadState_t *road_state = (RoadState_t *)pvParameters;
    size_t xReceivedBytes, xSentBytes;
    bool sig = true;
    int hwaiting, vwaiting;
    while (1)
    {
        vTaskDelay(pdMS_TO_TICKS(2000));
        xSentBytes = xStreamBufferSend(hsensor_signal, &sig, sizeof(bool), pdMS_TO_TICKS(50));
        if (xSentBytes < sizeof(bool))
        {
            LOG("h sensor signal not sent\n");
            exit(7);
        }
        xSentBytes = xStreamBufferSend(vsensor_signal, &sig, sizeof(bool), pdMS_TO_TICKS(50));
        vTaskDelay(pdMS_TO_TICKS(2000));

        if (xSentBytes < sizeof(bool))
        {
            LOG("v sensor signal not sent\n");
            exit(7);
        }
        // vTaskDelay(pdMS_TO_TICKS(2000));
        xReceivedBytes = xStreamBufferReceive(hsensor_data, &hwaiting, sizeof(int), pdMS_TO_TICKS(50));
        if (xReceivedBytes < sizeof(int))
        {
            LOG("h sensor data not received\n");
            exit(7);
        }
        xReceivedBytes = xStreamBufferReceive(vsensor_data, &vwaiting, sizeof(int), pdMS_TO_TICKS(50));
        if (xReceivedBytes < sizeof(int))
        {
            LOG("h sensor data not received\n");
            exit(7);
        }
        LOG("hsensor=%d, vsensor=%d\n", hwaiting, vwaiting);
    }
}
#define CONTROLLER_TASK_BODY(l)                                          \
    RoadState_t *road_state = (RoadState_t *)pvParameters;               \
    while (1)                                                            \
    {                                                                    \
        draw_lane_lights(road_state);                                    \
        if (road_state->l##_lane_light == LIGHT_RED)                     \
        {                                                                \
            vTaskDelay(pdMS_TO_TICKS(road_state->l##_lane_red_time));    \
            road_state->l##_lane_light = LIGHT_GREEN;                    \
            continue;                                                    \
        }                                                                \
        else if (road_state->l##_lane_light == LIGHT_YELLOW)             \
        {                                                                \
            vTaskDelay(pdMS_TO_TICKS(road_state->l##_lane_yellow_time)); \
            road_state->l##_lane_light = LIGHT_RED;                      \
            continue;                                                    \
        }                                                                \
        else if (road_state->l##_lane_light == LIGHT_GREEN)              \
        {                                                                \
            vTaskDelay(pdMS_TO_TICKS(road_state->l##_lane_green_time));  \
            road_state->l##_lane_light = LIGHT_YELLOW;                   \
            continue;                                                    \
        }                                                                \
        else                                                             \
        {                                                                \
            printf("unkown light: %d\n", road_state->l##_lane_light);    \
            exit(1);                                                     \
        }                                                                \
    }

void vTaskhtLightController(void *pvParameters)
{
    CONTROLLER_TASK_BODY(ht)
}
void vTaskhbLightController(void *pvParameters)
{
    CONTROLLER_TASK_BODY(hb)
}
void vTaskvlLightController(void *pvParameters)
{
    CONTROLLER_TASK_BODY(vl)
}
void vTaskvrLightController(void *pvParameters){
    CONTROLLER_TASK_BODY(vr)}

RoadState_t road_state;
TaskHandle_t drawerHandle, generatorHandle, lightTweakerHandle, htControllerHandle, hbControllerHandle, vlControllerHandle, vrControllerHandle, hsensorHandle, vsensorHandle;

void resizeHandler(int sig)
{
    on_window_resize();
    redraw_fully(&road_state);
}

int main(void)
{
    hsensor_signal = xStreamBufferCreate(sizeof(bool), sizeof(bool));
    vsensor_signal = xStreamBufferCreate(sizeof(bool), sizeof(bool));
    hsensor_data = xStreamBufferCreate(sizeof(int), sizeof(int));
    vsensor_data = xStreamBufferCreate(sizeof(int), sizeof(int));

    init_config();
    signal(SIGWINCH, resizeHandler);
    memset(&road_state, 0, sizeof(RoadState_t));
    *get_generated_traffic() = 0;
    *get_passed_traffic() = 0;

    generate_traffic(&road_state);
    road_state.hb_lane_light = LIGHT_RED;
    road_state.ht_lane_light = LIGHT_RED;
    road_state.vl_lane_light = LIGHT_GREEN;
    road_state.vr_lane_light = LIGHT_GREEN;

    road_state.hb_lane_red_time = RED_LIGHT_TIME_MS;
    road_state.hb_lane_green_time = GREEN_LIGHT_TIME_MS;
    road_state.hb_lane_yellow_time = YELLOW_LIGHT_TIME_MS;
    road_state.ht_lane_red_time = RED_LIGHT_TIME_MS;
    road_state.ht_lane_green_time = GREEN_LIGHT_TIME_MS;
    road_state.ht_lane_yellow_time = YELLOW_LIGHT_TIME_MS;
    road_state.vl_lane_red_time = RED_LIGHT_TIME_MS;
    road_state.vl_lane_green_time = GREEN_LIGHT_TIME_MS;
    road_state.vl_lane_yellow_time = YELLOW_LIGHT_TIME_MS;
    road_state.vr_lane_red_time = RED_LIGHT_TIME_MS;
    road_state.vr_lane_green_time = GREEN_LIGHT_TIME_MS;
    road_state.vr_lane_yellow_time = YELLOW_LIGHT_TIME_MS;

    pthread_attr_t attr;
    pthread_attr_init(&attr);
    size_t stack_size = 1024 * 1024; // 1MB
    pthread_attr_setstacksize(&attr, stack_size);

    // for (int i = 0; i < 5; i++)
    // {
    //     traffic_step(&road_state);
    //     redraw_fully(&road_state);
    // }
    // getch();
    // clear();
    // endwin();

    xTaskCreate(&vTaskDrawer, "Drawer Task", 1024, &road_state, 1, &drawerHandle);
    xTaskCreate(&vTaskTrafficGenerator, "Traffic Generator Controller", 1024, &road_state, 1, &generatorHandle);
    xTaskCreate(&vTaskhTrafficSensor, "Horizontal Sensor Task", 1024, &road_state, 1, &hsensorHandle);
    xTaskCreate(&vTaskvTrafficSensor, "Vertical Sensor Task", 1024, &road_state, 1, &vsensorHandle);
    xTaskCreate(&vTaskLightTimersTweaker, "Lights Timers Tweaker", 1024, &road_state, 1, &lightTweakerHandle);
    xTaskCreate(&vTaskhtLightController, "ht Light Controller", 1024, &road_state, 1, &htControllerHandle);
    xTaskCreate(&vTaskhbLightController, "hb Light Controller", 1024, &road_state, 1, &hbControllerHandle);
    xTaskCreate(&vTaskvlLightController, "vl Light Controller", 1024, &road_state, 1, &vlControllerHandle);
    xTaskCreate(&vTaskvrLightController, "vr Light Controller", 1024, &road_state, 1, &vrControllerHandle);

    // xTaskCreate(&vTask2, "Task 2", 1024, NULL, 1, NULL);

    vTaskStartScheduler();

    // init_drawer();
    // WINDOW *win = get_win();
    // // road_state.vl_lane[5] = 1;
    // // road_state.hb_lane[8] = 1;
    // draw_first(&road_state);
    // getchar();
    clear();
    endwin();

    return 0;
}