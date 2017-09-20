#ifndef __VELODROME_H__
#define __VELODROME_H__

#include <stdlib.h>

#include "typedef.h"
#include "rider.h"

typedef struct Rider Rider;

// This is a struct representing a counter-clockwise velodrome
struct Velodrome 
{
    // Length of velodrome, in meters
    int length;
    
    // Number of active riders
    uint rider_cnt;
    
    // This is the track. It stores rider ids (or -1)

    // This is accessed like this:
    // pista[meter][lane]
    // innermost lane is lane = 0, outermost is lane = 9
    int  **pista;

    // This is a vector of riders
    // riders[i] is pointer to rider with id i
    Rider *riders;
};

typedef struct Velodrome * Velodrome;

// Creates new velodrome struct
void create_velodrome (
        Velodrome *velodrome_ptr,
        uint length,
        uint rider_cnt);

// Destroys velodrome object
void destroy_velodrome (Velodrome *velodrome_ptr);

// Returns max speed cyclist can 
int max_rider_speed (
        Velodrome *velodrome_ptr,
        Rider *rider);

// If there are more than 5 riders
bool can_rider_break (
        Velodrome *velodrome_ptr);
#endif
