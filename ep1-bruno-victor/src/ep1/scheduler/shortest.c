#include "ep1/scheduler/shortest.h"
#include "ep1/process.h"
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>

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
    // Initializes a linked list
    ll = 0;
    
    // Initializes semaphore
    sem_init(&ll_s, 0, 1);
}

void* sjf(void *sch_init)
{
    // Get definition
    scheduler_def *def = (scheduler_def*)sch_init;

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
                    // Free semaphore and set running_p as 0
                    sem_close(&running_p[i]->sem);
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
        }
    }

    sem_destroy(&ll_s);
    free(free_cpu_stack);
    free(running_p);
    return 0;
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
