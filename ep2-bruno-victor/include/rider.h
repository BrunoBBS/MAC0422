#ifndef __RIDER_H__
#define __RIDER_H__

#include "typedef.h"
#include <pthread.h>
#include "velodrome.h"

#define V30KM 1 //one meter every 120ms
#define V60KM 2
#define V90KM 3

struct Rider
{
    int id;
    int speed;
    bool broken;
    pthread_t rider_t;
    // Absolute position
    int total_dist;
    int lane;
    // Just a reference for vallig velodrome functions
    Velodrome *velodrome;
};

typedef struct Rider Rider;

// Chage randomly the rider's speed using defined probilities
int change_speed(Rider *rider);

// Main function of rider
void ride(void *args);

// Calculates if breaks based on chance
bool will_break(Rider *rider);

// Calculates if will change and wich adjacent lane to change
void change_lane(Rider *rider);

#endif
