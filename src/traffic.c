#include "types.h"
#include "stdlib.h"

#include "util.h"
#include "traffic.h"
#include "string.h"
#include "drawer.h"
#include "config.h"

float htraffic = .25;
float vtraffic = .25;


float *get_htraffic() {
    return &htraffic;
}
float *get_vtraffic()
{
    return &vtraffic;
}

void generate_traffic(RoadState_t *road_state)
{
    if (!road_state->ht_lane[0] && rand_prob(htraffic))
    {
        road_state->ht_lane[0] = 1;
        (*get_generated_traffic())++;
    }
    if (!road_state->hb_lane[0] && rand_prob(htraffic))
    {
        road_state->hb_lane[0] = 1;
        (*get_generated_traffic())++;
    }
    if (!road_state->vl_lane[0] && rand_prob(vtraffic))
    {
        road_state->vl_lane[0] = 1;
        (*get_generated_traffic())++;
    }
    if (!road_state->vr_lane[0] && rand_prob(vtraffic))
    {
        road_state->vr_lane[0] = 1;
        (*get_generated_traffic())++;
    }
}

bool inside_junc_zone(int i, const char *lane)
{
    if (strcmp(lane, "hb") == 0)
        return i > 5 && i < 10;
    else if (strcmp(lane, "ht") == 0)
    {
        return i > 4 && i < 9;
    }
    else if (strcmp(lane, "vl") == 0)
    {
        return i > 3 && i < 6;
    }
    else if (strcmp(lane, "vr") == 0)
    {
        return i > 3 && i < 6;
    }
    else
    {
        char c[200];
        sprintf(c, "%s: uknown lane: %s", __func__, lane);
        draw_fatal_exit(c);
        return false; // unreachable
    }
}

int junc_zone_convert(int i, const char *from_lane, const char *to_lane)
{
    if (i == 4 && strcmp(from_lane, "vl") == 0 && strcmp(to_lane, "ht") == 0)
        return 6;
    else if (i == 6 && strcmp(from_lane, "ht") == 0 && strcmp(to_lane, "vl") == 0)
        return 4;
    else if (i == 5 && strcmp(from_lane, "vr") == 0 && strcmp(to_lane, "ht") == 0)
        return 7;
    else if (i == 7 && strcmp(from_lane, "ht") == 0 && strcmp(to_lane, "vr") == 0)
        return 5;
    else if (i == 7 && strcmp(from_lane, "hb") == 0 && strcmp(to_lane, "vr") == 0)
        return 4;
    else if (i == 4 && strcmp(from_lane, "vr") == 0 && strcmp(to_lane, "hb") == 0)
        return 7;
    else if (i == 5 && strcmp(from_lane, "vl") == 0 && strcmp(to_lane, "hb") == 0)
        return 8;
    else if (i == 8 && strcmp(from_lane, "hb") == 0 && strcmp(to_lane, "vl") == 0)
        return 5;

    return -1;
}

bool collision(RoadState_t *road_state, int i, char *lane)
{
    if (strcmp(lane, "vl") == 0)
    {
    }
    return false;
}

