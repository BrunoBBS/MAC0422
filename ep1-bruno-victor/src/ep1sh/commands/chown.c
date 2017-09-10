#include "ep1sh/commands/chown.h"
#include <grp.h>
#include <sys/types.h>
#include <ctype.h>

int chown_c(char* group, char* file)
{
    struct group* gp;
    int ret;
    if (isdigit(group[0])) {
        ret = chown(file, -1, atoi(group));
    } else {
        gp = getgrnam(group);
        ret = chown(file, -1, gp->gr_gid);
    }
    return ret;
}
