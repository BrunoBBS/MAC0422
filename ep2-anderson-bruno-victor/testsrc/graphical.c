#include "velodrome.h"
#include "rider.h"

#include <sys/ioctl.h>
#include <unistd.h>

int main()
{
    uint vel_len = 150;
    uint rider_cnt = 15;

    struct winsize w;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);

    printf ("Terminal of size: %d x %d\n", w.ws_col, w.ws_row);
    globals.e = true;

    Velodrome velodrome = NULL;

    create_velodrome(&velodrome, vel_len, rider_cnt);

    destroy_velodrome(&velodrome);
}
