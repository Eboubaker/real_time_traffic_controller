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
#include "print.h"
#include "util.h"

// dev
#include <wchar.h>
#include <locale.h>
#include "road.h"
#include <signal.h>
#include "memory.h"
#include <pthread.h>
#include <time.h> // for clock_t, clock(), CLOCKS_PER_SEC
#include "config.h"

RoadState_t road_state;
StreamBufferHandle_t hsensor_data;
StreamBufferHandle_t hsensor_signal;
StreamBufferHandle_t vsensor_data;
StreamBufferHandle_t vsensor_signal;
TaskHandle_t drawerHandle,
    generatorHandle,
    lightTweakerHandle,
    htControllerHandle,
    hbControllerHandle,
    vlControllerHandle,
    vrControllerHandle,
    hsensorHandle,
    vsensorHandle;

#define CONTROLLER_TASK_BODY(l)                                                                \
    RoadState_t *road_state = (RoadState_t *)pvParameters;                                     \
    while (1)                                                                                  \
    {                                                                                          \
        if (road_state->l##_lane_light == LIGHT_RED)                                           \
        {                                                                                      \
            vTaskDelay(pdMS_TO_TICKS(road_state->l##_lane_red_time_ms / *get_sim_speed()));    \
            road_state->l##_lane_light = LIGHT_GREEN;                                          \
            continue;                                                                          \
        }                                                                                      \
        else if (road_state->l##_lane_light == LIGHT_YELLOW)                                   \
        {                                                                                      \
            vTaskDelay(pdMS_TO_TICKS(road_state->l##_lane_yellow_time_ms / *get_sim_speed())); \
            road_state->l##_lane_light = LIGHT_RED;                                            \
            continue;                                                                          \
        }                                                                                      \
        else if (road_state->l##_lane_light == LIGHT_GREEN)                                    \
        {                                                                                      \
            vTaskDelay(pdMS_TO_TICKS(road_state->l##_lane_green_time_ms / *get_sim_speed()));  \
            road_state->l##_lane_light = LIGHT_YELLOW;                                         \
            continue;                                                                          \
        }                                                                                      \
        else                                                                                   \
        {                                                                                      \
            printf("unkown light: %d\n", road_state->l##_lane_light);                          \
            exit(1);                                                                           \
        }                                                                                      \
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
void vTaskvrLightController(void *pvParameters)
{
    CONTROLLER_TASK_BODY(vr);
}

void vTaskDrawer(void *pvParameters)
{
    RoadState_t *road_state = (RoadState_t *)pvParameters;
    char c;
    init_drawer();
    vTaskDelay(pdMS_TO_TICKS(500 / *get_sim_speed()));
    while (1)
    {
        traffic_step(road_state);
        redraw_fully(road_state);
        c = getch();
        if (c == 'q')
            break;
        else if (c == 'W')
            (*get_htraffic()) += .05;
        else if (c == 'w')
            (*get_htraffic()) -= .05;
        else if (c == 'E')
            (*get_vtraffic()) += .05;
        else if (c == 'e')
            (*get_vtraffic()) -= .05;
        else if (c == '+')
            (*get_sim_speed()) += 1;
        else if (c == '-')
            (*get_sim_speed()) -= 1;
        vTaskDelay(pdMS_TO_TICKS(500 / *get_sim_speed()));
    }
    vTaskEndScheduler();
}
#define SENSOR_BODY(s, a, b, n)                                             \
    RoadState_t *road_state = (RoadState_t *)pvParameters;                  \
    bool sig;                                                               \
    size_t xReceivedBytes, xSentBytes;                                      \
    int want_to_pass;                                                       \
    int delay_s = 2;                                                        \
    TickType_t xsleepms = delay_s * 1000;                                   \
    int frame_time_s = 30;                                                  \
    int calcs_per_frame = frame_time_s / delay_s;                           \
    int calc_i = 0;                                                         \
    int load = 0;                                                           \
    while (1)                                                               \
    {                                                                       \
        want_to_pass = 0;                                                   \
        for (int i = n - 1; i >= 0; i--)                                    \
        {                                                                   \
            if (road_state->s##a##_lane[i] && !entered_junc_zone(i, #s #a)) \
                want_to_pass++;                                             \
            if (road_state->s##b##_lane[i] && !entered_junc_zone(i, #s #b)) \
                want_to_pass++;                                             \
        }                                                                   \
        load += want_to_pass;                                               \
        calc_i++;                                                           \
        if (calc_i == calcs_per_frame)                                      \
        {                                                                   \
            calc_i = 0;                                                     \
            xSentBytes = xStreamBufferSend(s##sensor_data,                  \
                                           (void *)&load,                   \
                                           sizeof(int),                     \
                                           portMAX_DELAY);                  \
            road_state->s##load = load;                                     \
            load = 0;                                                       \
            if (xSentBytes != sizeof(int))                                  \
            {                                                               \
                LOG("failed to send %s sensor data\n", #s);                 \
                exit(7);                                                    \
            }                                                               \
        }                                                                   \
        vTaskDelay(xsleepms / *get_sim_speed());                            \
    }
void vTaskvTrafficSensor(void *pvParameters)
{
    SENSOR_BODY(v, l, r, V_LANE_CAP);
}
void vTaskhTrafficSensor(void *pvParameters)
{
    SENSOR_BODY(h, t, b, H_LANE_CAP);
}
void vTaskTrafficGenerator(void *pvParameters)
{
    RoadState_t *road_state = (RoadState_t *)pvParameters;
    while (1)
    {
        generate_traffic(road_state);
        vTaskDelay(pdMS_TO_TICKS(1000 / *get_sim_speed()));
    }
}
void vTaskLightTimersTweaker(void *pvParameters)
{
    RoadState_t *road_state = (RoadState_t *)pvParameters;
    size_t xReceivedBytes, xSentBytes;
    int hload, vload;
    while (1)
    {
        xReceivedBytes = xStreamBufferReceive(hsensor_data, (void *)&hload, sizeof(int), portMAX_DELAY);
        if (xReceivedBytes < sizeof(int))
        {
            LOG("h sensor data not received\n");
            exit(7);
        }
        xReceivedBytes = xStreamBufferReceive(vsensor_data, (void *)&vload, sizeof(int), portMAX_DELAY);
        if (xReceivedBytes < sizeof(int))
        {
            LOG("v sensor data not received\n");
            exit(7);
        }
        LOG("hload=%2d, vload=%2d ==> ", hload, vload);
        if (hload > vload)
        {
            if (road_state->ht_lane_red_time_ms > 1000)
            {
                road_state->ht_lane_green_time_ms += 1000;
                road_state->hb_lane_green_time_ms += 1000;
                road_state->vl_lane_red_time_ms += 1000;
                road_state->vr_lane_red_time_ms += 1000;

                road_state->ht_lane_red_time_ms -= 1000;
                road_state->hb_lane_red_time_ms -= 1000;
                road_state->vl_lane_green_time_ms -= 1000;
                road_state->vr_lane_green_time_ms -= 1000;
                LOG("increasing time for h lane\n");
            }
        }
        else
        {
            if (road_state->vl_lane_red_time_ms > 1000)
            {
                road_state->vl_lane_green_time_ms += 1000;
                road_state->vr_lane_green_time_ms += 1000;
                road_state->ht_lane_red_time_ms += 1000;
                road_state->hb_lane_red_time_ms += 1000;

                road_state->vl_lane_red_time_ms -= 1000;
                road_state->vr_lane_red_time_ms -= 1000;
                road_state->ht_lane_green_time_ms -= 1000;
                road_state->hb_lane_green_time_ms -= 1000;
                LOG("increasing time for v lane\n");
            }
        }
    }
}

void resizeHandler(int sig)
{
    on_window_resize(&road_state);
}

int main(void)
{
    hsensor_signal = xStreamBufferCreate(sizeof(bool), sizeof(bool));
    vsensor_signal = xStreamBufferCreate(sizeof(bool), sizeof(bool));
    hsensor_data = xStreamBufferCreate(sizeof(int), sizeof(int));
    vsensor_data = xStreamBufferCreate(sizeof(int), sizeof(int));
    signal(SIGWINCH, resizeHandler);
    init_config();
    init_road_state(&road_state);

    xTaskCreate(&vTaskDrawer, "Drawer Task", 1024, &road_state, 1, &drawerHandle);
    xTaskCreate(&vTaskTrafficGenerator, "Traffic Generator Controller", 1024, &road_state, 1, &generatorHandle);
    xTaskCreate(&vTaskhTrafficSensor, "Horizontal Sensor Task", 1024, &road_state, 1, &hsensorHandle);
    xTaskCreate(&vTaskvTrafficSensor, "Vertical Sensor Task", 1024, &road_state, 1, &vsensorHandle);
    xTaskCreate(&vTaskLightTimersTweaker, "Lights Timers Tweaker", 1024, &road_state, 1, &lightTweakerHandle);
    xTaskCreate(&vTaskhtLightController, "ht Light Controller", 1024, &road_state, 1, &htControllerHandle);
    xTaskCreate(&vTaskhbLightController, "hb Light Controller", 1024, &road_state, 1, &hbControllerHandle);
    xTaskCreate(&vTaskvlLightController, "vl Light Controller", 1024, &road_state, 1, &vlControllerHandle);
    xTaskCreate(&vTaskvrLightController, "vr Light Controller", 1024, &road_state, 1, &vrControllerHandle);
    vTaskStartScheduler();
    // init_drawer();
    // nodelay(get_win(), false);
    // for (int i = 0; i < V_LANE_CAP; i++)
    // {
    //     road_state.vl_lane[i] = 1;
    //     road_state.hb_lane[9] = 1;
    //     LOG("converted: %d\n", junc_zone_convert(i, "vl", "hb"));
    //     // road_state.hb_lane[i] = 1;

    //     redraw_fully(&road_state);
    //     getch();
    // }
    // int i = 0;
    // road_state.hb_lane[i] = 1;
    // redraw_fully(&road_state);
    // int x = entered_junc_zone(i, "vl");
    // LOG("inside: %d\n", x);
    // redraw_fully(&road_state);
    // getch();
    endwin();
    return 0;
}