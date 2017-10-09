#ifndef __RIDER_H__
#define __RIDER_H__

#include "typedef.h"
#include "velodrome.h"
#include <pthread.h>
#include <semaphore.h>

#define V30KM 1 //one meter every 120ms
#define V60KM 2
#define V90KM 3

// Declarations for circular dependency
void create_velodrome(
    Velodrome *velodrome_ptr,
    uint length,
    uint rider_cnt,
    uint turns);
void destroy_velodrome(Velodrome *velodrome_ptr);
int max_rider_speed(
    Velodrome *velodrome_ptr,
    Rider rider);
void complete_turn(
    Velodrome *velodrome_ptr,
    Rider rider);
bool can_rider_break(
    Velodrome *velodrome_ptr);

struct Rider
{
    // Constants set by velodrome
    int id;
    pthread_t rider_t;
    Velodrome velodrome;

    // Set by rider
    int speed;
    bool broken;
    int score;
    // Absolute position
    int total_dist;
    int lane;
    int step;
    uint turn;
    sem_t turn_done;
};

typedef struct Rider *Rider;

/*
 *Chage randomly the rider's speed using defined probilities
 */
int change_speed(Rider rider, bool V90);

/*
 *Main function of rider
 */
void *ride(void *args);

/*
 *Calculates if breaks based on chance
 */
bool will_break(Rider rider);

/*
 *Calculates if will change and wich adjacent lane to change
 *The rider has a 50% chance of changing lanes and 50% of chance to go to
 *rignt lane case will change lanes
 */
char change_lane(Rider rider);

#endif
