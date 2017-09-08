#include "ep1/scheduler/shortest.h"
#include "ep1/process.h"
#include "ep1/event_queue.h"

//semaphore for athe linked list
sem_t ll_s;
linked_queue ll = 0;

void lq_insert(linked_queue *queue, process *proc)
{
    linked_queue ptr = *queue;

    for (; ptr && ptr->next &&
            ptr->next->proc->dt_dec <= proc->dt_dec; ptr = ptr->next);
    lq_item *new_item = malloc(sizeof(struct lq_item));

    if (!*queue || (*queue)->proc->dt_dec > proc->dt_dec)
    {
        new_item->next = *queue;
        *queue = new_item;
    }
    else
    {
        new_item->next = ptr->next;
        ptr->next      = new_item;
    }
    new_item->proc = proc;
}

process *lq_get(linked_queue *queue)
{
    if (!*queue)
        return NULL;
    process *ret = (*queue)->proc;
    linked_queue ptr = *queue;
    *queue = (*queue)->next;
    free(ptr);
    return ret;
}

void sjf_init(void *sch_init)
{
    if (globals.extra)
        printf("[SJF ] Initializing Scheduler...\n");

    // Initializes a linked list
    ll = 0;

    // Initializes semaphore
    sem_init(&ll_s, 0, 1);
}

void* sjf(void *sch_init)
{
    // This is the event queue
    ev_queue events = 0;

    // Get definition
    scheduler_def *def = (scheduler_def*)sch_init;

    if (globals.extra)
        printf("[SJF ] Scheduler has started!\n");

    // This is the currently running processes
    unsigned int cpu_cnt = def->cpu_count;
    int *free_cpu_stack = malloc(cpu_cnt * sizeof(int));
    process **running_p = malloc(cpu_cnt * sizeof(process*));

    for (int i = 0; i < cpu_cnt; i++)
        running_p[i] = 0;

    // Runs indefinitely
    while (1)
    {
        // Wait for free cpu
        unsigned int free_cpu_cnt = 0;
        do {
            for (int i = 0; i < cpu_cnt; i++)
            {
                // If process has ended
                if (running_p[i] && running_p[i]->dt_dec == -1)
                {
                    unsigned int end_time = getwtime();
                    if (globals.extra)
                    {
                        printf("[SJF ] Process %s \e[31mended\e[0m at \e[34m%.1f\e[0m\n",
                                running_p[i]->name,
                                (float) end_time / 1000);

                        printf("[SJF ] Deadline for %s was \e[34m%.1f\e[0m: %s\n",
                                running_p[i]->name,
                                (float) running_p[i]->dl_dec / 10,
                                (end_time / 100) < running_p[i]->dl_dec ?
                                "\e[32mOK\e[0m" :
                                "\e[31mNOT OK\e[0m");
                        printf("[SJF ] Core \e[34m%d\e[0m freed!\n", i);
                    }

                    // Add event to queue
                    scheduler_event event;
                    event.event_t = PROCESS_ENDED;
                    event.core = i;
                    event.proc = running_p[i];
                    event.timestamp_millis = end_time;
                    event.extra_data.u = running_p[i]->t0_dec * 100;
                    eq_notify(&events, event);

                    // Free semaphore and set running_p as 0
                    sem_destroy(&running_p[i]->sem);
                    running_p[i] = 0;
                }

                // Add cpu to free cpu list
                if (!running_p[i])
                    free_cpu_stack[free_cpu_cnt++] = i;
            }
        } while (!free_cpu_cnt);

        // Get next job from the queue
        sem_wait(&ll_s);
        process *to_run = lq_get(&ll);
        sem_post(&ll_s);

        // If there is something to run
        if (to_run)
        {
            // Initialize process semaphore
            sem_init(&(to_run->sem), 0, 1);

            // Get core
            int core = free_cpu_stack[--free_cpu_cnt];

            // Binds process to core
            running_p[core] = to_run;

            // Creates process
            pthread_create(
                    &(running_p[core]->thread),
                    0,
                    &process_t,
                    (void*) running_p[core]);

            // Add event to queue
            scheduler_event event;
            event.event_t = PROCESS_STARTED;
            event.core = core;
            event.proc = to_run;
            event.timestamp_millis = getwtime();
            eq_notify(&events, event);

            if (globals.extra)
            {
                printf("[SJF ] Process %s \e[32mstarted\e[0m at \e[34m%.1f\e[0m\n",
                        to_run->name,
                        (float) event.timestamp_millis / 1000);
                printf("[SJF ] Process %s running at core \e[34m%d\e[0m\n",
                        to_run->name,
                        core);
            }
        }

        // If there is nothing to run, all cpus are free (there is nothing
        // running), and user thread has ended inserting processes, stop
        // scheduler
        if (!to_run && free_cpu_cnt == cpu_cnt && def->ended)
            break;
    }

    sem_destroy(&ll_s);
    free(free_cpu_stack);
    free(running_p);
    return events;
}

int sjf_add_job(process *job)
{
    int code;
    if (sem_getvalue(&ll_s, &code))
        return -1;
    sem_wait(&ll_s);
    lq_insert(&ll, job);
    sem_post(&ll_s);
    return 0;
}
