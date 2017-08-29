#include "ep1sh/commands/date.h"

typedef unsigned int uint;

int date_c ()
{
    time_t now = time(NULL);
    char out[50];
    strftime(out, 50, "%a %b %e %T %Z %Y", localtime(&now));
    printf("%s\n", out);
    return 0;
}
