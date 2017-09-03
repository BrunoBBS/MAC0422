#include "ep1/scheduler/shortest.h"
#include <semaphore.h>
#include <stdlib.h>

//semaphore for athe linked list
sem_t ll_s;
linked_queue ll = 0;

void lq_insert(linked_queue *queue, process *proc)
{
    linked_queue ptr = *queue;
    for (; ptr->next &&
           ptr->next->proc->dt_dec <= proc->dt_dec;
         ptr = ptr->next)
        ;
    lq_item *new_item = malloc(sizeof(struct lq_item));
    if (*queue && (*queue)->proc->dt_dec <= proc->dt_dec)
    {
        ptr->next = new_item;
        new_item->next = ptr->next;
    }
    else
    {
        new_item->next = *queue;
        *queue = new_item;
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

void* rr(void *sch_init)
{
    //initializes a linked list
    ll = 0;
    sem_init(&ll_s, 0, 1);
    sem_close(&ll_s);
}

int rr_add_job(process *job)
{
    int code;
    if (sem_getvalue(&ll_s, &code))
        return -1;
    sem_wait(&ll_s);
    lq_insert(&ll, job);
    sem_post(&ll_s);
    return 0;
}
