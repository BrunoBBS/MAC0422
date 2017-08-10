#include "ep1sh/commands/date.h"
#include "ep1sh/typedef.h"
#include <readline/history.h>
#include <readline/readline.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <sys/syscall.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

command parser(string line)
{
    string token;
    command com;
    int length = 1;
    int tknum = 0;

    token = strtok(line, " ");
    com.argv = malloc(length * sizeof(string));
    com.argv[0] = token;
    com.argc = 1;

    while (token = strtok(NULL, " "))
    {
        if (com.argc == length - 1)
        {
            length = length * 2;
            com.argv = realloc(com.argv, length * sizeof(string));
        }
        com.argv[com.argc++] = token;
    }
    com.argv[com.argc] = 0;
    return com;
}

int main()
{
    string ps1, input;
    using_history();
    do
    {
        string wd = getcwd(NULL, 0);
        ps1 = malloc((strlen(wd) + 3) * sizeof(char));
        sprintf(ps1, "[%s]$ ", wd);
        input = readline(ps1);
        add_history(input);
        command cmd = parser(input);
        if (!strcmp(cmd.argv[0], "exit"))
            input = 0;
        else if (!strcmp(cmd.argv[0], "date"))
            date_c();
        else
        {
            int pid = fork();
            if (!pid)
            {
                int res = execv(cmd.argv[0], cmd.argv);
                if (res == -1)
                {
                    printf("Failed to run program %s\n", cmd.argv[0]);
                    exit(-1);
                }
            }
            else if (pid == -1)
                printf("Could not create process for %s\n", cmd.argv[0]);
            else
                waitpid(pid, 0, 0);
        }
        free(cmd.argv);
        free(ps1);
        free(input);
    } while (input);
}
