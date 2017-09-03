#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__
#define _GNU_SOURCE
#include <semaphore.h>

typedef char *string;

typedef struct
{
    int argc;
    string *argv;
} command;

typedef struct
{
    int t0_dec;
    int dt_dec;
    int dl_dec;
    string name;
    int trace_line;
    sem_t sem;
} process;

typedef struct
{
    unsigned int cpu_count;
} scheduler_def;

#endif
