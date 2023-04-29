#include "drawer.h"
#include "types.h"
#include <locale.h>
#include <stdlib.h>
#include "config.h"
#include "traffic.h"
#include <sys/ioctl.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define LOG_CAP 100
#define LOG_MSG_LEN 500

#define WIN_MIN_ROWS 18

char **log;
int next_log_i;
int log_size;
WINDOW *win;
struct winsize size;

WINDOW *get_win()
{
    return win;
}

void draw_controlls()
{
    move(size.ws_row - 1, 0);
    LOG("Controlls: %d\n", size.ws_row - 1);
    attrset(COLOR_PAIR(TEXT_COLOR));
    printw("Controlls: [flow-fraction:horiz:[w/W]:%.2f,vert:[e/E]:%.2f]", *get_htraffic(), *get_vtraffic());
    refresh();
}
void draw_traffic_passed()
{
    attrset(COLOR_PAIR(TEXT_COLOR));
    move(size.ws_row - 7, 0);
    printw("Traffic passed: %d", *get_passed_traffic());
    move(0, 0);
    refresh();
}
void draw_light_timers(RoadState_t *road_state)
{
    attrset(COLOR_PAIR(TEXT_COLOR));
    move(size.ws_row - 2, 0);
#define S_ARGS(x) road_state->x##_lane_red_time / 1000, road_state->x##_lane_green_time / 1000, road_state->x##_lane_yellow_time / 1000
    printw("  Horizontal top: red: %ds, green: %ds, yellow: %ds", S_ARGS(ht));
    move(size.ws_row - 3, 0);
    printw("  Horizontal bottom: red: %ds, green: %ds, yellow: %ds", S_ARGS(hb));
    move(size.ws_row - 4, 0);
    printw("  Vertical Left: red: %ds, green: %ds, yellow: %ds", S_ARGS(vl));
    move(size.ws_row - 5, 0);
    printw("  Vertical Right: red: %ds, green: %ds, yellow: %ds", S_ARGS(vr));
    move(size.ws_row - 6, 0);
    addstr("Light Timers:");
    refresh();
}

void fresh_screen(RoadState_t *road_state)
{
    clear();
    if (size.ws_row < WIN_MIN_ROWS)
    {
        move(0, 0);
        attrset(COLOR_PAIR(TEXT_COLOR));
        printw("WINDOW HEIGHT VERY LOW, RESIZE");
        refresh();
        return;
    }
    draw_light_timers(road_state);
    draw_traffic_passed();
    draw_controlls();
    draw_logs();
}

void init_drawer()
{
    next_log_i = 0;
    log_size = 0;
    log = malloc(sizeof(char *) * LOG_CAP);
    for (int i = 0; i < LOG_CAP; i++)
    {
        log[i] = malloc(sizeof(char) * LOG_MSG_LEN);
        log[i][0] = '\0';
    }

    setlocale(LC_ALL, "");

    // init ncurses screen
    win = initscr();
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);

    // init lane colors
    start_color();
    init_pair(LIGHT_GREEN, COLOR_GREEN, COLOR_BLACK);
    init_pair(LIGHT_YELLOW, COLOR_YELLOW, COLOR_BLACK);
    init_pair(LIGHT_RED, COLOR_RED, COLOR_BLACK);
    init_pair(TRAFFIC_COLLISION_COLOR, COLOR_RED, COLOR_BLACK);
    init_pair(TRAFFIC_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(NO_LIGHT_LANE_COLOR, COLOR_WHITE, COLOR_BLACK);
    init_pair(ROAD_COLOR, COLOR_BLACK, COLOR_BLACK);
    init_pair(TEXT_COLOR, COLOR_WHITE, COLOR_BLACK);
}
void push_log(char *in_msg)
{
    strcpy(log[next_log_i], in_msg);
    next_log_i++;
    if (next_log_i == LOG_CAP)
        next_log_i = 0;
    if (log_size != LOG_CAP)
        log_size++;
    draw_logs();
}
void draw_logs()
{
    int n = MIN(size.ws_row, log_size);
    attrset(COLOR_PAIR(TEXT_COLOR));
    for (int i = n - 1; i >= 0; i--)
    {
        move(size.ws_row - 1 - i, 40);
        printw(log[i]);
    }
    move(0, 0);
    refresh();
}

void draw_first(RoadState_t *road_state)
{
    redraw_fully(road_state);
}

void on_window_resize()
{
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &size);
    // clearok(win, true);
}

