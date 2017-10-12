#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <stdio.h>

typedef unsigned int uint;
typedef unsigned char bool;
#define true 1
#define false 0

typedef struct Velodrome *Velodrome;
typedef struct Rider *Rider;

struct
{
    bool r; // Real Time
    bool e; // Extra mode
    bool d; // Debug mode
} globals;

#endif
