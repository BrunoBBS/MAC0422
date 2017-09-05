#include "ep1/scheduler/robin.h"
#include "ep1/event_queue.h"
#include "ep1/process.h"
#include "typedef.h"
#include <semaphore.h>
#include <stdlib.h>

//TODO: preciso definir quanto é um quantum

//semaphore for the linked list
sem_t queue_s;
c_ll_item *queue;
int proc_count, t_elapsed;
const int QUANTUM = 200; //in milliseconds

void c_ll_insert(c_ll_item **root, process *proc)
{
    c_ll_item *ptr = *root;

    //finds last place in list
    for (; ptr && ptr->next; ptr = ptr->next)
    {
    }

    //allocates memory for new node
    c_ll_item *new_item = malloc(sizeof(struct c_ll_item));

    //if list is empty
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

int rr_add_job(process *job)
{
    sem_init(&(job->sem), 0, 1);
    pthread_create(&(job->thread), 0, &process_t, (void *)job);
    sem_wait(&queue_s);
    c_ll_insert(&queue, job);
    sem_post(&queue_s);
    proc_count++;
    return 0;
}

void rr_init(void *sch_init)
{
    if (globals.extra)
        printf("[RR ] Initializing Scheduler...\n");

    // Initializes a linked list
    queue = 0;
    proc_count = 0;

    // Initializes process queue semaphore
    sem_init(&queue_s, 0, 1);
}

void *rr(void *sch_init)
{
    // This is the event queue
    ev_queue events = 0;
    // Poiter to current process
    c_ll_item *p_pointer = queue;
    c_ll_item *pp_pointer;
    // Get definition
    scheduler_def *def = (scheduler_def *)sch_init;

    if (globals.extra)
        printf("[RR ] Scheduler has started!\n");

    //MULTICORES
    // This is the currently running processes
    /*unsigned int cpu_cnt = def->cpu_count;*/
    /*int *free_cpu_stack = malloc(cpu_cnt * sizeof(int));*/
    /*unsigned int *startt = malloc(cpu_cnt * sizeof(unsigned int));*/
    /*process **running_p = malloc(cpu_cnt * sizeof(process *));*/

    while (!def->ended)
    {
        if (proc_count > 0)
        {
            // Unlocks current thread
            sem_post(&(p_pointer->proc->sem));
            // Wait fo one QUANTUM
            while ((getttime() - def->syst0) % QUANTUM > 0.001)
            {
                // If the current process has ended
                if (p_pointer->proc->dt_dec == -1)
                {
                    if (globals.extra)
                        printf("[RR ] A process has finished\n");
                    unsigned int end_time = getttime();
                    // frees semaphore
                    sem_close(&(p_pointer->proc->sem));
                    // Removes from list
                    pp_pointer->next = p_pointer->next;
                    proc_count--;
                    break;
                }
            }
            // One quantum has passed and the process isnt finished
            // Pauses the thread
            sem_wait(&(p_pointer->proc->sem));
            if (globals.extra)
                printf("[RR ] Changing processes\n");
            //goes to the next process
            sem_wait(&queue_s);
            pp_pointer = p_pointer;
            p_pointer = p_pointer->next;
            sem_post(&queue_s);
        }
    }
}
