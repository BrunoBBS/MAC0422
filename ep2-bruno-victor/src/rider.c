#include "rider.h"

const float break_chance = 0.01;
const float v90_chance = 0.2;
// Chance when speed was 30, otherwise is 50/50
const float v30_chance = 0.3;
const float v60_chance = 0.7;

// Chage randomly the rider's speed using defined probilities
int change_speed(Rider *rider);

// Main function of rider
void ride(void *args);

// Calculates if breaks based on chance
bool will_break(Rider *rider);

// Calculates if will change and wich adjacent lane to change
void change_lane(Rider *rider);
