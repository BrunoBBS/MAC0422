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
        *velodrome_ptr = malloc(sizeof(struct Velodrome));
    velodrome->length = length;
    velodrome->rider_cnt = rider_cnt;
    velodrome->lap_cnt = lap_cnt;
    velodrome->arrive = malloc(rider_cnt * sizeof(sem_t));
    velodrome->continue_flag = malloc(rider_cnt * sizeof(sem_t));
    velodrome->round_time = 60;
    velodrome->a_rider_cnt = rider_cnt;
    sem_init(&velodrome->velodrome_sem, 0, 1);
    sem_init(&velodrome->rand_sem, 0, 1);

    for (int i = 0; i< rider_cnt;i++)
    {
        sem_init(&velodrome->arrive[i], 0, 0);
        sem_init(&velodrome->continue_flag[i], 0, 0);
    }

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
        velodrome->pista[start_meter + velodrome->length][start_lane] = i;
        velodrome->riders[i].overtake = malloc(rider_cnt * sizeof(int));
        for (int j = 0; j < rider_cnt; j++)
            velodrome->riders[i].overtake[j] = 0;
    }

    if (globals.e)
        printf("velodrome:l%3d -> Set up riders\n", __LINE__);

    // Create placings
    velodrome->placings = malloc((lap_cnt / 10) * sizeof(int *));
    for (int i = 0; i < lap_cnt; i++)
        velodrome->placings[i] = 4;

    // Start riders
    pthread_barrier_init(&velodrome->start_barrier, 0, rider_cnt + 1);
    for (int i = 0; i < rider_cnt; i++)
    {
        if (globals.e)
            printf("velodrome:l%3d -> Starting rider %d\n", __LINE__, i);
        pthread_create(&velodrome->riders[i].rider_t, 0, &ride,
                       &velodrome->riders[i]);
    }
    // Start coordinator thread
    pthread_create(&velodrome->coordinator_t, 0, &coordinator, velodrome);
    pthread_barrier_wait(&velodrome->start_barrier);

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
    velodrome->placings = NULL;

    // Free velodrome struct
    free(velodrome);
    *velodrome_ptr = NULL;
}

int max_rider_speed(Velodrome *velodrome_ptr, Rider rider)
{
    Velodrome velodrome = *velodrome_ptr;
    int max_speed = V90KM;
    // Get the meter immediately in front of the rider
    int next_meter =
        (get_pos(rider) + 1) % velodrome->length;

    int id = velodrome->pista[next_meter][rider->lane] >= 0;
    if (id >= 0)
        return velodrome->riders[id].speed;
}

Rider rider_in_front(Rider behind)
{
    Velodrome velodrome = behind->velodrome;
    // Calculates position of behind rider
    int meter = get_pos(behind);
    int lane = behind->lane;
    int front_id = velodrome->pista[(meter + 1) % velodrome->length][lane];
    if (front_id == -1)
        return NULL;
    return &velodrome->riders[front_id];
}

bool can_rider_break(
    Velodrome *velodrome_ptr)
{
    return (*velodrome_ptr)->a_rider_cnt > 5;
}

void mark_placing(Rider rider, int lap)
{
    if (lap <= 0 || lap >= rider->velodrome->lap_cnt || lap % 10)
        return;

    int points = sprint_points[rider->velodrome->placings[lap / 10 - 1]--];

    if (globals.e && points)
        printf("velodrome:l%3d -> Rider %d gained %d points on lap %d\n",
               __LINE__, rider->id, points, lap);

    rider->score += points;

    if (rider->velodrome->placings[lap / 10 - 1] < 0)
        rider->velodrome->placings[lap / 10 - 1]++;
}

void mark_overtake(Rider rider)
{
    int meter = get_pos(rider);
    Velodrome velodrome = rider->velodrome;
    Rider overtaken;
    for (int i = 0; i < 10; i++)
    {
        if (i != rider->lane)
        {
            int id = velodrome->pista[meter][i];
            if (id >= 0)
            {
                overtaken = &velodrome->riders[id];
                rider->overtake[overtaken->id]++;
                overtaken->overtake[rider->id]--;
            }
        }
    }
    int points = 20;
    for (int i = 0; i < velodrome->rider_cnt; i++)
    {
        if (!velodrome->riders[i].broken && (rider->overtake[i] % 2 != 0 || rider->overtake[i] < 0 || rider->total_dist < velodrome->length))
            // No poits for you
            points = 0;
    }
    rider->score += points;
    return;
}
