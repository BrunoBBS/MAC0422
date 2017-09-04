#include "typedef.h"

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/resource.h>
#include <sys/time.h>
#include <unistd.h>

#include "ep1/scheduler/robin.h"
#include "ep1/scheduler/shortest.h"

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
        time_str[last] = 0;
    }
    sscanf(time_str, "%u", &time);
    return time;
}

int order_by_t0(const void *el1, const void *el2)
{
    process *p1 = (process *)el1;
    process *p2 = (process *)el2;
    return p1->t0_dec - p2->t0_dec;
}

int get_processes(string filename, process **processes)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        printf("File '%s' could not be opened!\n", filename);
        return -1;
    }
    else
    {
        char line[MAX_PROCESS_LINE + 1];
        char t0_str[MAX_NUMBER_CHARS + 2];
        char dt_str[MAX_NUMBER_CHARS + 2];
        char dl_str[MAX_NUMBER_CHARS + 2];
        char name[MAX_PROCESS_NAME + 1];
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
            (*processes)[len].t0_dec = convert_time(t0_str);
            (*processes)[len].dt_dec = convert_time(dt_str);
            (*processes)[len].dl_dec = convert_time(dl_str);
            (*processes)[len].name =
                malloc((strlen(name) + 1) * sizeof(char));
            (*processes)[len].trace_line = len;
            strcpy((*processes)[len++].name, name);
        }
        qsort(*processes, len, sizeof(process), order_by_t0);
        fclose(file);
        return len;
    }
}

void user(int pc, process *pv, int (*add_job)(process *), time_t syst0)
{
    //sysdt is an integer where the last digit is a decimal
    int sysdt = 0;

    //for each process
    for (int i = 0; i < pc; i++)
    {
        do
        {
            //sets the tnow for the simulation
            struct rusage usage;
            struct timeval time_spent;
            getrusage(RUSAGE_THREAD, &usage);
            timeradd(&usage.ru_utime, &usage.ru_stime, &time_spent);
            int millis = (time_spent.tv_sec * 1000) + (time_spent.tv_usec / 1000);
            sysdt = millis / 100;
            //printf("%d é menor que %d\n", sysdt, pv[i].t0_dec);
        } while (sysdt < pv[i].t0_dec);

        add_job(&pv[i]);
        fprintf(stderr, "Process %s added at %.1f\n", pv[i].name, (float)sysdt / 10);
    }
}

int main(int argc, string *argv)
{
    if (argc < 2)
    {
        printf("Usage: ep1 <scheduler> <trace file> <output file>\n");
        exit(0);
    }

    process *processes;
    int proc_cnt;
    proc_cnt = get_processes(argv[2], &processes);

    //print process list
    for (int i = 0; i < proc_cnt; i++)
    {
        printf(
            "process: %s\nt0: %f\ndt: %f\ndeadline: %f\n\n",
            processes[i].name,
            ((float)processes[i].t0_dec) / 10,
            ((float)processes[i].dt_dec) / 10,
            ((float)processes[i].dl_dec) / 10);
    }

    //creates the thread for the scheduler
    pthread_t scheduler;

    //sets the t0 for the simulation
    struct rusage usage;
    struct timeval time_spent;
    getrusage(RUSAGE_THREAD, &usage);
    timeradd(&usage.ru_utime, &usage.ru_stime, &time_spent);
    int millis = (time_spent.tv_sec * 1000) + (time_spent.tv_usec / 1000);
    int syst0 = millis * 1000;

    //creates the scheduler definitions struct
    scheduler_def defs;
    defs.cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    defs.syst0 = syst0;

    switch (atoi(argv[1]))
    {
    //creates scheduler thread
    case 1:
        //shortest job first
        sjf_init(&defs);
        pthread_create(&scheduler, 0, &sjf, (void *)&defs);
        user(proc_cnt, processes, &sjf_add_job, syst0);
        break;
    case 2:
        //round robin
        rr_init(&defs);
        pthread_create(&scheduler, 0, &rr, (void *)&defs);
        user(proc_cnt, processes, &rr_add_job, syst0);
        break;
    case 3:
        //priority scheduler
        break;
    }

    pthread_join(scheduler, NULL);

    //cleans everything
    if (proc_cnt > 0)
        free(processes);
}
