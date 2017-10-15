/* ------------------------------------------------
EP2 - MAC0422 - Sistemas Operacionais 2s2017

Authors:

Anderson Andrei da Silva , 8944025
Bruno Boaventura Scholl, 9793586
Victor Seiji Hariki, 9793694
------------------------------------------------- */

#include "rider.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char** argv)
{
    if (argc < 4) {
        printf("Usage: ep2 d n v\n");
    }
    uint vel_len = atoi(argv[1]);
    uint rider_cnt = atoi(argv[2]);
    uint lap_cnt = atoi(argv[3]);

    globals.e = false;
    globals.d = false;
    globals.r = false;

    if (argc == 5)
        if (argv[4][0] == 'd')
            globals.d = true;
        else if (argv[4][0] == 'e')
            globals.e = true;

    srand(time(0));
    Velodrome velodrome = NULL;

    create_velodrome(&velodrome, vel_len, rider_cnt, lap_cnt);

    while (velodrome->a_rider_cnt > 0) {
        for (int i = 0; i < velodrome->rider_cnt; i++) {
            if (velodrome->riders[i].broken)
                pthread_join(velodrome->riders[i].rider_t, NULL);
        }
    }

    destroy_velodrome(&velodrome);
}
