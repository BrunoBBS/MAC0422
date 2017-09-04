#include "ep1/scheduler/robin.h"
#include <semaphore.h>
#include <stdlib.h>

//TODO: preciso de uma linked list pra guardar a lista de processos
//TODO: preciso que o scheduler marque o tempo
//TODO: preciso definir quanto é um quantum

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
}

void *rr(void *sch_init)
{
    return 0;
}

int rr_add_job(process *job)
{
    return 0;
}
