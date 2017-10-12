#include "rider.h"
#include "velodrome.h"

#include <pthread.h>
#include <sys/ioctl.h>
#include <unistd.h>

struct winsize wins;

void* debug_print_thread(void* velodrome_ptr)
{
    Velodrome velodrome = (Velodrome)velodrome_ptr;
    while (1)
    {
        int i = 0;
        while (i < velodrome->length)
        {
            for (int lane = 0; lane < 10; lane++)
            {
                for (int meter = i;
                     meter < i + wins.ws_col && meter < velodrome->length;
                     meter++)
                {
                    int value = velodrome->pista[meter][lane];
                    fprintf(stderr, "%c", value < 0 ? '-' : 'A' + value);
                }
                fprintf(stderr, "\n");
            }
            fprintf(stderr, "\n");
            i += wins.ws_col;
        }
        sleep(1);
    }
}

int main(int argc, char **argv)
{
    uint vel_len = 150;
    uint rider_cnt = 15;
    uint lap_cnt = 160;

    ioctl(STDOUT_FILENO, TIOCGWINSZ, &wins);

    printf("Terminal of size: %d x %d\n", wins.ws_col, wins.ws_row);

    globals.e = globals.d = globals.r = false;
    if (argc > 1)
    {
        int c = 0;
        while (argv[0][c])
        {
            char option = argv[0][c++];
            switch (option)
            {
                case 'e':
                    globals.e = true;
                    break;
                case 'r':
                    globals.r = true;
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
    if (!globals.e)
    {
        pthread_create(&graphics_pthread, NULL, &debug_print_thread, velodrome);
        pthread_join(graphics_pthread, NULL);
    }
    destroy_velodrome(&velodrome);
}
