#ifndef TYPES_H
#define TYPES_H

#define MAX(x, y) (((x) > (y)) ? (x) : (y))
#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#include "config.h"

typedef struct
{
    // lanes: 0 for empty 1 for car
    int vl_lane[10]; // vertical left lane, printed from top to bottom
    int vr_lane[10]; // vertical right lane, printed from bottom to top
    int ht_lane[15]; // horizontal top lane, printed from right to left
    int hb_lane[15]; // horizontal bottom lane, printed from left to right

    // light control 
    // 1: green 
    // 2: yellow 
    // 3: red
    int hb_lane_light; // horizontal bottom lane light
    int ht_lane_light; // horizontal top lane light

    int vr_lane_light; // vertical right lane light
    int vl_lane_light; // vertical left lane light

    // light timers for each lane in ms.
    int hb_lane_red_time;
    int hb_lane_green_time;
    int hb_lane_yellow_time;
    int ht_lane_red_time;
    int ht_lane_green_time;
    int ht_lane_yellow_time;
    int vl_lane_red_time;
    int vl_lane_green_time;
    int vl_lane_yellow_time;
    int vr_lane_red_time;
    int vr_lane_green_time;
    int vr_lane_yellow_time;
} RoadState_t;
#endif // TYPES_H
