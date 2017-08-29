#include "ep1sh/commands/chown.h"
#include <grp.h>
#include <sys/types.h>

int chown_c(char *group, char *file)
{
    printf("%s", group);
    struct group *gp;
    printf("2");
    gp = getgrnam("licn");
    printf("3");
    fflush(stdout);
    int ret = chown(file, -1, gp->gr_gid);
    fflush(stdout);
    free(gp);
    printf("4");
    fflush(stdout);
    return ret;
}
