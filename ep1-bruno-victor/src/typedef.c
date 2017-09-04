#include "typedef.h"

unsigned int getttime()
{
    struct rusage usage;
    struct timeval time_spent;
    getrusage(RUSAGE_THREAD, &usage);
    timeradd(&usage.ru_utime, &usage.ru_stime, &time_spent);
    return (time_spent.tv_sec * 1000) + (time_spent.tv_usec / 1000);
}
