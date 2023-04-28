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
#include "ncursesw/ncurses.h"
#include <wchar.h>
#include <locale.h>
#include "traffic.h"
#include <signal.h>
#include "memory.h"
#include <pthread.h>
#include <time.h> // for clock_t, clock(), CLOCKS_PER_SEC

StaticStreamBuffer_t stream;
void vTaskDrawer(void *pvParameters)
{
    RoadState_t *road_state = (RoadState_t *)pvParameters;
    init_drawer();
    WINDOW *win = get_win();
    draw_first(road_state);
    vTaskDelay(pdMS_TO_TICKS(500));
    int i = 0;
    nodelay(win, true);
    while (1)
    {
        traffic_step(road_state);
        redraw_fully(road_state);
        char c = getch();
        if (c == 'q')
            break;
        if (c == 'w')
            (*get_htraffic()) += .05;
        if (c == 'W')
            (*get_htraffic()) -= .05;
        if (c == 'e')
            (*get_vtraffic()) += .05;
        if (c == 'E')
            (*get_vtraffic()) -= .05;
        vTaskDelay(pdMS_TO_TICKS(500));
    }
    vTaskEndScheduler();
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
#define CONTROLLER_TASK_BODY(l)                                            \
    RoadState_t *road_state = (RoadState_t *)pvParameters;                 \
    while (1)                                                              \
    {                                                                      \
        draw_lane_lights(road_state);                                      \
        if (road_state->l##_lane_light == LIGHT_RED)                     \
        {                                                                  \
            vTaskDelay(pdMS_TO_TICKS(road_state->l##_lane_red_time));    \
            road_state->l##_lane_light = LIGHT_GREEN;                    \
            continue;                                                      \
        }                                                                  \
        else if (road_state->l##_lane_light == LIGHT_YELLOW)             \
        {                                                                  \
            vTaskDelay(pdMS_TO_TICKS(road_state->l##_lane_yellow_time)); \
            road_state->l##_lane_light = LIGHT_RED;                      \
            continue;                                                      \
        }                                                                  \
        else if (road_state->l##_lane_light == LIGHT_GREEN)              \
        {                                                                  \
            vTaskDelay(pdMS_TO_TICKS(road_state->l##_lane_green_time));  \
            road_state->l##_lane_light = LIGHT_YELLOW;                   \
            continue;                                                      \
        }                                                                  \
        else                                                               \
        {                                                                  \
            printf("unkown light: %d\n", road_state->l##_lane_light);    \
            exit(1);                                                       \
        }                                                                  \
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
    CONTROLLER_TASK_BODY(vr)
}
void draw_traffic1()
{
    // attrset(COLOR_PAIR(road_state.h_lane_light));
    // move(3, 0);
    // printw("==========");
    // move(3, 19);
    // printw("==========");
    // move(6, 0);
    // printw("==========");
    // move(6, 19);
    // printw("==========");
    printw(
        "          ||     ||          \n"
        "          || D   ||          \n"
        "          ||   D ||          \n"
        "==========|| D D ||==========\n"
        "    D D D           D   D D D\n"
        "D D D D D                    \n"
        "==========||     ||==========\n"
        "          || D D ||          \n"
        "          ||     ||          \n"
        "          ||     ||          \n");
    // draw_lane_lights('h');
    refresh();
    char t = getch();
    clear();
    endwin();
}

RoadState_t road_state;
TaskHandle_t drawerHandle, generatoreHandle, htControllerHandle, hbControllerHandle, vlControllerHandle, vrControllerHandle;

void resizeHandler(int sig)
{
    on_window_resize();
    redraw_fully(&road_state);
}

int main(void)
{
    init_config();
    signal(SIGWINCH, resizeHandler);
    memset(&road_state, 0, sizeof(RoadState_t));
    *get_generated_traffic() = 0;
    *get_passed_traffic() = 0;

    generate_traffic(&road_state);
    road_state.hb_lane_light = LIGHT_RED;
    road_state.ht_lane_light = LIGHT_RED;
    road_state.vl_lane_light = LIGHT_RED;
    road_state.vr_lane_light = LIGHT_RED;

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
    xTaskCreate(&vTaskhtLightController, "ht Light Controller", 1024, &road_state, 1, &htControllerHandle);
    xTaskCreate(&vTaskhbLightController, "hb Light Controller", 1024, &road_state, 1, &hbControllerHandle);
    xTaskCreate(&vTaskvlLightController, "vl Light Controller", 1024, &road_state, 1, &vlControllerHandle);
    xTaskCreate(&vTaskvrLightController, "vr Light Controller", 1024, &road_state, 1, &vrControllerHandle);
    xTaskCreate(&vTaskTrafficGenerator, "Generator Controller", 1024, &road_state, 1, &generatoreHandle);

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

/*
          |      |
          | D    |
          |    D |
==========| D  D |==========
D D D D D
D D D D D
==========|      |==========
          | D  D |
          |      |
          |      |
*/