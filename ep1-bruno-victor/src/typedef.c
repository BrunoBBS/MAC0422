#include "typedef.h"

unsigned int getttime()
{
    struct rusage usage;
    struct timeval time_spent;
    getrusage(RUSAGE_THREAD, &usage);
    timeradd(&usage.ru_utime, &usage.ru_stime, &time_spent);
    return (time_spent.tv_sec * 1000) + (time_spent.tv_usec / 1000);
}

unsigned int getwtime()
{
    struct timeval curr_time, delta;
    gettimeofday(&curr_time, NULL);
    timersub(&curr_time, &globals.t0, &delta);

    long long int delta_m = (delta.tv_sec * 1000) + (delta.tv_usec / 1000);
    if (delta_m < 0)
        delta_m = 0;
    return (unsigned int) delta_m;
}
