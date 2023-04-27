#include "drawer.h"
#include "types.h"
#include "ncurses.h"
#include <locale.h>
#include <stdlib.h>
#include "config.h"
#include "traffic.h"
#include <sys/ioctl.h>
#include <unistd.h>


WINDOW *draw_first(RoadState_t *road_state)
{
    setlocale(LC_ALL, "");

    // init ncurses screen
    WINDOW *win = initscr();

    // init lane colors
    start_color();
    init_pair(LIGHT_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(LIGHT_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(LIGHT_RED, COLOR_RED, COLOR_BLACK);
    init_pair(TRAFFIC_COLLISION_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(TRAFFIC_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(NO_LIGHT_LANE_COLOR, COLOR_WHITE, COLOR_BLACK);

    init_pair(1, COLOR_GREEN, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_RED, COLOR_BLACK);

    draw_lane_lights(road_state);

    draw_traffic(road_state);

    return win;
}

void redraw_fully(RoadState_t *road_state)
{
    // struct winsize size;
    // ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    clear();
    draw_lane_lights(road_state);
    draw_traffic(road_state);
    refresh();
}

void draw_fatal_exit(char *msg)
{
    move(11, 0);
    printw("fatal: %s", msg);
    move(12, 0);
    printw("press any key to exit.");
    move(0, 0);
    refresh();
    getch();
    clear();
    endwin();
    fprintf(stderr, "%s", msg);
    exit(-1);
}

void draw_lane_lights(RoadState_t *road_state)
{
    // if (lane == 'h')
    // {
        attrset(COLOR_PAIR(road_state->ht_lane_light));
        move(3, 0);
        printw("==========");
        attrset(COLOR_PAIR(NO_LIGHT_LANE_COLOR));
        move(3, 19);
        printw("==========");
        move(6, 0);
        printw("==========");
        attrset(COLOR_PAIR(road_state->hb_lane_light));
        move(6, 19);
        printw("==========");
    // }
    // else
    // {
        for (int i = 0; i < 4; i++)
        {
            attrset(COLOR_PAIR(road_state->vl_lane_light));
            move(i, 10);
            printw("||");
            attrset(COLOR_PAIR(NO_LIGHT_LANE_COLOR));
            move(i, 17);
            printw("||");
        }
        for (int i = 6; i < 10; i++)
        {
            attrset(COLOR_PAIR(NO_LIGHT_LANE_COLOR));
            move(i, 10);
            printw("||");
            attrset(COLOR_PAIR(road_state->vr_lane_light));
            move(i, 17);
            printw("||");
        }
    // }
    move(0, 0);
    refresh();
}

void draw_traffic(RoadState_t *road_state)
{
    // int vl_lane[15]; // vertical top lane
    // int vr_lane[15]; // vertical bottom lane
    // int ht_lane[15]; // horizontal left lane
    // int hb_lane[15]; // horizontal right lane
    attrset(COLOR_PAIR(TRAFFIC_COLOR));

    for (int i = 0; i < 10; i++)
    {
        move(i, 13); // vertical left lane
        if (road_state->vl_lane[i])
            printw(".");
        else
        {
            int i2 = junc_zone_convert(i, "vl", "hb");
            if (i2 < 0 || (i2 >= 0 && road_state->hb_lane[i2] == 0))
            {
                printw(" ");
            }
            i2 = junc_zone_convert(i, "vl", "ht");
            if (i2 < 0 || (i2 >= 0 && road_state->ht_lane[i2] == 0))
            {
                printw(" ");
            }
        }
        printw(" ");
        move(i, 15); // vertical right lane
        if (road_state->vr_lane[9 - i])
            printw(".");
        else
        {
            int i2 = junc_zone_convert(i, "vr", "hb");
            if (i2 < 0 || (i2 >= 0 && road_state->hb_lane[i2] == 0))
            {
                printw(" ");
            }
            i2 = junc_zone_convert(i, "vr", "ht");
            if (i2 < 0 || (i2 >= 0 && road_state->ht_lane[i2] == 0))
            {
                printw(" ");
            }
        }
    }

    for (int i = 0; i < 15; i++)
    {
        move(4, 1 + 2 * i); // horizontal top lane
        if (road_state->ht_lane[i])
            printw(".");
        else
        {
            int i2 = junc_zone_convert(i, "ht", "vr");
            if (i2 < 0 || (i2 >= 0 && road_state->vr_lane[i2] == 0))
            {
                printw(" ");
            }
            i2 = junc_zone_convert(i, "ht", "vl");
            if (i2 < 0 || (i2 >= 0 && road_state->vl_lane[i2] == 0))
            {
                printw(" ");
            }
        }
        move(5, 1 + 2 * i); // horizontal bottom lane
        if (road_state->hb_lane[14 - i])
            printw(".");
        else
        {
            int i2 = junc_zone_convert(i, "hb", "vr");
            if (i2 < 0 || (i2 >= 0 && road_state->vr_lane[i2] == 0))
            {
                printw(" ");
            }
            i2 = junc_zone_convert(i, "hb", "vl");
            if (i2 < 0 || (i2 >= 0 && road_state->vl_lane[i2] == 0))
            {
                printw(" ");
            }
        }
    }
    move(0, 0);
    refresh();
}
