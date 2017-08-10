#include "ep1sh/commands/chown.h"

int main (int argc, char** argv)
{
    chown_c(argv[1], argv[2]);
}
