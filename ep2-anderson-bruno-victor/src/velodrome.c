/* ------------------------------------------------
EP2 - MAC0422 - Sistemas Operacionais 2s2017

Authors:

Anderson Andrei da Silva , 8944025
Bruno Boaventura Scholl, 9793586
Victor Seiji Hariki, 9793694
------------------------------------------------- */

#include "velodrome.h"

const int sprint_points[] = { 0, 1, 2, 3, 5 };

void create_velodrome(
    Velodrome* velodrome_ptr, uint length, uint rider_cnt, uint lap_cnt)
{
    if (globals.e)
        printf("velodrome:l%3d -> Creating velodrome...\n", __LINE__);

    // Allocates the velodrome struct
    Velodrome velodrome = *velodrome_ptr = malloc(sizeof(struct Velodrome));
    velodrome->length = length;
    velodrome->rider_cnt = rider_cnt;
    velodrome->lap_cnt = lap_cnt;
    velodrome->arrive = malloc(rider_cnt * sizeof(sem_t));
    velodrome->continue_flag = malloc(rider_cnt * sizeof(sem_t));
    velodrome->round_time = 60;
    velodrome->a_rider_cnt = rider_cnt;
    velodrome->a_score = malloc(rider_cnt * sizeof(struct Rider));
    sem_init(&velodrome->velodrome_sem, 0, 1);
    sem_init(&velodrome->rand_sem, 0, 1);
    sem_init(&velodrome->print_sem, 0, 1);
    sem_init(&velodrome->score_sem, 0, 1);

    for (int i = 0; i < rider_cnt; i++) {
        sem_init(&velodrome->arrive[i], 0, 0);
        sem_init(&velodrome->continue_flag[i], 0, 0);
    }

    if (globals.e)
        printf("velodrome:l%3d -> Allocated velodrome\n", __LINE__);

    // Allocates the track
    velodrome->pista = malloc(length * sizeof(int*));
    for (int i = 0; i < length; i++)
        velodrome->pista[i] = malloc(10 * sizeof(int));
    for (int i = 0; i < length; i++)
        for (int j = 0; j < 10; j++)
            velodrome->pista[i][j] = -1;

    if (globals.e)
        printf("velodrome:l%3d -> Allocated track matrix\n", __LINE__);

    // Allocates the placings stack
    velodrome->placings_v = malloc(velodrome->lap_cnt * sizeof(int*));
    for (int i = 0; i < velodrome->lap_cnt; i++)
        velodrome->placings_v[i] = malloc(velodrome->rider_cnt * sizeof(int));
    for (int i = 0; i < velodrome->lap_cnt; i++)
        for (int j = 0; j < velodrome->rider_cnt; j++)
            velodrome->placings_v[i][j] = -1;

    velodrome->s_indexes = malloc(velodrome->lap_cnt * sizeof(int));
    for (int i = 0; i < velodrome->lap_cnt; i++)
        velodrome->s_indexes[i] = 0;

/*    for (int i = 0; i < velodrome->rider_cnt; i++){
        velodrome->a_score[i] = 0;
    }*/

    // Create riders
    velodrome->riders = malloc(rider_cnt * sizeof(struct Rider));
    for (int i = 0; i < rider_cnt; i++) {
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
        velodrome->riders[i].finished = false;
    }

    if (globals.e)
        printf("velodrome:l%3d -> Set up riders\n", __LINE__);

    // Create placings
    velodrome->placings = malloc((lap_cnt / 10) * sizeof(int));
    for (int i = 0; i < lap_cnt; i++)
        velodrome->placings[i] = 4;

    // Start riders
    pthread_barrier_init(&velodrome->start_barrier, 0, rider_cnt + 1);
    for (int i = 0; i < rider_cnt; i++) {
        if (globals.e)
            printf("velodrome:l%3d -> Starting rider %d\n", __LINE__, i);
        pthread_create(
            &velodrome->riders[i].rider_t, 0, &ride, &velodrome->riders[i]);
    }
    // Start coordinator thread
    pthread_create(&velodrome->coordinator_t, 0, &coordinator, velodrome);
    pthread_barrier_wait(&velodrome->start_barrier);

    if (globals.e)
        printf("velodrome:l%3d -> Started riders\n", __LINE__);
    if (globals.e)
        printf("velodrome:l%3d -> Created velodrome!\n", __LINE__);
}

