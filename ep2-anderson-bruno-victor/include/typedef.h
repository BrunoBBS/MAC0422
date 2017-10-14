/* ------------------------------------------------
EP2 - MAC0422 - Sistemas Operacionais 2s2017

Authors:

Anderson Andrei da Silva , 8944025
Bruno Boaventura Scholl, 9793586
Seiji
------------------------------------------------- */

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
    int r; // Real Time
    bool e; // Extra mode
    bool d; // Debug mode
} globals;

#endif
