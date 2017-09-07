#include "ep1/scheduler/priority.h"
#include "ep1/event_queue.h"
#include "ep1/process.h"
#include "typedef.h"
#include <semaphore.h>
#include <stdlib.h>

// semaphore for the linked list
sem_t queue_s;
priority_ll_item *queue;
const int PQUANTUM = 200; // in milliseconds
// Constatnts for  priority calculations
const float min_p = 1;   //minimum priority multiplier
const float max_p = 9;   //maximum priority multiplier
const float min_r = 0.1; //minimum threshold for minimum priority
const float max_r = 0.9; //maximum threshold for maximum priotiry

void priority_ll_insert(priority_ll_item **root, process *proc)
{
    priority_ll_item *item = malloc(sizeof(priority_ll_item));
    item->next = *root;
    proc->rem_milli = proc->dt_dec * 100;
    item->proc = proc;
    *root = item;
}

void priority_ll_remove(priority_ll_item **root, process *proc)
{
    if (!*root)
        return;

    if ((*root)->proc == proc)
    {
        priority_ll_item *next = (*root)->next;
        free(*root);
        *root = next;
    }
    else
    {
        priority_ll_item *ptr = *root;
        for (; ptr && ptr->next && ptr->next->proc != proc; ptr = ptr->next)
            ;
        if (ptr && ptr->next && ptr->next->proc == proc)
        {
            priority_ll_item *next = ptr->next->next;
            free(ptr->next);
            ptr->next = next;
        }
    }
}

unsigned int curr_round_p_cnt = 0;
unsigned int curr_round_p_size = 1;
process **curr_round_p;

int priority_add_job(process *job)
{
    sem_init(&(job->sem), 0, 0);
    pthread_create(&(job->thread), 0, &process_t, (void *)job);

    sem_wait(&queue_s);
    // Add it to currently running processes list
    priority_ll_insert(&queue, job);
    // Add it to current round
    if (curr_round_p_cnt + 1 >= curr_round_p_size)
        curr_round_p = realloc(curr_round_p,
                               (curr_round_p_size *= 2) * sizeof(process *));

    curr_round_p[curr_round_p_cnt++] = job;
    sem_post(&queue_s);

    return 0;
}

void priority_init(void *sch_init)
{
    if (globals.extra)
        printf("[PRIO] Initializing Scheduler...\n");

    // Initializes a linked list
    queue = 0;

    // Initializes process queue semaphore
    sem_init(&queue_s, 0, 1);

    // Initialized round stack
    curr_round_p = malloc(1 * sizeof(process *));
}

