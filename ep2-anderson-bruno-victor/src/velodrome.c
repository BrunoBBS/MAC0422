#include "velodrome.h"

const int sprint_points[] = {0, 1, 2, 3, 5};

void create_velodrome(Velodrome *velodrome_ptr,
        uint length,
        uint rider_cnt,
        uint lap_cnt)
{
    if (globals.e)
        printf("velodrome:l%3d -> Creating velodrome...\n", __LINE__);

    // Allocates the velodrome struct
    Velodrome velodrome =
        *velodrome_ptr = malloc(sizeof(Velodrome));
    velodrome->length = length;
    velodrome->rider_cnt = rider_cnt;
    velodrome->lap_cnt = lap_cnt;

    if (globals.e)
        printf("velodrome:l%3d -> Allocated velodrome\n", __LINE__);

    // Allocates the track
    velodrome->pista = malloc(length * sizeof(uint *));
    for (int i = 0; i < length; i++)
        velodrome->pista[i] = malloc(10 * sizeof(uint));
    for (int i = 0; i < length; i++)
        for (int j = 0; j < 10; j++)
            velodrome->pista[i][j] = -1;

    if (globals.e)
        printf("velodrome:l%3d -> Allocated track matrix\n", __LINE__);

    // Create riders
    velodrome->riders = malloc(rider_cnt * sizeof(struct Rider));
    for (int i = 0; i < rider_cnt; i++)
    {
        int start_lane = i % 10;
        int start_meter = -((i + 10) / 10);
        velodrome->riders[i].id = i;
        velodrome->riders[i].lane = start_lane;
        velodrome->riders[i].total_dist = start_meter;
        velodrome->riders[i].velodrome = velodrome;
        sem_init(&velodrome->riders[i].turn_done, 1, 1);
        velodrome->pista[start_meter + velodrome->length][start_lane] = i;
    }

    if (globals.e)
        printf("velodrome:l%3d -> Set up riders\n", __LINE__);

    // Create placings
    velodrome->placings = malloc((lap_cnt / 10) * sizeof(int *));
    for (int i = 0; i < lap_cnt; i++)
        velodrome->placings[i] = 4;

    // Start riders
    pthread_barrier_init(velodrome->start_barrier, 0, rider_cnt + 1);
    for (int i = 0; i < rider_cnt; i++)
    {
        if (globals.e)
           printf("velodrome:l%3d -> Starting rider %d\n", __LINE__, i);
        pthread_create(&velodrome->riders[i].rider_t, 0, &ride,
                &velodrome->riders[i]);
    }
    pthread_barrier_wait(velodrome->start_barrier);

    if (globals.e)
        printf("velodrome:l%3d -> Started riders\n", __LINE__);
    if (globals.e)
        printf("velodrome:l%3d -> Created velodrome!\n", __LINE__);
}

void destroy_velodrome(Velodrome *velodrome_ptr)
{
    Velodrome velodrome = *velodrome_ptr;

    void *ret;
    for (int i = 0; i < velodrome->rider_cnt; i++)
        pthread_join(velodrome->riders[i].rider_t, &ret);

    // Free track
    for (int i = 0; i < velodrome->length; i++)
    {
        free(velodrome->pista[i]);
        velodrome->pista[i] = NULL;
    }
    free(velodrome->pista);
    velodrome->pista = NULL;
 
    // Free placings
    free(velodrome->placings);
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

Rider rider_in_front(Velodrome *velodrome_ptr, Rider behind)
{
    Velodrome velodrome = *velodrome_ptr;
    // Calculates position of behind rider
    int meter = (behind->total_dist + velodrome->length) % velodrome->length;
    int lane = behind->lane;
    int front_id = velodrome->pista[(meter + 1) % velodrome->length][lane];
    if (front_id == -1)
        return NULL;
    return &velodrome->riders[front_id];
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

void mark_placing(Rider rider, int lap)
{
    rider->score += sprint_points[
        rider->velodrome->placings[lap / 10 - 1]--];

    if (rider->velodrome->placings[lap / 10 - 1] < 0)
        rider->velodrome->placings[lap / 10 - 1]++;
}
