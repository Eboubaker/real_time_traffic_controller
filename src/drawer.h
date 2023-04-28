#ifndef DRAWER_H
#define DRAWER_H

#include "types.h"
#include "ncurses.h"

void draw_lane_lights(RoadState_t *road_state);
void draw_traffic(RoadState_t *road_state);
void draw_first(RoadState_t *road_state);
void draw_fatal_exit(char *msg);
void init_drawer();
void on_window_resize();
void draw_logs();
void push_log(char *in_msg);
void redraw_fully(RoadState_t *road_state);
void draw_traffic_jump(RoadState_t *road_state, int i, char *lane);
WINDOW *get_win();
void fresh_screen(RoadState_t *road_state);
#endif // DRAWER_H
