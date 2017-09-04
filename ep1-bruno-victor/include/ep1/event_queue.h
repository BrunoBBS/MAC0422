#ifndef __EVENT_QUEUE_H__
#define __EVENT_QUEUE_H__
#include "typedef.h"
// Types

// Types of events to handle
typedef enum event_type
{
    PROCESS_ADDED   = 1,
    PROCESS_STARTED = 2,
    PROCESS_PAUSED  = 3,
    PROCESS_RESUMED = 4,
    PROCESS_ENDED   = 5,
} event_type;

// Event data
typedef struct scheduler_event
{
    event_type event_t;
    int core;
    process *proc;
    unsigned int timestamp_millis;
} scheduler_event;

// Item of event queue
typedef struct event_queue_item
{
    scheduler_event event;
    struct event_queue_item *next;
} event_queue_item;

typedef event_queue_item ev_item;
typedef event_queue_item* ev_queue;

// Operations

// Adds event to queue
void eq_notify(ev_queue *queue, scheduler_event event);

// Frees queue
void eq_destroy(ev_queue *queue);

// Runs a function for all items until return is different from 0
void eq_forall(ev_queue *queue, int (*action) (scheduler_event *));

#endif
