#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "typedef.h"

#define MAX_PROCESS_LINE 100
#define MAX_PROCESS_NAME 80
#define MAX_NUMBER_CHARS 10

int convert_time(string time_str)
{
    char *dot_loc;
    unsigned int time;
    if ((dot_loc = strchr(time_str, '.')))
    {
        *dot_loc = dot_loc[1];
        dot_loc[1] = 0;
    }
    else
    {
        unsigned int last = strlen(time_str);
        time_str[last++] = '0';
        time_str[last  ] = 0;
    }
    sscanf(time_str, "%u", &time);
    return time;
}

int order_by_t0(const void *el1, const void *el2)
{
    process *p1 = (process*) el1;
    process *p2 = (process*) el2;
    return p1->t0_dec - p2->t0_dec;
}

int get_processes(string filename, process **processes)
{
    FILE *file = fopen(filename, "r");
    if (!file) {
        printf("File '%s' could not be opened!\n", filename);
        return -1;
    }
    else
    {
        char line  [MAX_PROCESS_LINE + 1];
        char t0_str[MAX_NUMBER_CHARS + 2];
        char dt_str[MAX_NUMBER_CHARS + 2];
        char dl_str[MAX_NUMBER_CHARS + 2];
        char name  [MAX_PROCESS_NAME + 1];
        int len = 0;
        int vsize = 1;
        *processes = malloc(vsize * sizeof(process));
        while (fgets(line, MAX_PROCESS_LINE, file))
        {
            if (len == vsize - 1)
            {
                vsize *= 2;
                process *proc = realloc(*processes, vsize * sizeof(process));
                if (!proc)
                    return -2;
                *processes = proc;
            }
            sscanf(line, "%s %s %s %s", t0_str, dt_str, dl_str, name);
            (*processes)[len  ].t0_dec = convert_time(t0_str);
            (*processes)[len  ].dt_dec = convert_time(dt_str);
            (*processes)[len  ].dl_dec = convert_time(dl_str);
            (*processes)[len  ].name =
                malloc((strlen(name) + 1) * sizeof(char));
            strcpy((*processes)[len++].name, name);
        }
        qsort(*processes, len, sizeof(process), order_by_t0);
        fclose(file);
        return len;
    }
}

int main (int argc, string *argv)
{
    if (argc < 2)
    {
        printf("Usage: ep1 <processes file>\n");
        exit(0);
    }

    process *processes;
    int proc_cnt;
    proc_cnt = get_processes(argv[1], &processes);

    for (int i = 0; i < proc_cnt; i++)
    {
        printf (
                "process: %s\nt0: %f\ndt: %f\ndeadline: %f\n\n",
                processes[i].name,
                ((float)processes[i].t0_dec) / 10,
                ((float)processes[i].dt_dec) / 10,
                ((float)processes[i].dl_dec) / 10);
    }
    if (proc_cnt > 0)
        free(processes);
}