void *priority(void *sch_init)
{
    // This is the event queue
    ev_queue events = 0;

    // Get definition
    scheduler_def *def = (scheduler_def *)sch_init;

    if (globals.extra)
        printf("[PRIO] Scheduler has started!\n");

    // MULTICORES
    // This is the currently running processes
    unsigned int cpu_cnt = def->cpu_count;
    int *free_cpu_stack = malloc(cpu_cnt * sizeof(int));
    unsigned int *startt = malloc(cpu_cnt * sizeof(unsigned int));
    process **running_p = malloc(cpu_cnt * sizeof(process *));
    float *priorities = malloc(cpu_cnt * sizeof(float));

    for (int i = 0; i < cpu_cnt; i++)
        running_p[i] = 0;

    // Runs indefinitely
    while (1)
    {
        // Wait for free cpu
        unsigned int free_cpu_cnt = 0;
        do
        {
            unsigned int curr_time = getttime();
            for (int i = 0; i < cpu_cnt; i++)
            {
                // If process has ended
                if (running_p[i] && running_p[i]->dt_dec == -1)
                {
                    if (globals.extra)
                    {
                        printf("[PRIO] Process %s \e[31mended\e[0m at \e[34m%.1f\e[0m\n",
                               running_p[i]->name,
                               (float)curr_time / 1000);

                        printf("[PRIO] Deadline for %s was \e[34m%.1f\e[0m: %s\n",
                               running_p[i]->name,
                               (float)running_p[i]->dl_dec / 10,
                               (curr_time / 100) < running_p[i]->dl_dec ? "\e[32mOK\e[0m" : "\e[31mNOT OK\e[0m");
                        printf("[PRIO] Core \e[34m%d\e[0m freed!\n", i);
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
                    priority_ll_remove(&queue, running_p[i]);
                    sem_post(&queue_s);
                    for (int j = 0; j < curr_round_p_cnt; j++)
                        if (curr_round_p[j] == running_p[i])
                            curr_round_p[j] = curr_round_p[--curr_round_p_cnt];
                    running_p[i] = 0;
                }

                // If prioritized ammount of quantums has ended
                else if (running_p[i] && curr_time - startt[i] >=
                        priorities[i] * PQUANTUM)
                {
                    if (globals.extra)
                    {
                        printf("[PRIO] Process %s \e[33mpaused\e[0m at \e[34m%.1f\e[0m\n",
                               running_p[i]->name,
                               (float)curr_time / 1000);
                        printf("[PRIO] Core \e[34m%d\e[0m freed!\n", i);
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
        if (!curr_round_p_cnt)
        {
            sem_wait(&queue_s);
            // For each process on queue
            priority_ll_item *ptr;
            for (ptr = queue; ptr; ptr = ptr->next)
            {
                if (curr_round_p_cnt + 1 >= curr_round_p_size)
                    curr_round_p = realloc(curr_round_p,
                                           (curr_round_p_size *= 2) * sizeof(process *));

                curr_round_p[curr_round_p_cnt++] = ptr->proc;
            }
            sem_post(&queue_s);
        }

        // Search for a valid process in the stack
        // (Not being executed)
        process *to_resume = NULL;
        for (int i = 0; free_cpu_cnt && !to_resume && i < curr_round_p_cnt; i++)
        {
            // Is the process running?
            char running = 0;
            for (int core = 0; core < cpu_cnt; core++)
                running = running || running_p[core] == curr_round_p[i];

            // If it's not running, we can resume it
            if (!running)
            {
                to_resume = curr_round_p[i];
                curr_round_p[i] = curr_round_p[--curr_round_p_cnt];
            }
        }

        // Resume process for some core if there is something to run
        if (to_resume && free_cpu_cnt)
        {
            // Get free core
            int core = free_cpu_stack[--free_cpu_cnt];

            // Get current time
            unsigned int curr_time = getttime();

            // Calculates priority
            float perc = (float)to_resume->rem_milli /
                (((float) to_resume->dl_dec * 100) - curr_time);

            priorities[core] =
                perc < min_r && perc > 0 ? min_p :
                perc > max_r || perc < 0 ? max_p :
                (perc - min_r) / (max_r - min_r) * (max_p - min_p) + min_p;

            // Unlock process' semaphore
            sem_post(&to_resume->sem);

            // Bind core to process
            running_p[core] = to_resume;

            // Set start time
            startt[core] = curr_time;

            // Add event to queue
            scheduler_event event;
            event.event_t = PROCESS_RESUMED;
            event.core = core;
            event.proc = to_resume;
            event.timestamp_millis = startt[core];
            eq_notify(&events, event);

            if (globals.extra)
            {
                printf("[PRIO] Process %s \e[32mresumed\e[0m at \e[34m%.1f\e[0m\n",
                       to_resume->name,
                       (float) event.timestamp_millis / 1000);
                printf("[PRIO] Process %s has now priority \e[34m%.3f\e[0m - Quantum of \e[34m%.1f\e[0m\n",
                        to_resume->name,
                        priorities[core],
                        PQUANTUM * priorities[core]);
                printf("[PRIO] Process %s running at core \e[34m%d\e[0m\n",
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
    free(curr_round_p);
    free(priorities);
    sem_destroy(&queue_s);
    return events;
}
