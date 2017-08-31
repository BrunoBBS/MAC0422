#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__

#include <semaphore.h>

typedef char *string;

typedef struct
{
    int argc;
    string *argv;
} command;

typedef struct
{
    unsigned int t0_dec;
    unsigned int dt_dec;
    unsigned int dl_dec;
    string name;
    sem_t sem;
} process;

typedef struct
{
    unsigned int cpu_count;
} scheduler_def;

#endif