void destroy_velodrome(Velodrome* velodrome_ptr)
{
    Velodrome velodrome = *velodrome_ptr;

    void* ret;
    for (int i = 0; i < velodrome->rider_cnt; i++)
        pthread_join(velodrome->riders[i].rider_t, &ret);

    // Join coordinator
    pthread_join(velodrome->coordinator_t, &ret);

    // Print race's final status
    for (int i = 0; i < velodrome->rider_cnt; i++) {
        velodrome->a_score[i] = velodrome->riders[i];
    }
    print_end_of_run(velodrome->a_score, velodrome);

    // Free track
    for (int i = 0; i < velodrome->length; i++) {
        //printf("free pista[%d]: 0x%lx\n", i, velodrome->pista[i]);
        free(velodrome->pista[i]);
        velodrome->pista[i] = NULL;
    }
    //printf("free pista: 0x%lx\n", velodrome->pista);
    free(velodrome->pista);
    velodrome->pista = NULL;

    // Free placings
    free(velodrome->placings);
    velodrome->placings = NULL;

    // Frees the placings stack
    for (int i = 0; i < velodrome->lap_cnt; i++) {
        //printf("free placings_v[%d]: 0x%lx\n", i, velodrome->placings_v[i]);
        free(velodrome->placings_v[i]);
        velodrome->placings_v[i] = NULL;
    }
    //printf("free placings_v: 0x%lx\n", velodrome->placings_v);
    free(velodrome->placings_v);
    velodrome->placings_v = NULL;
    //printf("free s_indexes: 0x%lx\n", velodrome->s_indexes);
    free(velodrome->s_indexes);
    velodrome->s_indexes = NULL;

    //printf("free a_score: 0x%lx\n", velodrome->a_score);
    free(velodrome->a_score);
    velodrome->a_score = NULL;

    // Destroy semaphores
    for (int i = 0; i < velodrome->rider_cnt; i++) {
        sem_destroy(&velodrome->arrive[i]);
        sem_destroy(&velodrome->continue_flag[i]);
    }
    sem_destroy(&velodrome->velodrome_sem);
    sem_destroy(&velodrome->rand_sem);
    sem_destroy(&velodrome->score_sem);
    sem_destroy(&velodrome->print_sem);

    // Free velodrome struct
    free(velodrome);
    *velodrome_ptr = NULL;
}

