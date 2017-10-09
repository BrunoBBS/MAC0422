#ifndef __TYPEDEFS_H__
#define __TYPEDEFS_H__

#include <stdio.h>

typedef unsigned int uint;
typedef unsigned char bool;
#define true 't'
#define false 'f'

typedef struct Velodrome *Velodrome;
typedef struct Rider *Rider;

struct
{
    bool e; // Extra mode
    bool d; // Debug mode
} globals;

#endif
