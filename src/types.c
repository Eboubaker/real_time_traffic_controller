#include "types.h"
#include "stdlib.h"
#include "stdio.h"
#include "string.h"

void init_road_state(RoadState_t *road_state)
{
    memset(road_state, 0, sizeof(RoadState_t));
    road_state->hb_lane_light = LIGHT_RED;
    road_state->ht_lane_light = LIGHT_RED;
    road_state->vl_lane_light = LIGHT_GREEN;
    road_state->vr_lane_light = LIGHT_GREEN;

    road_state->hb_lane_red_time_ms = DEFAULT_RED_LIGHT_TIME_MS;
    road_state->hb_lane_green_time_ms = DEFAULT_GREEN_LIGHT_TIME_MS;
    road_state->hb_lane_yellow_time_ms = DEFAULT_YELLOW_LIGHT_TIME_MS;
    road_state->ht_lane_red_time_ms = DEFAULT_RED_LIGHT_TIME_MS;
    road_state->ht_lane_green_time_ms = DEFAULT_GREEN_LIGHT_TIME_MS;
    road_state->ht_lane_yellow_time_ms = DEFAULT_YELLOW_LIGHT_TIME_MS;
    road_state->vl_lane_red_time_ms = DEFAULT_RED_LIGHT_TIME_MS;
    road_state->vl_lane_green_time_ms = DEFAULT_GREEN_LIGHT_TIME_MS;
    road_state->vl_lane_yellow_time_ms = DEFAULT_YELLOW_LIGHT_TIME_MS;
    road_state->vr_lane_red_time_ms = DEFAULT_RED_LIGHT_TIME_MS;
    road_state->vr_lane_green_time_ms = DEFAULT_GREEN_LIGHT_TIME_MS;
    road_state->vr_lane_yellow_time_ms = DEFAULT_YELLOW_LIGHT_TIME_MS;
    generate_traffic(road_state);
}