void redraw_fully(RoadState_t *road_state)
{
    if (size.ws_row < WIN_MIN_ROWS)
    {
        clear();
        move(0, 0);
        attrset(COLOR_PAIR(TEXT_COLOR));
        printw("WINDOW HEIGHT VERY LOW, RESIZE");
        refresh();
        return;
    }
    fresh_screen(road_state);
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

void draw_traffic_jump(RoadState_t *road_state, int i, char *lane)
{
    attrset(COLOR_PAIR(TRAFFIC_COLOR));

    if (strcmp(lane, "ht") == 0)
    {
        move(4, 1 + 2 * i);
        printw(" ");
        if (i != 14)
        {
            move(4, 1 + (2 * i) + 2);
            printw(CAR_GLYPH_HT);
        }
    }
    else if (strcmp(lane, "hb") == 0)
    {
        move(5, 2 * (14 - i) + 3);
        printw(" ");
        if (i != 14)
        {
            move(5, (2 * (14 - i - 1)) + 3);
            printw(CAR_GLYPH_HB);
        }
    }
    else if (strcmp(lane, "vl") == 0)
    {
        move(i, 13);
        printw(" ");
        if (i != 9)
        {
            move(i + 1, 13);
            printw(CAR_GLYPH_VL);
        }
    }
    else if (strcmp(lane, "vr") == 0)
    {
        move(i, 15);
        printw(" ");
        if (i != 9)
        {
            move(i + 1, 13);
            printw(CAR_GLYPH_VR);
        }
    }
    move(0, 0);
    refresh();
}

void draw_traffic(RoadState_t *road_state)
{
    // int vl_lane[15]; // vertical top lane
    // int vr_lane[15]; // vertical bottom lane
    // int ht_lane[15]; // horizontal left lane
    // int hb_lane[15]; // horizontal right lane

    for (int i = 0; i < 10; i++)
    {
        move(i, 13); // vertical left lane
        if (road_state->vl_lane[i])
        {
            attrset(COLOR_PAIR(TRAFFIC_COLOR));
            printw(CAR_GLYPH_VL);
        }
        else
        {
            int ihb = junc_zone_convert(i, "vl", "hb");
            int iht = junc_zone_convert(i, "vl", "ht");
            if (!(ihb >= 0 && road_state->hb_lane[ihb]) && !(iht >= 0 && road_state->ht_lane[iht]))
            {
                attrset(COLOR_PAIR(ROAD_COLOR));
                printw(" ");
            }
        }
        printw(" ");
        move(i, 15); // vertical right lane
        if (road_state->vr_lane[9 - i])
        {
            attrset(COLOR_PAIR(TRAFFIC_COLOR));
            printw(CAR_GLYPH_VR);
        }
        else
        {
            int ihb = junc_zone_convert(i, "vr", "hb");
            int iht = junc_zone_convert(i, "vr", "ht");
            if (!(ihb >= 0 && road_state->hb_lane[ihb]) && !(iht >= 0 && road_state->ht_lane[iht]))
            {
                attrset(COLOR_PAIR(ROAD_COLOR));
                printw(" ");
            }
        }
    }

    for (int i = 0; i < 15; i++)
    {
        move(4, 1 + 2 * i); // horizontal top lane
        if (road_state->ht_lane[i])
        {
            attrset(COLOR_PAIR(TRAFFIC_COLOR));
            printw(CAR_GLYPH_HT);
        }
        else
        {
            int ivr = junc_zone_convert(i, "ht", "vr");
            int ivl = junc_zone_convert(i, "ht", "vl");
            if (!(ivr >= 0 && road_state->vr_lane[ivr]) && !(ivl >= 0 && road_state->vl_lane[ivl]))
            {
                attrset(COLOR_PAIR(ROAD_COLOR));
                printw(" ");
            }
        }
        move(5, 1 + 2 * i); // horizontal bottom lane
        if (road_state->hb_lane[14 - i])
        {
            attrset(COLOR_PAIR(TRAFFIC_COLOR));

            printw(CAR_GLYPH_HB);
        }
        else
        {
            int ivr = junc_zone_convert(14 - i, "hb", "vr");
            int ivl = junc_zone_convert(14 - i, "hb", "vl");
            // LOG("i: %d cont: %d, move: %d %d, ivr: %d, ivl: %d, cond1: %d, cond2: %i\n", i, road_state->hb_lane[i], 5, 1 + 2 * i, ivr, ivl, !(ivr >= 0 && road_state->vr_lane[ivr]), !(ivl >= 0 && road_state->vl_lane[ivl]));
            if (!(ivr >= 0 && road_state->vr_lane[ivr]) && !(ivl >= 0 && road_state->vl_lane[ivl]))
            {
                attrset(COLOR_PAIR(ROAD_COLOR));
                printw(" ");
            }
        }
    }
    move(0, 0);
    refresh();
}
