#ifndef __CHOWN_H__
#define __CHOWN_H__

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/syscall.h>
#include <unistd.h>

int chown_c (char*, char*);

#endif
