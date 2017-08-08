#include "ep1sh/commands/date.h"

char weekdays[7][4] = {
    "Thu",
    "Fri",
    "Sat",
    "Sun",
    "Mon",
    "Tue",
    "Wed",
};

char months[12][4] = {
    "Jan",
    "Feb",
    "Mar",
    "Apr",
    "May",
    "Jun",
    "Jul",
    "Aug",
    "Sep",
    "Oct",
    "Nov",
    "Dec",
};

typedef unsigned int uint;

int date_c ()
{
    timeval time;
    char* month = "AUG";
    char* tz = "BRT";
    uint day;

    if (syscall(SYS_gettimeofday, &time, NULL))
        return -1;

    uint sec = time.tv_sec % 60;
    uint min = (time.tv_sec % 3600) / 60;
    uint hour = (time.tv_sec % 86400) / 3600;
    uint year = 1970 + time.tv_sec / 31556926;
    uint weekday = (time.tv_sec / 86400) % 7;

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
