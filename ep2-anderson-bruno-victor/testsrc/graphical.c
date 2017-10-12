#include "rider.h"
#include "velodrome.h"

#include <time.h>

#include <pthread.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct winsize wins;

void print_vel(Velodrome velodrome)
{
    sem_wait(&velodrome->velodrome_sem);
    for (int j = 0; j < 61; j++)
        printf("* ");
    printf("\n");
    for (int j = 0; j < velodrome->rider_cnt; j++)
    {
        Rider rider = &velodrome->riders[j];
        printf("* %c -> l:%5d s:%5d p:%6d t:%5d o:", 'A' + j,
                rider->total_dist / velodrome->length,
                rider->speed, rider->score,
                rider->total_dist);
        printf("[ ");
        for (int i = 0; i < velodrome->rider_cnt; i++)
            printf("%3d%s", rider->overtake[i],
                    i == velodrome->rider_cnt - 1 ? " ] *\n" : ", ");
    }
    for (int j = 0; j < 61; j++)
        printf("* ");
    printf("\n");

    int i = 0;
    while (i < velodrome->length) {
        for (int lane = 0; lane < 10; lane++) {
            for (int meter = i;
                 meter < i + wins.ws_col && meter < velodrome->length;
                 meter++) {
                int value = velodrome->pista[meter][lane];
                fprintf(stderr, "%c", value < 0 ? '-' : 'A' + value);
            }
            fprintf(stderr, "\n");
        }
        fprintf(stderr, "\n");
        i += wins.ws_col;
    }
    sem_post(&velodrome->velodrome_sem);
}

void* debug_print_thread(void* velodrome_ptr)
{
    Velodrome velodrome = (Velodrome)velodrome_ptr;
    while (velodrome->a_rider_cnt) {
        print_vel(velodrome);

        struct timespec sleep_time;
        sleep_time.tv_sec = 0;
        sleep_time.tv_nsec = 100000000;
        nanosleep(&sleep_time, NULL);
    }
}

int main(int argc, char** argv)
{
    uint vel_len = 150;
    uint rider_cnt = 15;
    uint lap_cnt = 160;

    srand(time(0));

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wins);

    printf("Terminal of size: %d x %d\n", wins.ws_col, wins.ws_row);

    globals.e = false;
    globals.d = false;
    globals.r = false;

    if (argc > 1) {
        int c = 0;
        while (argv[1][c]) {
            char option = argv[1][c++];
            switch (option) {
            case 'e':
                globals.e = true;
                break;
            case 'r':
                globals.r = true;
                if (argv[1][c] >= '1' && argv[1][c] <= '9')
                    globals.r = argv[1][c++] - '0';
                break;
            default:
                fprintf(stderr, "Unrecognized option: '%c'\n", option);
                break;
            }
        }
    }

    Velodrome velodrome = NULL;

    create_velodrome(&velodrome, vel_len, rider_cnt, lap_cnt);

    if (globals.e)
        printf("velodrome:l%3d -> Creating graphics thread...\n", __LINE__);
    pthread_t graphics_pthread;
    if (!globals.e && globals.r) {
        printf("Starting graphical engine\n");
        pthread_create(&graphics_pthread, NULL, &debug_print_thread, velodrome);
        pthread_join(graphics_pthread, NULL);
    }

    print_vel(velodrome);

    destroy_velodrome(&velodrome);
}
