#ifndef __TYPEDEF_H__
#define __TYPEDEF_H__
#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Type definitions
typedef char *string;

typedef struct command
{
    int argc;
    string *argv;
} command;

typedef struct process
{
    int t0_dec;
    int dt_dec;
    int dl_dec;
    int rem_milli;
    string name;
    int trace_line;
    sem_t sem;
    pthread_t thread;
} process;

typedef struct scheduler_def
{
    char ended;
    int syst0;
    unsigned int cpu_count;
} scheduler_def;

// Global structures
struct globals {
    char debug;
    char extra;
    struct timeval t0;
} globals;

// Some very useful functions
unsigned int getttime ();
unsigned int getwtime ();

#endif
