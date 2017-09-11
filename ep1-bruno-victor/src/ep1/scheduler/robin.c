#include "ep1/scheduler/robin.h"
#include "ep1/event_queue.h"
#include "ep1/process.h"
#include "typedef.h"
#include <semaphore.h>
#include <stdlib.h>

// TODO: preciso definir quanto é um quantum

// semaphore for the linked list
sem_t queue_s;
rr_ll_item *queue;
const int QUANTUM = 200; // in milliseconds

void rr_ll_insert(rr_ll_item **root, process *proc)
{
    rr_ll_item *item = malloc(sizeof(rr_ll_item));
    item->next = *root;
    item->proc = proc;
    *root = item;
}

void rr_ll_remove(rr_ll_item **root, process *proc)
{
    if (!*root)
        return;

    if ((*root)->proc == proc)
    {
        rr_ll_item *next = (*root)->next;
        free(*root);
        *root = next;
    }
    else
    {
        rr_ll_item *ptr = *root;
        for (; ptr && ptr->next && ptr->next->proc != proc; ptr = ptr->next);
        if (ptr && ptr->next && ptr->next->proc == proc)
        {
            rr_ll_item *next = ptr->next->next;
            free(ptr->next);
            ptr->next = next;
        }
    }
}

unsigned int curr_round_cnt = 0;
unsigned int curr_round_size = 1;
process **curr_round;

int rr_add_job(process *job)
{
    sem_init(&(job->sem), 0, 0);
    pthread_create(&(job->thread), 0, &process_t, (void *)job);

    sem_wait(&queue_s);
    // Add it to currently running processes list
    rr_ll_insert(&queue, job);
    // Add it to current round
    if (curr_round_cnt + 1 >= curr_round_size)
        curr_round = realloc(curr_round,
                (curr_round_size *= 2) * sizeof(process *));

    curr_round[curr_round_cnt++] = job;
    sem_post(&queue_s);
    if (globals.debug)
        fprintf(stderr,"[RR  ] Process %s, line: %d arrived at system\n", job->name, job->trace_line);

    return 0;
}

void rr_init(void *sch_init)
{
    if (globals.extra)
        printf("[RR  ] Initializing Scheduler...\n");

    // Initializes a linked list
    queue = 0;

    // Initializes process queue semaphore
    sem_init(&queue_s, 0, 1);

    // Initialized round stack
    curr_round = malloc(1 * sizeof(process *));
}

