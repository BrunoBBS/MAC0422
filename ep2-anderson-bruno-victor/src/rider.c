#include "rider.h"
#include <pthread.h>
#include <stdlib.h>

const int break_chance = 1;
const int v90_chance = 20;
const int ch_lane_l_chance = 60;
// Chance when speed was 30, otherwise is 50/50
float v30_chance = 0.3;
//TODO Scoring system
// Chage randomly the rider's speed using defined probilities
int change_speed(Rider rider, bool V90)
{
    int p = rand() % 100;
    if (!V90)
    {
        if (rider->speed == V60KM)
        {
            v30_chance = 50;
        }
        return p < v30_chance ? V30KM : V60KM;
    }
    else
    {
        return p < v90_chance ? V90KM : rider->speed;
    }
}

// Main function of rider
void *ride(void *args)
{
    //TODO
    //wait start
    //if lap comleted:
    //  ->choose new speed
    //  ->checks if scores
    //  ->if lap is multiple of 15:
    //      ->decide if breaks
    //go foraward, left or right:
    //   ->if is a rider in front:
    //      ->wait rider in front, left or right do its turn
    //      ->chacks max speed poddible and reduce if needed
    //notify rider behind semaphore
    //notify global barrier
    //wait global barrier

    Rider myself = (Rider)args;
    Velodrome vel = myself->velodrome;
    
    if (globals.e)
        printf("rider:l%3d -> Created rider %d\n", __LINE__, myself->id);

    //wait start
    pthread_barrier_wait(vel->start_barrier);
    myself->speed = V30KM;
    while (1)
    {
        if (myself->total_dist % vel->length == 0)
        {
            int lap = myself->total_dist / vel->length;
            myself->speed = change_speed(myself, false);
            myself->score += 1; //TODO check scorei;
            if (lap % 15 == 0 && will_break(myself))
            {
                myself->broken = true;
            }
        }

    }

    return NULL;
}

// Calculates if breaks based on chance
bool will_break(Rider rider)
{
    if (can_rider_break(&rider->velodrome))
    {
        int p = rand() % 100;
        if (p > break_chance)
            rider->broken = true;
    }
    return rider->broken;
}

// Calculates if will change and wich adjacent lane to change
void change_lane(Rider rider)
{
    int p = rand() % 100;
    // decides if will change lanes
    if (p < 50)
    {
        //decides wich lane will change
        p = rand() % 100;
        if (p < 60)
        {
            sem_wait(rider->velodrome->velodrome_sem);
            /*go left*/
            sem_post(rider->velodrome->velodrome_sem);
        }
        else
        {
            sem_wait(rider->velodrome->velodrome_sem);
            /*go right*/
            sem_post(rider->velodrome->velodrome_sem);
        }
    }
}
