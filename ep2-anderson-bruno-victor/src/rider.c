/* ------------------------------------------------
EP2 - MAC0422 - Sistemas Operacionais 2s2017

Authors:

Anderson Andrei da Silva , 8944025
Bruno Boaventura Scholl, 9793586
Victor Seiji Hariki, 9793694
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
    int meter = get_pos(rider);
    int new_meter = (meter + 1) % vel->length;
    // Exit track if broken
    char move_p[3];

    switch (dir) {
    case 'r':
        move_p[0] = 'r';
        move_p[1] = 'l';
        move_p[2] = 'f';
        break;
    case 'l':
        move_p[0] = 'l';
        move_p[1] = 'r';
        move_p[2] = 'f';
        break;
    case 'f':
        move_p[0] = 'f';
        move_p[1] = 'r';
        move_p[2] = 'l';
        break;
    }
    char final_dir = dir == 'b' ? 'b' : '\0';

    sem_wait(&vel->velodrome_sem);

    int new_lane = lane;

    int tries = 3;

    for (int i = 0; i < 3 && !final_dir && tries > 0; i++) {
        char move_to = move_p[i];
        int rider_id = -1;

        if (move_to == 'r' && lane + 1 == 10)
            continue;
        if (move_to == 'l' && lane - 1 == -1)
            continue;

        if (move_to == 'r')
            new_lane++;

        if (move_to == 'l')
            new_lane--;

        if ((rider_id = vel->pista[new_meter][new_lane]) == -1)
            final_dir = move_to;

        if (rider_id >= 0 && rider_id != rider->id
            && !vel->riders[rider_id].broken
            && !vel->riders[rider_id].finished) {
            int res;
            sem_getvalue(&vel->arrive[rider_id], &res);
            if (!res) {
                i--;
                tries--;
                printf("Rider %d is waiting for %d\n", rider->id, rider_id);
                sem_post(&vel->velodrome_sem);
                sem_wait(&vel->arrive[rider_id]);
                sem_post(&vel->arrive[rider_id]);
                sem_wait(&vel->velodrome_sem);
                printf("Wait ended for %d\n", rider->id);
            }
        }
    }

    if (!final_dir) {
        new_meter = meter;
        new_lane = lane;
        // Compensate dist added after step
        rider->total_dist--;
    } else if (final_dir == 'r')
        new_lane = lane + 1;
    else if (final_dir == 'l')
        new_lane = lane - 1;
    else if (final_dir == 'b') {
        vel->pista[meter][lane] = -1;
        sem_post(&vel->velodrome_sem);
        return;
    }

    vel->pista[meter][lane] = -1;
    vel->pista[new_meter][new_lane] = rider->id;
    sem_post(&vel->velodrome_sem);

    rider->total_dist++;
    rider->lane = new_lane;
}

// Main function for barrier coordinator
void* coordinator(void* args)
{
    Velodrome vel = (Velodrome)args;
    if (globals.e)
        printf("rider:l%3d -> Coordinator started!\n", __LINE__);
    while (vel->a_rider_cnt > 0) {
        for (int i = 0; i < vel->rider_cnt; i++) {
            if (!vel->riders[i].broken && !vel->riders[i].finished) {
                int val;
                sem_getvalue(&vel->arrive[i], &val);
                // printf("Waiting %d: %d\n", i, val);
                sem_wait(&vel->arrive[i]);
                sem_post(&vel->arrive[i]);
            }
        }
        for (int i = 0; i < vel->rider_cnt; i++) {
            if (!vel->riders[i].broken && !vel->riders[i].finished)
                sem_wait(&vel->arrive[i]);
        }
        mark_overtake(vel);
        // printf("B1\n");
        for (int j = 0; j < vel->rider_cnt; j++) {
            if (!vel->riders[j].broken && !vel->riders[j].finished)
                sem_post(&vel->continue_flag[j]);
        }
        // printf("B2\n");
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

    // set start speed
    myself->speed = V30KM;

    // wait start
    pthread_barrier_wait(&vel->start_barrier);
    if (globals.e)
        printf("rider:l%3d -> Rider %d started!\n", __LINE__, myself->id);
    int lap = 1;
    while (lap < vel->lap_cnt) {
        if (get_pos(myself) == 0 && myself->step_time == 0) {
            lap = myself->total_dist / vel->length;
            if (globals.e)
                printf("rider:l%3d -> Rider %d is at lap %d \n", __LINE__,
                    myself->id, lap);
            if (myself->total_dist > vel->length - 1)
                myself->speed = change_speed(myself);
            mark_lap(myself, myself->total_dist / vel->length);
            if (lap > 10 && lap % 15 == 0 && will_break(myself)) {
                printf("******************************\n");
                printf("Atention! -> Rider %d crashed!\n", myself->id);
                printf("******************************\n");
                if (globals.e)
                    printf(
                        "rider:l%3d -> Rider %d died!\n", __LINE__, myself->id);
                // TODO die
                myself->broken = true;
                break;
            }
        }
        // Checks if is exceeding max speed possible
        int curr_spd = myself->speed;
        if (max_rider_speed(&vel, myself) > myself->speed)
            curr_spd = max_rider_speed(&vel, myself);

        // go!!
        if (myself->step_time < curr_spd)
            myself->step_time += vel->round_time;
        else if (myself->step_time >= curr_spd) {
            char dir = change_lane(myself);
            if (dir != 's') {
                step(dir, myself, vel);
                myself->step_time = 0;
            }
        }

        // Notify Global Barrier
        sem_post(&vel->arrive[myself->id]);
        sem_wait(&vel->continue_flag[myself->id]);

        if (globals.r) {
            struct timespec sleep_time;
            sleep_time.tv_sec = 0;
            sleep_time.tv_nsec
                = ((vel->round_time / globals.r) % 1000) * 1000000;
            nanosleep(&sleep_time, NULL);
        }
    }
    step('b', myself, vel);
    if (!myself->broken)
        myself->finished = true;
    if (myself->broken)
        sem_post(&vel->arrive[myself->id]);
    vel->a_rider_cnt--;
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

// Calculates if will change and which adjacent lane to change
char change_lane(Rider rider)
{
    sem_wait(&rider->velodrome->rand_sem);
    bool will_change = rand() % 100 < ch_lane_chance; // Will be changing lanes?
    char p_lane = rand() % 100 < 50 ? 'r' : 'l';      // Which side if it will
    sem_post(&rider->velodrome->rand_sem);

    if (will_change && p_lane == 'l' && rider->lane > 0)
        return 'l';
    else if (will_change && rider->lane < 9)
        return 'r';
    return 'f';
}
