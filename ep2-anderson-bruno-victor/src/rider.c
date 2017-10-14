/* ------------------------------------------------
EP2 - MAC0422 - Sistemas Operacionais 2s2017

Authors:

Anderson Andrei da Silva , 8944025
Bruno Boaventura Scholl, 9793586
Seiji
------------------------------------------------- */

#include "rider.h"
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

const int break_chance = 5;
const int v90_chance = 20;
const int ch_lane_chance = 50;
// Chance when speed was 30, otherwise is 50/50
const int v30_60_chance = 70;
const int v60_30_chance = 50;

// Calculates rider position in the velodrome
int get_pos(Rider rider)
{
    return (rider->total_dist + rider->velodrome->length)
        % rider->velodrome->length;
}

// Change randomly the rider's speed using defined probilities
int change_speed(Rider rider)
{
    sem_wait(&rider->velodrome->rand_sem);
    int p = rand() % 100;
    sem_post(&rider->velodrome->rand_sem);
    return rider->speed == V30KM ? p < v30_60_chance ? V60KM : V30KM
                                 : p < v60_30_chance ? V30KM : V60KM;
}

// Writes down in velodrome when rider rides 1 meter
void step(char dir, Rider rider, Velodrome vel)
{
    int lane = rider->lane;
    int new_lane = lane;
    int meter = get_pos(rider);
    int new_meter = (get_pos(rider) + 1) % vel->length;
    if (dir == 'r')
        new_lane = lane + 1;
    else if (dir == 'l')
        new_lane = lane - 1;
    sem_wait(&vel->velodrome_sem);
    vel->pista[meter][lane] = -1;
    vel->pista[new_meter][new_lane] = rider->id;
    sem_post(&vel->velodrome_sem);
    rider->total_dist++;
    rider->lane = new_lane;
    if (globals.e)
        printf("rider:l%3d -> Rider %d moved\n", __LINE__, rider->id);
}

// Main function for barrier coordinator
void* coordinator(void* args)
{
    Velodrome vel = (Velodrome)args;
    if (globals.e)
        printf("rider:l%3d -> Coordinator started!\n", __LINE__);
    while (1) {
        if (globals.e)
            printf("rider:l%3d -> Coordinator running\n", __LINE__);
        for (int i = 0; i < vel->rider_cnt; i++) {
            if (!vel->riders[i].broken) {
                if (globals.e)
                    printf("rider:l%3d -> Coordinator waiting rider %d arrive\n", __LINE__, i);
                sem_wait(&vel->arrive[i]);
            }
        }
        if (globals.e)
            printf("rider:l%3d -> Coordinator passed first for\n", __LINE__);
        for (int j = 0; j < vel->rider_cnt; j++) {
            if (!vel->riders[j].broken)
                sem_post(&vel->continue_flag[j]);
        }
    }
}

// Main function of rider
void* ride(void* args)
{
    // TODO
    // wait start
    // if lap comleted:
    //  ->choose new speed
    //  ->check if scores
    //  ->if lap is multiple of 15:
    //      ->decide if breaks
    // go foraward, left or right:
    //   ->if is a rider in front:
    //      ->wait rider in front, left or right do its turn
    //      ->chacks max speed poddible and reduce if needed
    // notify rider behind semaphore
    // notify global barrier
    // wait global barrier

    Rider myself = (Rider)args;
    Velodrome vel = myself->velodrome;

    if (globals.e)
        printf("rider:l%3d -> rider count %d\n", __LINE__, vel->rider_cnt);
    if (globals.e)
        printf("rider:l%3d -> Created rider %d\n", __LINE__, myself->id);

    // set speed
    myself->speed = V30KM;

    // wait start
    pthread_barrier_wait(&vel->start_barrier);
    if (globals.e)
        printf("rider:l%3d -> Rider %d started!\n", __LINE__, myself->id);
    int lap = 1;
    while (lap < vel->lap_cnt) {
        if (get_pos(myself) == 0 && myself->step_time == 0) {
            lap = myself->total_dist / vel->length;
            if (myself->total_dist > vel->length - 1)
            myself->speed = change_speed(myself);
            mark_lap(myself, myself->total_dist / vel->length);
            if (lap % 15 == 0 && will_break(myself))
            {
                if (globals.e)
                    printf(
                        "rider:l%3d -> Rider %d died!\n", __LINE__, myself->id);
                // TODO die
                myself->broken = true;
                lap = vel->lap_cnt;
                myself->velodrome = NULL;
                vel->a_rider_cnt -= 1;
                continue;
            }
        }

        Rider front;
        if (front = rider_in_front(myself)) {
            if (globals.e)
                printf("rider:l%3d -> Rider %d is waiting rider %d \n",
                    __LINE__, myself->id, front->id);
        }

        // Checks if is exceeding max speed possible
        int curr_spd = myself->speed;
        if (max_rider_speed(&vel, myself) > myself->speed)
            curr_spd = max_rider_speed(&vel, myself);

        // go!!
        if (myself->step_time < curr_spd)
            myself->step_time += vel->round_time;
        else if (myself->step_time >= curr_spd) {
            step(change_lane(myself), myself, vel);
            // TODO not overtake from left
            mark_overtake(myself);
            myself->step_time = 0;
        }

        if (globals.e)
            printf(
                "rider:l%3d -> Rider %d is at barrier\n", __LINE__, myself->id);

        // Notify Global Barrier
        sem_post(&vel->arrive[myself->id]);
        sem_wait(&vel->continue_flag[myself->id]);

        if (globals.e)
            printf("rider:l%3d -> Rider %d passed barrier\n", __LINE__,
                myself->id);
        if (globals.r) {
            struct timespec sleep_time;
            sleep_time.tv_sec = 0;
            sleep_time.tv_nsec
                = ((vel->round_time / globals.r) % 1000) * 1000000;
            nanosleep(&sleep_time, NULL);
        }
    }

    if (globals.e)
        printf("rider:l%3d -> Rider %d terminated\n", __LINE__, myself->id);

    return NULL;
}

// Calculates if breaks based on chance
bool will_break(Rider rider)
{
    if (can_rider_break(&rider->velodrome)) {
        sem_wait(&rider->velodrome->rand_sem);
        int p = rand() % 100;
        sem_post(&rider->velodrome->rand_sem);
        if (p < break_chance)
            return true;
    }
    return false;
}

// Calculates if will change and wich adjacent lane to change
char change_lane(Rider rider)
{
    do {
        sem_wait(&rider->velodrome->rand_sem);
        int p = rand() % 100;
        sem_post(&rider->velodrome->rand_sem);
        Velodrome vel = rider->velodrome;
        int pos = (get_pos(rider) + 1) % vel->length;
        // decides if will change lanes
        if (p < 50) {
            // decides wich lane will change
            p = rand() % 100;
            if (p < ch_lane_chance / 2 && rider->lane > 0
                && vel->pista[pos][rider->lane - 1] == -1) {
                /*go left*/
                return 'l';
            } else if (p > ch_lane_chance / 2 && p < ch_lane_chance
                && rider->lane < 9 && vel->pista[pos][rider->lane + 1] == -1
                && vel->pista[(pos + vel->length - 1) % vel->length]
                             [rider->lane + 1]
                    == -1) {
                /*go right*/
                return 'r';
            }
        } else if (vel->pista[pos][rider->lane] == -1) {
            // go just forward
            return 'f';
        }
    } while (1);
}
