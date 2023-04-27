#include "stdlib.h"
#include "stdbool.h"

#include "util.h"
#include "stdarg.h"

bool rand_prob(float prob)
{
    float r = (float)rand() / RAND_MAX;
    if (r < prob)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int rrand_int(int minimum_number, int max_number)
{
    return rand() % (max_number + 1 - minimum_number) + minimum_number;
}