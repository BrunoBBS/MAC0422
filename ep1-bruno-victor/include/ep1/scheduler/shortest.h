#ifndef __SCH_SHORTEST_H__
#define __SCH_SHORTEST_H__
#include "typedef.h"

typedef struct lq_item
{
    process *proc;
    struct lq_item *next;
} lq_item;

typedef lq_item *linked_queue;

void lq_insert(linked_queue *, process *);
process *lq_get(linked_queue *);

void sjf_init(void *);
void *sjf(void *);
int sjf_add_job(process *);

#endif
