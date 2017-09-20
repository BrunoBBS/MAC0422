#include "rider.h"

// Chage randomly the rider's speed using defined probilities
int change_speed(Rider *rider);

// Main function of rider
void ride(void *args);

// Calculates if breaks based on chance
bool will_break(Rider *rider);

// Calculates if will change and wich adjacent lane to change
void change_lane(Rider *rider);
