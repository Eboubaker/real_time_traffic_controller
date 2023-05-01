#ifndef TYPES_H
#define TYPES_H

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#include "config.h"


typedef struct
{
    // lanes: 0 for empty 1 for car
    int vl_lane[V_LANE_CAP]; // vertical left lane, printed from top to bottom
    int vr_lane[V_LANE_CAP]; // vertical right lane, printed from bottom to top
    int ht_lane[H_LANE_CAP]; // horizontal top lane, printed from right to left
    int hb_lane[H_LANE_CAP]; // horizontal bottom lane, printed from left to right

    // light control 
    // 1: green 
    // 2: yellow 
    // 3: red
    int hb_lane_light; // horizontal bottom lane light
    int ht_lane_light; // horizontal top lane light

    int vr_lane_light; // vertical right lane light
    int vl_lane_light; // vertical left lane light

    // light timers for each lane in ms.
    int hb_lane_red_time_ms;
    int hb_lane_green_time_ms;
    int hb_lane_yellow_time_ms;
    int ht_lane_red_time_ms;
    int ht_lane_green_time_ms;
    int ht_lane_yellow_time_ms;
    int vl_lane_red_time_ms;
    int vl_lane_green_time_ms;
    int vl_lane_yellow_time_ms;
    int vr_lane_red_time_ms;
    int vr_lane_green_time_ms;
    int vr_lane_yellow_time_ms;
} RoadState_t;
void init_road_state(RoadState_t *road_state);
#endif // TYPES_H
