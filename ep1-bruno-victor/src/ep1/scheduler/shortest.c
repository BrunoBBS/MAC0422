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
    // This is the currently running process
    pthread_t running_t;
    process *running = 0;

    // Runs indefinitely
    while (1)
    {
        // Wait until there is a process to be run
        do {
            sem_wait(&ll_s);
            running = lq_get(&ll);
            sem_post(&ll_s);
        } while (!running);

        // Initialized process semaphore
        sem_init(&(running->sem), 0, 1);

        // Creates process
        pthread_create(&running_t, 0, &process_t, (void*) running);

        // Waits for it to finish
        pthread_join(running_t, NULL);

        // Frees semaphore
        sem_destroy(&(running->sem));

        // Now no process ir running
        running = 0;
    }

    sem_destroy(&ll_s);
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
