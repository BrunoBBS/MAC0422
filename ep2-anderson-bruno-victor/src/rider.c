#include "rider.h"
#include <pthread.h>
#include <stdlib.h>

const int break_chance = 1;
const int v90_chance = 20;
const int ch_lane_chance = 50;
// Chance when speed was 30, otherwise is 50/50
float v30_chance = 0.3;
// TODO Scoring system

// Calculates rider position in the velodrome
int get_pos(Rider rider)
{
    return (rider->total_dist + rider->velodrome->length)
        % rider->velodrome->length;
}

// Change randomly the rider's speed using defined probilities
int change_speed(Rider rider, bool V90)
{
    int p = rand() % 100;
    if (!V90) {
        if (rider->speed == V60KM) {
            v30_chance = 50;
        }
        return p < v30_chance ? V30KM : V60KM;
    } else {
        return p < v90_chance ? V90KM : rider->speed;
    }
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
    if (globals.e)
        printf("rider:l%3d -> Rider %d moved\n", __LINE__, rider->id);
}

void* coordinator(void* args)
{
    int sum = 0;
    Velodrome velodrome = (Velodrome)args;
    while (sum < velodrome->a_rider_cnt) {
        sum = 0;
        for (int i = 0; i < velodrome->rider_cnt; i++) {
            sum += velodrome->arrive[i];
        }
    }
    for (int j = 0; j < velodrome->rider_cnt; j++)
        velodrome->continue_flag[j] = 1;
    // TODO change to pthread_cond
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

    // wait start
    pthread_barrier_wait(&vel->start_barrier);
    if (globals.e)
        printf("rider:l%3d -> Rider %d started!\n", __LINE__, myself->id);
    myself->speed = V30KM;
    int lap = 0;
    while (lap < vel->lap_cnt) {
        if (get_pos(myself) == 0 && myself->step == 0) {
            lap = myself->total_dist / vel->length;
            myself->speed = change_speed(myself, false);
            if (lap % 10 == 0)
                mark_placing(myself, myself->total_dist / vel->length);
            if (lap % 15 == 0 && will_break(myself)) {
                // TODO die
                lap = vel->lap_cnt;
                myself -> velodrome = NULL;
                vel -> a_rider_cnt -= 1;
                sem_post(&myself->turn_done);
                continue;
            }
        }

        Rider front;
        if (front = rider_in_front(myself))
            sem_wait(&front->turn_done);

        // Checks if is exceeding max speed possible
        if (max_rider_speed(&vel, myself) < myself->speed) {
            myself->speed = max_rider_speed(&vel, myself);
            myself->step = 0;
        }
        // go!!
        int steps_needed;
        switch (myself->speed) {
        case V30KM:
            steps_needed = 1;
            if (vel->round_time == 20)
                steps_needed = 5; // 0 to 5, six steps
            if (myself->step < steps_needed)
                myself->step += 1;
            else if (myself->step == steps_needed) {
                step(change_lane(myself), myself, vel);
                // TODO overtake count
                mark_overtake(myself);
                myself->step = 0;
            }
            break;
        case V60KM:
            steps_needed = 0;
            if (vel->round_time == 20)
                steps_needed = 2; // 0 to 2, three steps
            if (myself->step < steps_needed)
                myself->step += 1;
            else if (myself->step == steps_needed) {
                step(change_lane(myself), myself, vel);
                myself->step = 0;
            }
            break;
        case V90KM:
            // The simulation is already at a 20ms pace
            steps_needed = 1;

            if (myself->step < steps_needed)
                myself->step += 1;
            else if (myself->step == steps_needed) {
                step(change_lane(myself), myself, vel);
                myself->step = 0;
            }
        }
        if (globals.e)
            printf("rider:l%3d -> Rider %d turn done\n", __LINE__, myself->id);

        sem_post(&myself->turn_done);
        if (globals.e)
            printf("rider:l%3d -> Rider %d NOTIFIED turn done\n", __LINE__,
                myself->id);
        vel->arrive[myself->id] = 1;
        if (globals.e)
            printf("rider:l%3d -> Rider %d ARRIVE BARRIER\n", __LINE__,
                myself->id);
        while (vel->continue_flag[myself->id] == 0) {
        }
        if (globals.e)
            printf("rider:l%3d -> Rider %d PASSED BARRIER\n", __LINE__,
                myself->id);
        vel->continue_flag[myself->id] = 0;
    }

    return NULL;
}

// Calculates if breaks based on chance
bool will_break(Rider rider)
{
    if (can_rider_break(&rider->velodrome)) {
        int p = rand() % 100;
        if (p > break_chance)
            rider->broken = true;
    }
    return rider->broken;
}

// Calculates if will change and wich adjacent lane to change
char change_lane(Rider rider)
{
    int p = rand() % 100;
    // decides if will change lanes
    if (p < 50) {
        // decides wich lane will change
        p = rand() % 100;
        if (p < ch_lane_chance / 2) {
            /*go left*/
            return 'l';
        } else if (p > ch_lane_chance / 2 && p < ch_lane_chance) {
            /*go right*/
            return 'r';
        } else {
            // go just forward
            return 'f';
        }
    }
}