void traffic_step(RoadState_t *road_state)
{
    int i2;
    for (int i = 14; i >= 0; i--)
    {
        if (road_state->ht_lane[i])
        {
            if (i != 14 && !inside_junc_zone(i, "ht") && inside_junc_zone(i + 1, "ht")) // check if we will step into junction in next step
            {
                if (road_state->ht_lane_light == LIGHT_RED || road_state->ht_lane_light == LIGHT_YELLOW)
                    continue; // traffic controls does not allow this car to step into junction
            }
            if (i != 14)
            {
                i2 = junc_zone_convert(i + 1, "ht", "vl");
                if (i2 >= 0 && road_state->vl_lane[i2])    // check if next car position is empty
                    continue;                              // prevent traffic collision
                i2 = junc_zone_convert(i + 1, "ht", "vr"); //
                if (i2 >= 0 && road_state->vr_lane[i2])    // check if next car position is empty
                    continue;                              // prevent traffic collision
                if (road_state->ht_lane[i + 1])            // check if next car position is empty
                    continue;                              // prevent traffic collision
            }

            // car can safely move one step
            road_state->ht_lane[i] = 0; // clear old position
            if (i != 14)
                road_state->ht_lane[i + 1] = 1; // set to next position
            else
                (*get_passed_traffic())++;
            // draw_traffic_jump(road_state, i, "ht");
        }
    }

    for (int i = 14; i >= 0; i--)
    {
        if (road_state->hb_lane[i])
        {
            if (i != 14 && !inside_junc_zone(i, "hb") && inside_junc_zone(i + 1, "hb")) // check if we will step into junction in next step
            {
                if (road_state->hb_lane_light == LIGHT_RED || road_state->hb_lane_light == LIGHT_YELLOW)
                    continue; // traffic controls does not allow this car to step into junction
            }
            if (i != 14)
            {
                i2 = junc_zone_convert(i + 1, "hb", "vl");
                if (i2 >= 0 && road_state->vl_lane[i2])    // check if next car position is empty
                    continue;                              // prevent traffic collision
                i2 = junc_zone_convert(i + 1, "hb", "vr"); //
                if (i2 >= 0 && road_state->vr_lane[i2])    // check if next car position is empty
                    continue;                              // prevent traffic collision
                if (road_state->hb_lane[i + 1])            // check if next car position is empty
                    continue;                              // prevent traffic collision
            }

            // car can safely move one step
            road_state->hb_lane[i] = 0; // clear old position
            if (i != 14)
                road_state->hb_lane[i + 1] = 1; // set to next position
            else
                (*get_passed_traffic())++;
            // draw_traffic_jump(road_state, i, "hb");
        }
    }

    for (int i = 9; i >= 0; i--)
    {
        if (road_state->vl_lane[i])
        {
            if (i != 9 && !inside_junc_zone(i, "vl") && inside_junc_zone(i + 1, "vl")) // check if we will step into junction in next step
            {
                if (road_state->vl_lane_light == LIGHT_RED || road_state->vl_lane_light == LIGHT_YELLOW)
                    continue; // traffic controls does not allow this car to step into junction
            }
            if (i != 9)
            {
                i2 = junc_zone_convert(i + 1, "vl", "ht");
                if (i2 >= 0 && road_state->ht_lane[i2])    // check if next car position is empty
                    continue;                              // prevent traffic collision
                i2 = junc_zone_convert(i + 1, "vl", "hb"); //
                if (i2 >= 0 && road_state->hb_lane[i2])    // check if next car position is empty
                    continue;                              // prevent traffic collision
                if (road_state->vl_lane[i + 1])            // check if next car position is empty
                    continue;                              // prevent traffic collision
            }

            // car can safely move one step
            road_state->vl_lane[i] = 0; // clear old position
            if (i != 9)
                road_state->vl_lane[i + 1] = 1; // set to next position
            else
                (*get_passed_traffic())++;
            // draw_traffic_jump(road_state, i, "hb");
        }
    }

    for (int i = 9; i >= 0; i--)
    {
        if (road_state->vr_lane[i])
        {
            if (i != 9 && !inside_junc_zone(i, "vr") && inside_junc_zone(i + 1, "vr")) // check if we will step into junction in next step
            {
                if (road_state->vr_lane_light == LIGHT_RED || road_state->vr_lane_light == LIGHT_YELLOW)
                    continue; // traffic controls does not allow this car to step into junction
            }
            if (i != 9)
            {
                i2 = junc_zone_convert(i + 1, "vr", "ht");
                if (i2 >= 0 && road_state->ht_lane[i2])    // check if next car position is empty
                    continue;                              // prevent traffic collision
                i2 = junc_zone_convert(i + 1, "vr", "hb"); //
                if (i2 >= 0 && road_state->hb_lane[i2])    // check if next car position is empty
                    continue;                              // prevent traffic collision
                if (road_state->vr_lane[i + 1])            // check if next car position is empty
                    continue;                              // prevent traffic collision
            }

            // car can safely move one step
            road_state->vr_lane[i] = 0; // clear old position
            if (i != 9)
                road_state->vr_lane[i + 1] = 1; // set to next position
            else
                (*get_passed_traffic())++;
            // draw_traffic_jump(road_state, i, "hb");
        }
    }
}