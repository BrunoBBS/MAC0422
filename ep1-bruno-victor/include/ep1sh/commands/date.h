#ifndef __DATE_H__
#define __DATE_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

typedef struct
{
    long unsigned int tv_sec;
    long unsigned int tv_usec;
} timeval;

int date_c ();

#endif
