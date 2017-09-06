#ifndef __SCH_ROBIN_H__
#define __SCH_ROBIN_H__
#include "typedef.h"

typedef struct rr_ll_item
{
    process *proc;
    struct rr_ll_item *next;
} rr_ll_item;

void rr_ll_insert(rr_ll_item **, process *);
void rr_ll_remove(rr_ll_item **, process *);

void rr_init(void *);
void *rr(void *);
int rr_add_job(process *);

#endif
