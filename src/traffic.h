#include "types.h"

void generate_traffic(RoadState_t *road_state);
void traffic_step(RoadState_t *road_state);
int junc_zone_convert(int i, const char *from_lane, const char *to_lane);
float *get_htraffic();
float *get_vtraffic();
