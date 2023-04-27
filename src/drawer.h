#ifndef DRAWER_H
#define DRAWER_H

#include "types.h"
#include "ncurses.h"

void draw_lane_lights(RoadState_t *road_state);
void draw_traffic(RoadState_t *road_state);
WINDOW *draw_first(RoadState_t *road_state);
void draw_fatal_exit(char *msg);
void redraw_fully(RoadState_t *road_state);

#endif // DRAWER_H
