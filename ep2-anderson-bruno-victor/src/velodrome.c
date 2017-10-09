#include "velodrome.h"

void create_velodrome(Velodrome *velodrome_ptr, uint length, uint rider_cnt)
{
    // Allocates the velodrome struct
    Velodrome velodrome =
        *velodrome_ptr = malloc(sizeof(Velodrome));
    velodrome->length = length;
    velodrome->rider_cnt = rider_cnt;

    // Allocates the track
    velodrome->pista = malloc(length * sizeof(uint *));
    for (int i = 0; i < length; i++)
        velodrome->pista[i] = malloc(10 * sizeof(uint));

    // Create riders
    velodrome->riders = malloc(rider_cnt * sizeof(Rider));
    for (int i = 0; i < rider_cnt; i++)
    {
        velodrome->riders[i].id = i;
        velodrome->riders[i].lane = i % 10;
        velodrome->riders[i].total_dist = -(i / 10);
    }

    // Create placings
    velodrome->placings = malloc(160 * sizeof(int));
    for (int i = 0; i < rider_cnt; i++)
    {
        velodrome->placings[i] = malloc(rider_qnt * sizeof(int));;
    }

    //Inicializing the placings with 0
    for (int i = 0; i < 160; i++)
        for (int j = 0; j < rider_cnt; j++)
            velodrome->placings[i][j] = 0;

    // Start riders
    pthread_barrier_init(velodrome->start_barrier, 0, rider_cnt + 1);
    for (int i = 0; i < rider_cnt; i++)
    {
        pthread_create(&velodrome->riders[i].rider_t, 0, &ride,
                &velodrome->riders[i]);
    }
    pthread_barrier_wait(velodrome->start_barrier);


}

void destroy_velodrome(Velodrome *velodrome_ptr)
{
    Velodrome velodrome = *velodrome_ptr;
    // Free track
    for (int i = 0; i < velodrome->length; i++)
    {
        free(velodrome->pista[i]);
        velodrome->pista[i] = NULL;
    }
    free(velodrome->pista);
    velodrome->pista = NULL;

    // Free velodrome struct
    free(velodrome);
    *velodrome_ptr = NULL;
}

int max_rider_speed(
    Velodrome *velodrome_ptr,
    Rider rider)
{
    Velodrome velodrome = *velodrome_ptr;
    int max_speed = 3;
    // Get the meter immediately in front of the rider
    int next_meter = (rider->total_dist + velodrome->length + 1) % velodrome->length;

    // For each lane external to this one
    for (int lane = rider->lane; lane < 10; lane++)
    {
        // Get id of rider in this place
        int id = velodrome->pista[next_meter][lane];

        // If there is no rider here
        if (id < 0)
            continue;
        // Else, if some other rider is slower than max speed
        else if (velodrome->riders[id].speed < max_speed)
            max_speed = velodrome->riders[id].speed;
    }

    // Return max speed so rider does not overtake from internal lane
    return max_speed;
}

Rider rider_in_front(Velodrome velodrome_ptr, Rider behind)
{
    // Calculates position of behind rider
    int meter = behind->total_dist % velodrome_ptr->length;
    int lane = behind->lane;
    int front_id = velodrome_ptr->pista[meter + 1][lane];
    return &velodrome_ptr->riders[front_id];
}

bool can_rider_break(
    Velodrome *velodrome_ptr)
{
    Velodrome velodrome = *velodrome_ptr;
    return velodrome->a_rider_cnt; 
}

bool is_sprint(Velodrome *velodrome_ptr,
    Rider rider) 
{
    if (rider->step % 10 == 0)
        return true;
    return false;
}

void complete_turn(
    Velodrome *velodrome_ptr,
    Rider rider) 
{
    if (rider->total_dist % rider->step == 0){
        rider->total_dist += 60;
    }
    return;
}

void mark_placing(
    Velodrome *velodrome_ptr,
    Rider rider)
{
    int i;
    for (i = 0; i < velodrome_ptr->rider_cnt; i++ && velodrome_ptr->placings[i][velodrome_ptr->turn] != 0) {
        velodrome_ptr->placings[i][velodrome_ptr.turn] = rider->id;
    }
    return;
}

void scoring(Velodrome *velodrome_ptr,
    Rider rider) 
{
    int round = rider.total_dist/60;
    if (is_sprint()) {
        if (velodrome_ptr->placings[round][0] == rider->id) 
            rider->score += 5;
        
        else if (velodrome_ptr->placings[round][1] == rider->id) 
            rider->score += 3;
        
        else if (velodrome_ptr->placings[round][2] == rider->id) 
            rider->score += 2;
        
        else if (velodrome_ptr->placings[round][3] == rider->id) 
            rider->score += 1;
    }
    return;
}