void *rr(void *sch_init)
{
    // This is the event queue
    ev_queue events = 0;

    // Get definition
    scheduler_def *def = (scheduler_def *) sch_init;

    if (globals.extra)
        printf("[RR  ] Scheduler has started!\n");

    // MULTICORES
    // This is the currently running processes
    unsigned int cpu_cnt = def->cpu_count;
    int *free_cpu_stack = malloc(cpu_cnt * sizeof(int));
    unsigned int * startt = malloc(cpu_cnt * sizeof(unsigned int));
    process **running_p = malloc(cpu_cnt * sizeof(process *));

    for (int i = 0; i < cpu_cnt; i++)
        running_p[i] = 0;

    // Runs indefinitely
    while (1)
    {
        // Wait for free cpu
        unsigned int free_cpu_cnt = 0;
        do {
            unsigned int curr_time = getwtime();
            for (int i = 0; i < cpu_cnt; i++)
            {
                // If process has ended
                if (running_p[i] && running_p[i]->dt_dec == -1)
                {
                    if (globals.debug)
                    {
                        fprintf(stderr,"[RR  ] Process %s has left virtual CPU %d\n", running_p[i]->name, i);
                        fprintf(stderr,"[RR  ] Process %s, at line %d has ended \n", running_p[i]->name, running_p[i]->trace_line);
                    }
                    if (globals.extra)
                    {
                        printf("[RR  ] Process %s \e[31mended\e[0m at \e[34m%.1f\e[0m\n",
                                running_p[i]->name,
                                (float) curr_time / 1000);

                        printf("[RR  ] Deadline for %s was \e[34m%.1f\e[0m: %s\n",
                                running_p[i]->name,
                                (float) running_p[i]->dl_dec / 10,
                                (curr_time / 100) < running_p[i]->dl_dec ?
                                "\e[32mOK\e[0m" :
                                "\e[31mNOT OK\e[0m");
                        printf("[RR  ] Core \e[34m%d\e[0m freed!\n", i);
                    }


                    // Add event to queue
                    scheduler_event event;
                    event.event_t = PROCESS_ENDED;
                    event.core = i;
                    event.proc = running_p[i];
                    event.timestamp_millis = curr_time;
                    event.extra_data.u = running_p[i]->t0_dec * 100;
                    eq_notify(&events, event);

                    // Free semaphore, remove process from queue and round
                    // and set running_p as 0
                    sem_destroy(&running_p[i]->sem);
                    sem_wait(&queue_s);
                    //for (rr_ll_item *ptr = queue; ptr; ptr = ptr->next)
                    //    printf("%s%s", ptr->proc->name, ptr->next?" -> ":"\n");
                    rr_ll_remove(&queue, running_p[i]);
                    sem_post(&queue_s);
                    //for (int j = 0; j < curr_round_cnt; j++)
                    //    printf("%s%s", curr_round[j]->name, (j < curr_round_cnt - 1)?" -> ":"\n");
                    for (int j = 0; j < curr_round_cnt; j++)
                        if (curr_round[j] == running_p[i])
                            curr_round[j] = curr_round[--curr_round_cnt];
                    running_p[i] = 0;

                }
                // If quantum has ended
                else if (running_p[i] && curr_time - startt[i] >= QUANTUM)
                {
                    if (globals.debug)
                        fprintf(stderr,"[RR  ] Process %s has left virtual CPU %d\n", running_p[i]->name, i);
                    if (globals.extra)
                    {
                        printf("[RR  ] Process %s \e[33mpaused\e[0m at \e[34m%.1f\e[0m\n",
                                running_p[i]->name,
                                (float) curr_time / 1000
                              );
                        printf("[RR  ] Core \e[34m%d\e[0m freed!\n", i);
                    }

                    // Add event to queue
                    scheduler_event event;
                    event.event_t = PROCESS_PAUSED;
                    event.core = i;
                    event.proc = running_p[i];
                    event.timestamp_millis = curr_time;
                    event.extra_data.u = running_p[i]->t0_dec * 100;
                    eq_notify(&events, event);

                    // Block semaphore and set running_p as 0
                    sem_wait(&running_p[i]->sem);
                    running_p[i] = 0;
                }

                // Add cpu to free cpu list
                if (!running_p[i])
                    free_cpu_stack[free_cpu_cnt++] = i;
            }
        } while (!free_cpu_cnt);

        // If stack is empty, add processes to the new round
        if (!curr_round_cnt)
        {
            sem_wait(&queue_s);
            // For each process on queue
            rr_ll_item *ptr;
            for (ptr = queue; ptr; ptr = ptr->next)
            {
                if (curr_round_cnt + 1 >= curr_round_size)
                    curr_round = realloc(curr_round,
                            (curr_round_size *= 2) * sizeof(process *));

                curr_round[curr_round_cnt++] = ptr->proc;
            }
            sem_post(&queue_s);
        }

        // Search for a valid process in the stack
        // (Not being executed)
        process *to_resume = NULL;
        for (int i = 0; free_cpu_cnt && !to_resume && i < curr_round_cnt; i++)
        {
            // Is the process running?
            char running = 0;
            for (int core = 0; core < cpu_cnt; core++)
                running = running || running_p[core] == curr_round[i];

            // If it's not running, we can resume it
            if (!running)
            {
                to_resume = curr_round[i];
                curr_round[i] = curr_round[--curr_round_cnt];
            }
        }

        // Resume process for some core if there is something to run
        if (to_resume && free_cpu_cnt)
        {
            // Get free core
            int core = free_cpu_stack[--free_cpu_cnt];

            // Unlock process' semaphore
            sem_post(&to_resume->sem);

            // Bind core to process
            running_p[core] = to_resume;

            // Set start time
            startt[core] = getwtime();

            // Add event to queue
            scheduler_event event;
            event.event_t = PROCESS_RESUMED;
            event.core = core;
            event.proc = to_resume;
            event.timestamp_millis = startt[core];
            eq_notify(&events, event);

            if (globals.debug)
                fprintf(stderr,"[RR  ] Process %s is using virtual CPU %d\n", to_resume->name, core);
            if (globals.extra)
            {
                printf("[RR  ] Process %s \e[32mresumed\e[0m at \e[34m%.1f\e[0m\n",
                        to_resume->name,
                        (float) event.timestamp_millis / 1000);
                printf("[RR  ] Process %s running at core \e[34m%d\e[0m\n",
                        to_resume->name,
                        core);
            }
        }

        // If there are no processes in queue, no processes running, and
        // user has finihsed inserting processes, exit
        if (!queue && free_cpu_cnt == cpu_cnt && def->ended)
            break;
    }

    free(free_cpu_stack);
    free(curr_round);
    sem_destroy(&queue_s);
    return events;
}
