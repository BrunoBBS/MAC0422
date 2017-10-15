/* ------------------------------------------------
EP2 - MAC0422 - Sistemas Operacionais 2s2017

Authors:

Anderson Andrei da Silva , 8944025
Bruno Boaventura Scholl, 9793586
Victor Seiji Hariki, 9793694
------------------------------------------------- */

#ifndef __RIDER_H__
#define __RIDER_H__

#include "typedef.h"
#include "velodrome.h"
#include <pthread.h>
#include <semaphore.h>

#define V30KM 120 //one meter every 120ms
#define V60KM 60  //one meter every 60ms
#define V90KM 40  //one meter every 40ms

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
void mark_overtake(Velodrome velodrome);

struct Rider
{
    // Constants set by velodrome
    int id;
    pthread_t rider_t;
    Velodrome velodrome;

    // Set by rider
    int speed;
    bool broken;
    bool finished;
    int score;
    // Absolute position
    int total_dist;
    int lane;
    int step_time;
    uint turn;
    int *overtake;
};

typedef struct Rider *Rider;

/*
 *Calculates rider position in the velodrome
 */
int get_pos(Rider rider);

/*
 *Change randomly the rider's speed using defined probilities
 */
int change_speed(Rider rider);

/*
 *Main function of rider
 */
void *ride(void *args);

/*
 *Calculates if breaks based on chance
 */
bool will_break(Rider rider);

/*
 *Writes down the rider movement
 */
void step(char dir, Rider rider, Velodrome vel);

/*
 *Barrier Coordinator function
 */
void *coordinator(void *args);

/*
 *Calculates if will change and wich adjacent lane to change
 *The rider has a 50% chance of changing lanes and 50% of chance to go to
 *rignt lane case will change lanes
 */
char change_lane(Rider rider);

#endif
