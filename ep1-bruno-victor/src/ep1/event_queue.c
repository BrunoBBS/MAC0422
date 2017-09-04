#include "ep1/event_queue.h"

void eq_notify(ev_queue *queue, scheduler_event event)
{
    ev_queue ptr = *queue;
    for (; ptr && ptr->next; ptr = ptr->next);
    
    ev_item *item = malloc(sizeof(struct event_queue_item));
    item->next = 0;
    item->event = event;

    if (!ptr)
        *queue = item;
    else
        ptr->next = item;
}

void eq_destroy(ev_queue *queue)
{
    ev_queue ptr = *queue;

    while (ptr)
    {
        ev_queue next = ptr->next;
        free(ptr);
        ptr = next;
    }
}

void eq_forall(ev_queue *queue, int (*action) (scheduler_event *))
{
    ev_queue ptr = *queue;
    for (; ptr; ptr = ptr->next)
        action(&ptr->event);
}
