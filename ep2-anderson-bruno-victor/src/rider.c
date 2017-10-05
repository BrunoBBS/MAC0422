#include "rider.h"
#include <stdlib.h>

const float break_chance = 0.01;
const float v90_chance = 0.2;
// Chance when speed was 30, otherwise is 50/50
float v30_chance = 0.3;

// Chage randomly the rider's speed using defined probilities
int change_speed(Rider *rider, bool V90)
{
    int p = rand() % 100;
    if (!V90)
    {
        if (rider->speed == V60KM)
        {
            v30_chance = 0.5;
        }
        return p < v30_chance * 100 ? V30KM : V60KM;
    }
    else
    {
        return p < v90_chance * 100 ? V90KM : rider->speed;
    }
}

// Main function of rider
void ride(void *args)
{
}

// Calculates if breaks based on chance
bool will_break(Rider *rider)
{
    if (can_rider_break(rider->velodrome))
    {
        int p = rand() % 100;
        if (p > break_chance)
            rider->broken = true;
    }
}

// Calculates if will change and wich adjacent lane to change
void change_lane(Rider *rider)
{
    int p = rand() % 100;
    // decides if will change lanes
    if (p < 50)
    {
        //decides wich lane will change
        p = rand() % 100;
        if (p < 40)
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
