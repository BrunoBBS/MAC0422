#ifndef __RIDER_H__
#define __RIDER_H__

#include "typedef.h"
#include "velodrome.h"
#include <pthread.h>

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

/*
 *Chage randomly the rider's speed using defined probilities
 */
int change_speed(Rider *rider, bool V90);

/*
 *Main function of rider
 */
void ride(void *args);

/*
 *Calculates if breaks based on chance
 */
bool will_break(Rider *rider);

/*
 *Calculates if will change and wich adjacent lane to change
 *The rider has a 50% chance of changing lanes and 60% of chance to go to 
 *rignt lane case will change lanes
 */
void change_lane(Rider *rider);

#endif
