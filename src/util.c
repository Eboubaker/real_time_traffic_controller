#include "stdlib.h"
#include "stdbool.h"

#include "util.h"
#include "stdarg.h"
#include <string.h>
#include <stdio.h>

bool rand_prob(float prob)
{
    float r = (float)rand() / RAND_MAX;
    return r < prob;
}

int rrand_int(int minimum_number, int max_number)
{
    return rand() % (max_number + 1 - minimum_number) + minimum_number;
}

double get_seconds_fractions()
{
    struct timeval now;
    gettimeofday(&now, NULL);
    return (now.tv_usec + (unsigned long long)now.tv_sec * 1000000) / 1000000.0L;
}