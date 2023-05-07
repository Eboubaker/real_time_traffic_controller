#include <stdlib.h>
#include <stdio.h>
#include "config.h"
#include "road.h"
#include "string.h"

int generated_traffic;
int passed_traffic;
int simulation_speed;

int *get_sim_speed()
{
    return &simulation_speed;
}
// globally share these values
int *get_generated_traffic()
{
    return &generated_traffic;
}
int *get_passed_traffic()
{
    return &passed_traffic;
}

FILE *log_file;
void init_config()
{
    log_file = fopen("log", "w");
    generated_traffic = 0;
    passed_traffic = 0;
    simulation_speed = DEFAULT_SIMULATION_SPEED;
}

FILE *get_log_file()
{
    return log_file;
}
