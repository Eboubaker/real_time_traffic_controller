#ifndef TYPES_H
#define TYPES_H

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

} RoadState_t;
#endif // TYPES_H
