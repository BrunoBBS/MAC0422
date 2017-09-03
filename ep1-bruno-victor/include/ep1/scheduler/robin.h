#ifndef __SCH_ROBIN_H__
#define __SCH_ROBIN_H__
#include "typedef.h"

void rr_init(void*);
void* rr(void*);
int rr_add_job(process*);

#endif
