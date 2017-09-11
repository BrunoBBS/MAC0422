#include "ep1/event_queue.h"
#include "ep1/scheduler/priority.h"
#include "ep1/scheduler/robin.h"
#include "ep1/scheduler/shortest.h"
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
            (*processes)[len].trace_line = len + 1;
            strcpy((*processes)[len++].name, name);
        }
        qsort(*processes, len, sizeof(process), order_by_t0);
        fclose(file);
        return len;
    }
}

void user(int pc, process *pv, int (*add_job)(process *), time_t syst0, scheduler_def *def)
{
    //sysdt is an integer where the last digit is a decimal
    int sysdt = 0;

    //for each process
    for (int i = 0; i < pc; i++)
    {
        // Wait to add next process
        do {
            sysdt = getwtime() / 100;
        } while (sysdt < pv[i].t0_dec);

        add_job(&pv[i]);
        if (globals.extra)
            printf("[USER] Process %s added at \e[34m%.1f\e[0m\n", pv[i].name, (float)sysdt / 10);
    }

    // Notify scheduler it has finished inserting processes
    def->ended = 1;
}

int output_event(scheduler_event *event, void *args)
{
    FILE *file = (FILE *)args;
    if (event->event_t == PROCESS_ENDED)
    {
        fprintf(file, "%s %.1f %.1f\n",
                event->proc->name,
                (float)event->timestamp_millis / 1000,
                (float)(event->timestamp_millis - event->extra_data.u) / 1000);
    }

    return 0;
}

int count_resumes(scheduler_event *event, void *args)
{
    if (event->event_t == PROCESS_RESUMED)
        (*(unsigned int *) args)++;

    return 0;
}

int print_event(scheduler_event *event, void *args)
{
    printf("\n[EVN ] Event type   : ");
    switch (event->event_t)
    {
    case PROCESS_ADDED:
        printf("PROCESS_ADDED");
        break;
    case PROCESS_PAUSED:
        printf("PROCESS_PAUSED");
        break;
    case PROCESS_RESUMED:
        printf("PROCESS_RESUMED");
        break;
    case PROCESS_ENDED:
        printf("PROCESS_ENDED");
        break;
    case PROCESS_STARTED:
        printf("PROCESS_STARTED");
        break;
    default:
        printf("UNKNOWN");
    }
    printf("\n");
    printf("[EVN ] Event process: %s\n", event->proc->name);
    printf("[EVN ] Event core   : %d\n", event->core);
    printf("[EVN ] Event time   : %.1f\n",
           (float)event->timestamp_millis / 1000);

    return 0;
}

int main(int argc, string *argv)
{
    if (argc < 4)
    {
        printf("Usage: ep1 <scheduler> <trace file> <output file>\n");
        exit(0);
    }

    globals.debug = 0;
    globals.extra = 0;

    if (argc == 5)
    {
        char *options = argv[4];
        int option_cnt = strlen(options);
        for (int option = 0; option < option_cnt; option++)
        {
            switch (options[option])
            {
            case 'd':
                globals.debug = 1;
                break;
            case 'e':
                globals.extra = 1;
                break;
            default:
                printf("Unrecognized option '%c'\n", options[option]);
            }
        }
    }

    process *processes;
    int proc_cnt;
    proc_cnt = get_processes(argv[2], &processes);
    if (proc_cnt < 0)
        exit(-1);

    //print process list
    if (globals.extra)
    {
        for (int i = 0; i < proc_cnt; i++)
        {
            printf(
                "process: %s\nt0: %f\ndt: %f\ndeadline: %f\n\n",
                processes[i].name,
                ((float)processes[i].t0_dec) / 10,
                ((float)processes[i].dt_dec) / 10,
                ((float)processes[i].dl_dec) / 10);
        }
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
    defs.ended = 0;
    defs.cpu_count = sysconf(_SC_NPROCESSORS_ONLN);
    defs.syst0 = syst0;

    if (globals.extra)
        printf("[MAIN] Detected \e[34m%d\e[0m cores in this machine!\n",
               defs.cpu_count);

    // Reference t0
    gettimeofday(&globals.t0, NULL);

    switch (atoi(argv[1]))
    {
    //creates scheduler thread
    case 1:
        //shortest job first
        if (globals.extra)
            printf("[MAIN] Using \e[34mShortest Job First\e[0m Scheduler\n");
        sjf_init(&defs);
        pthread_create(&scheduler, 0, &sjf, (void *)&defs);
        user(proc_cnt, processes, &sjf_add_job, syst0, &defs);
        break;
    case 2:
        if (globals.extra)
            printf("[MAIN] Using \e[34mRound Robin\e[0m Scheduler\n");
        //round robin
        rr_init(&defs);
        pthread_create(&scheduler, 0, &rr, (void *)&defs);
        user(proc_cnt, processes, &rr_add_job, syst0, &defs);
        break;
    case 3:
        if (globals.extra)
            printf("[MAIN] Using \e[34mPriority\e[0m Scheduler\n");
        //priority scheduler
        priority_init(&defs);
        pthread_create(&scheduler, 0, &priority, (void *)&defs);
        user(proc_cnt, processes, &priority_add_job, syst0, &defs);
        break;
    }

    ev_queue events;

    pthread_join(scheduler, (void **)&events);

    if (globals.extra)
        eq_forall(&events, &print_event, NULL);

    // Write to output file
    FILE *file = fopen(argv[3], "w");
    if (!file)
    {
        fprintf(stderr, "File '%s' could not be opened!\n", argv[3]);
    }
    else
    {
        if (globals.extra)
            printf("[MAIN] Writing output file '%s'\n", argv[3]);
        eq_forall(&events, &output_event, (void *)file);
        fclose(file);
    }
    unsigned int resume_cnt;

    eq_forall(&events, &count_resumes, (void *)&resume_cnt);

    if (globals.debug)
        fprintf(stderr, "[MAIN] Quantidade final de mudanças de contexto: %u\n",
                resume_cnt);

    eq_destroy(&events);
    //cleans everything
    if (proc_cnt > 0)
        free(processes);
}
