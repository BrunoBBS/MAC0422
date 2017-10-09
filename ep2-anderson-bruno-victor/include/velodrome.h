#ifndef __VELODROME_H__
#define __VELODROME_H__

#include <stdlib.h>

#include "rider.h"
#include "typedef.h"
#include <semaphore.h>

typedef struct Rider *Rider;

// Declarations for circular dependency
int change_speed(Rider rider, bool V90);
void *ride(void *args);
bool will_break(Rider rider);
char change_lane(Rider rider);

// This is a struct representing a counter-clockwise velodrome
struct Velodrome
{
    // Length of velodrome, in meters
    int length;

    // Total number of riders
    uint rider_cnt;

    // Number of active riders
    uint a_rider_cnt;

    // Turn count
    uint turn_cnt;

    // This is the track. It stores rider ids (or -1)

    // This is accessed like this:
    // pista[meter][lane]
    // innermost lane is lane = 0, outermost is lane = 9
    int **pista;

    // This is a vector of riders
    // riders[i] is pointer to rider with id i
    struct Rider *riders;

    // This is the barrier for riders to wait for start
    pthread_barrier_t *start_barrier;
    // All riders write on the same array
    sem_t *velodrome_sem;

    // How much time passes in one barrier round
    int round_time;

    //Table of placings per turn. Size [n][160].
    int **placings;
};

typedef struct Velodrome *Velodrome;

// Creates new velodrome struct
void create_velodrome(
    Velodrome *velodrome_ptr,
    uint length,
    uint rider_cnt,
    uint turns);

// Destroys velodrome object
void destroy_velodrome(Velodrome *velodrome_ptr);

// Returns max speed cyclist can
int max_rider_speed(
    Velodrome *velodrome_ptr,
    Rider rider);

// Returns rider in front of parameter
Rider rider_in_front(Velodrome *velodrome_ptr, Rider behind);

// Scores rider for turn
void complete_turn(
    Velodrome *velodrome_ptr,
    Rider rider);

// If there are more than 5 riders
bool can_rider_break(
    Velodrome *velodrome_ptr);

// Mark placings to all turns
void mark_placing(
    Velodrome *velodrome_ptr,
    Rider rider);

// Verify if the turn is a sprint
bool is_sprint(Velodrome *velodrome_ptr,
    Rider rider);

// To rider score
void scoring(Velodrome *velodrome_ptr,
    Rider rider);

#endif
