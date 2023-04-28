#include <stdio.h>

#define LOG(fmt, ...)                                    \
    fprintf(get_log_file(), fmt __VA_OPT__(,)__VA_ARGS__); \
    fflush(get_log_file())

// define language, give syntax, example how to use language, most important thing is example,


#define TEXT_COLOR 8
#define ROAD_COLOR 7

#define NO_LIGHT_LANE_COLOR 6

#define TRAFFIC_COLLISION_COLOR 5
#define TRAFFIC_COLOR 4

#define LIGHT_RED 3
#define LIGHT_YELLOW 2
#define LIGHT_GREEN 1

#define GREEN_LIGHT_TIME_MS 8000
#define RED_LIGHT_TIME_MS 10000
#define YELLOW_LIGHT_TIME_MS 2000

#define CAR_GLYPH "#"

int *get_generated_traffic();
int *get_passed_traffic();
void init_config();
FILE *get_log_file();