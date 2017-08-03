#include "ep1sh/commands/date.h"

int date_c ()
{
    timeval time;
    unsigned int weekday;
    char* month = "AUG";
    unsigned int day;
    unsigned int hour;
    unsigned int min;
    unsigned int sec;
    char* tz = "BRT";
    unsigned int year;
    if (syscall(SYS_gettimeofday, &time, NULL))
        return -1;

    sec = time.tv_sec % 60;
    min = (time.tv_sec % 3600) / 60;
    hour = (time.tv_sec % 86400) / 3600;
    year = 1970 + time.tv_sec / 31556926;
    weekday = (time.tv_sec / 86400) % 7;

    printf("%s %s %2u %02u:%02u:%02u %s %04u\n",
            weekdays[weekday],
            month,
            day,
            hour,
            min,
            sec,
            tz,
            year);
    return 0;
}
