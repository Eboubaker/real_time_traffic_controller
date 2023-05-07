#ifndef CONFIG_H
#define CONFIG_H

#include <stdio.h>

#define DEFAULT_H_LOAD .60
#define DEFAULT_V_LOAD .25


#define V_LANE_CELLS 8
#define H_LANE_CELLS 8

#define V_LANE_CAP 2 * V_LANE_CELLS
#define H_LANE_CAP 2 * H_LANE_CELLS

#define LOG(fmt, ...)                                        \
    fprintf(get_log_file(), fmt __VA_OPT__(, ) __VA_ARGS__); \
    fflush(get_log_file())

#define TEXT_COLOR 8
#define ROAD_COLOR 7

#define NO_LIGHT_LANE_COLOR 6

#define TRAFFIC_COLLISION_COLOR 5
#define TRAFFIC_COLOR 4

#define LIGHT_RED 3
#define LIGHT_YELLOW 2
#define LIGHT_GREEN 1

#define DEFAULT_GREEN_LIGHT_TIME_MS 10000
#define DEFAULT_RED_LIGHT_TIME_MS 10000
#define DEFAULT_YELLOW_LIGHT_TIME_MS 0

#define DEFAULT_SIMULATION_SPEED 4

#include "ncursesw/ncurses.h"

#define CAR_GLYPH_HT "#"
#define CAR_GLYPH_HB "#"
#define CAR_GLYPH_VL "#"
#define CAR_GLYPH_VR "#"

int *get_sim_speed();
int *get_generated_traffic();
int *get_passed_traffic();
void init_config();
FILE *get_log_file();

#include "types.h"

#endif // CONFIG_H