#ifndef __SCH_PRIORITY_H__
#define __SCH_PRIORITY_H__
#include "typedef.h"

typedef struct priority_ll_item
{
    process *proc;
    struct priority_ll_item *next;
} priority_ll_item;

void priority_ll_insert(priority_ll_item **, process *);
void priority_ll_remove(priority_ll_item **, process *);

void priority_init(void *);
void *priority(void *);
int priority_add_job(process *);

#endif
