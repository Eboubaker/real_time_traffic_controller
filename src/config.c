#include <stdlib.h>
#include <stdio.h>

int generated_traffic;
int passed_traffic;

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
}

FILE* get_log_file() {
    return log_file;
}