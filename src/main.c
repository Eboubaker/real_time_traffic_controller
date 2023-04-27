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

StaticStreamBuffer_t stream;
void vTaskDrawer(void *pvParameters)
{
    RoadState_t *road_state = (RoadState_t *)pvParameters;
    WINDOW *win = draw_first(road_state);

    while (1)
    {
        traffic_step(road_state);
        redraw_fully(road_state);
        vTaskDelay(pdMS_TO_TICKS(100));
        getch();
    }
    vTaskEndScheduler();
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

void resizeHandler(int sig)
{
    redraw_fully(&road_state);
}

int main(void)
{
    signal(SIGWINCH, resizeHandler);
    memset(&road_state, 0, sizeof(RoadState_t));
    generate_traffic(1, &road_state);
    road_state.hb_lane_light = LIGHT_RED;
    road_state.ht_lane_light = LIGHT_RED;
    road_state.vl_lane_light = LIGHT_RED;
    road_state.vr_lane_light = LIGHT_RED;

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

    TaskHandle_t drawerHandle;
    xTaskCreate(&vTaskDrawer, "Drawer Task", 1024, &road_state, 1, &drawerHandle);

    // xTaskCreate(&vTask2, "Task 2", 1024, NULL, 1, NULL);

    vTaskStartScheduler();

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