#include "ep1/scheduler/robin.h"
#include <semaphore.h>
#include <stdlib.h>
#include "typedef.h"

//TODO: preciso de uma linked list pra guardar a lista de processos
//TODO: preciso que o scheduler marque o tempo
//TODO: preciso definir quanto é um quantum

//semaphore for the linked list
sem_t ll_s;
linked_queue ll = 0;

void c_ll_insert(c_ll_item **root, process *proc)
{
    c_ll_item *ptr = *root;

    for (; ptr && ptr->next &&
           ptr->next->proc->dt_dec <= proc->dt_dec;
         ptr = ptr->next)
        ;
    c_ll_item *new_item = malloc(sizeof(struct c_ll_item));

    if (!*root)
    {
        new_item->next = *root;
        *root = new_item;
    }
    else
    {
        new_item->next = ptr->next;
        ptr->next = new_item;
    }
    new_item->proc = proc;
}

void rr_init(void *sch_init)
{
    if (globals.extra)
        printf("[RR ] Initializing Scheduler...\n");

    // Initializes a linked list
    c_ll = 0;

    // Initializes semaphore
    sem_init(&ll_s, 0, 1);
}

void *rr(void *sch_init)
{
    // This is the event queue
    ev_queue events = 0;

    // Get definition
    scheduler_def *def = (scheduler_def*)sch_init;

    if (globals.extra)
        printf("[RR ] Scheduler has started!\n");

    // This is the currently running processes
    unsigned int cpu_cnt = def->cpu_count;
    int *free_cpu_stack = malloc(cpu_cnt * sizeof(int));
    unsigned int *startt = malloc(cpu_cnt * sizeof(unsigned int));
    process **running_p = malloc(cpu_cnt * sizeof(process*));


}

int rr_add_job(process *job)
{
    return 0;
}
