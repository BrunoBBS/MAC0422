#include "ep1sh/commands/chown.h"
#include <grp.h>
#include <sys/types.h>

int chown_c(char *group, char *file)
{
    struct group *gp;
    gp = getgrnam(group);
    int ret = chown(file, -1, gp->gr_gid);
    return ret;
}
