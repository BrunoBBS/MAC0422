/* ------------------------------------------------
EP2 - MAC0422 - Sistemas Operacionais 2s2017

Authors:

Anderson Andrei da Silva , 8944025
Bruno Boaventura Scholl, 9793586
Victor Seiji Hariki, 9793694
------------------------------------------------- */

#ifndef __VELODROME_H__
#define __VELODROME_H__

#include <stdlib.h>

#include "rider.h"
#include "typedef.h"
#include <semaphore.h>
#include <stdlib.h>

typedef struct Rider *Rider;

// Declarations for circular dependency
int get_pos(Rider rider);
int change_speed(Rider rider);
void *ride(void *args);
bool will_break(Rider rider);
char change_lane(Rider rider);
void *coordinator(void *args);

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
    uint lap_cnt;

    // This is the track. It stores rider ids (or -1)

    // This is accessed like this:
    // pista[meter][lane]
    // innermost lane is lane = 0, outermost is lane = 9
    int **pista;

    // This is a vector of riders
    // riders[i] is pointer to rider with id i
    struct Rider *riders;

    // Coordinator thread
    pthread_t coordinator_t;

    // This is the barrier for riders to wait for start
    pthread_barrier_t start_barrier;

    // All riders write on the same array
    sem_t velodrome_sem;

    // How much time passes in one barrier round
    int round_time;

    // Array of arrive at barrier flags
    sem_t *arrive;

    // Flag to pass barrier
    sem_t *continue_flag;

    // Remaining riders
    int *placings;

    // Stack of placings by round
    int *s_indexes;
    uint **placings_v;

    // Random generator semaphor
    sem_t rand_sem;

    // Scores semaphore
    sem_t score_sem;
};

typedef struct Velodrome *Velodrome;

// Creates new velodrome struct
void create_velodrome(
    Velodrome *velodrome_ptr, uint length, uint rider_cnt, uint lap_cnt);

// Destroys velodrome object
void destroy_velodrome(Velodrome *velodrome_ptr);

// Returns max speed cyclist can
int max_rider_speed(Velodrome *velodrome_ptr, Rider rider);

// Returns rider in front of parameter
Rider rider_in_front(Rider behind);

// If there are more than 5 riders
bool can_rider_break(Velodrome *velodrome_ptr);

// Mark sprints
void mark_sprint(Rider rider, int lap);

    // Mark overtakes
void mark_overtake(Rider rider);

// Mark lap
void mark_lap(Rider rider, int lap);

// Verify if the turn is a sprint
bool is_sprint(Velodrome *velodrome_ptr, Rider rider);

// Compare the score to qsort()
int compare_scores(const void * a, const void * b);

// Print infos how scores or positions
void print_info(uint *id, Velodrome velodrome_ptr, int lap); 

#endif