int max_rider_speed(Velodrome* velodrome_ptr, Rider rider)
{
    Velodrome velodrome = *velodrome_ptr;
    // Get the meter immediately in front of the rider
    int next_meter = (get_pos(rider) + 1) % velodrome->length;

    int id = velodrome->pista[next_meter][rider->lane] >= 0;
    if (id >= 0)
        return velodrome->riders[id].speed;
    return 0;
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

bool can_rider_break(Velodrome* velodrome_ptr)
{
    return (*velodrome_ptr)->a_rider_cnt > 5;
}

void mark_sprint(Rider rider, int lap)
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

// Mark lap
void mark_lap(Rider rider, int lap)
{
    if (lap <= 0 || lap >= rider->velodrome->lap_cnt)
        return;

    Velodrome velodrome = rider->velodrome;

    sem_wait(&rider->velodrome->score_sem);
    if (!(lap % 10) && lap > 0) {
        mark_sprint(rider, lap);
    }

    velodrome->placings_v[lap - 1][velodrome->s_indexes[lap - 1]++] = rider->id;


    bool behind = false;
    for (int i = 0; i < velodrome->rider_cnt;i++)
    {
        if (i == rider->id)
            continue;
        Rider rider2 = &velodrome->riders[i];
        if (!rider2->broken && rider2->total_dist <= rider->total_dist)
        {
            behind = true;
            break;
        }
    }

    if (!behind){
        print_info(velodrome->placings_v[lap - 1], rider->velodrome, lap,
                velodrome->s_indexes[lap - 1]);
        if (!(lap % 10) && lap > 0) {
            for (int i = 0; i < velodrome->rider_cnt; i++) {
                velodrome->a_score[i] = velodrome->riders[i];
            }
            print_scores(velodrome->a_score, rider->velodrome, lap);
        }
/*        if(lap == rider->velodrome->lap_cnt - 1) {
            for (int i = 0; i < velodrome->rider_cnt; i++) {
                velodrome->a_score[i] = velodrome->riders[i];
            }
            print_end_of_run(rider->velodrome->a_score, rider->velodrome);
        }*/
    }
    sem_post(&rider->velodrome->score_sem);
}

void mark_overtake(Velodrome velodrome)
{
    struct Rider a, b;
    for (int i = 0; i < velodrome->rider_cnt; i++) {
        a = velodrome->riders[i];
        for (int j = i + 1; j < velodrome->rider_cnt; j++) {
            b = velodrome->riders[j];
            int dista_b = a.total_dist - b.total_dist;
            if (!a.broken && !b.broken) {
                if (dista_b > 0) {
                    a.overtake[b.id] = 1 + dista_b / a.velodrome->length;
                } else {
                    b.overtake[a.id] = 1 + dista_b / b.velodrome->length;
                }
            }
        }
    }
    for (int j = 0; j < velodrome->rider_cnt; j++) {
        a = velodrome->riders[j];
        int points = 20;
        for (int i = 0; i < velodrome->rider_cnt; i++) {
            if (!velodrome->riders[i].broken && i != a.id
                && (a.overtake[i] % 2 != 0 || a.overtake[i] == 0))
                // No poits for you
                points = 0;
        }
        a.score += points;
    }
}

/* Use it how : void qsort(void *base, size_t nmemb, size_t size,
                    int (*compar)(const void *, const void *));
*/
int compare_scores(const void* rider_a, const void* rider_b)
{
    Rider a = ((Rider)rider_a);
    Rider b = ((Rider)rider_b);
    if (a->score > b->score)
        return -1;
    else if (a->score < b->score)
        return 1;
    return 0;
}

void print_info(int *id, Velodrome velodrome_ptr, int lap, int lap_cnt) {
    sem_wait(&velodrome_ptr->print_sem);
    printf("Classification lap %2d : ", lap);
    for (int i = 0; i < lap_cnt; i++) {
        printf(" %2d", id[i]);
    }
    printf("\n");
    sem_post(&velodrome_ptr->print_sem);
}


void print_scores(struct Rider *scores, Velodrome velodrome_ptr, int lap) {
    qsort(scores, velodrome_ptr->rider_cnt, sizeof(struct Rider), compare_scores);
    sem_wait(&velodrome_ptr->print_sem);
    printf("---------------------------------\n");
    printf("Attention! Total scores lap %d \n Active riders \n", lap);
    for (int i = 0; i < velodrome_ptr->rider_cnt; i++){
        if(!scores[i].broken) printf(" %2d", scores[i].id);
    }
    printf("\n");
    for (int i = 0; i < velodrome_ptr->rider_cnt; i++){
        if(!scores[i].broken) printf(" %2d", scores[i].score);
    }
    printf("\n---------------------------------\n");

    printf(" Riders crashed \n");
    for (int i = 0; i < velodrome_ptr->rider_cnt; i++){
        if(scores[i].broken) printf(" %2d", scores[i].id);
    }
    printf("\n");
    for (int i = 0; i < velodrome_ptr->rider_cnt; i++){
        if(scores[i].broken) printf(" %2d", scores[i].score);
    }
    printf("\n---------------------------------\n");
    sem_post(&velodrome_ptr->print_sem);
}

void print_end_of_run (struct Rider *scores, Velodrome velodrome_ptr) {
    sem_wait(&velodrome_ptr->print_sem);
    printf("\n==================================\n");
    printf("TOTAL SCORES - FINAL LAP \n Active riders \n");
    for (int i = 0; i < velodrome_ptr->rider_cnt; i++){
        if(!scores[i].broken) printf(" %5d", scores[i].id);
    }
    printf("\n");
    for (int i = 0; i < velodrome_ptr->rider_cnt; i++){
        if(!scores[i].broken) printf(" %5d", scores[i].score);
    }
    printf("\n");
    printf("\n Instant of arrival \n ");
    for (int i = 0; i < velodrome_ptr->rider_cnt; i++){
        if(!scores[i].broken) printf("%5d ", scores[i].total_time);
    }
    printf("\n==================================\n");

    printf(" Riders crashed - Lap crashed\n");
    for (int i = 0; i < velodrome_ptr->rider_cnt; i++){
        if(velodrome_ptr->a_score[i].broken) printf(" %5d", velodrome_ptr->a_score[i].id);
    }
    printf("\n");
    for (int i = 0; i < velodrome_ptr->rider_cnt; i++){
        if(velodrome_ptr->a_score[i].broken) printf(" %5d", (velodrome_ptr->a_score[i].total_dist / velodrome_ptr->length)+1);
    }
    printf("\n==================================\n");
    sem_post(&velodrome_ptr->print_sem);
}

void print_map (Velodrome velodrome)
{
    sem_wait(&velodrome->print_sem);
    
    printf("pista       ->");
    for (int i = 0; i < 10; i++)
    {
        printf(" %4d", i);
    }
    printf("\n");

    for (int m = 0; m < velodrome->length; m++)
    {
        if (!(m % 10) || m == velodrome->length - 1)
            printf("meter: %4d ->", m);
        else
            printf("              ");

        for (int i = 0; i < 10; i++)
        {
            if (velodrome->pista[m][i] >= 0)
                printf("|%4d", velodrome->pista[m][i]);
            else
                printf("|    ");
        }
        printf("|\n");
    }
    printf("\n");
    sem_post(&velodrome->print_sem);
}
