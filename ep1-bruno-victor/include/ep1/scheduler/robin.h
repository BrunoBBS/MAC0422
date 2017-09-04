#ifndef __SCH_ROBIN_H__
#define __SCH_ROBIN_H__
#include "typedef.h"

typedef struct c_ll_item
{
    process *proc;
    struct c_ll_item *next;
} c_ll_item;

void c_ll_insert(c_ll_item **, process *);
process *c_ll_get(c_ll_item **);

void rr_init(void *);
void *rr(void *);
int rr_add_job(process *);

#endif
