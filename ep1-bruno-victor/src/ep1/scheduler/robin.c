#include "ep1/scheduler/robin.h"
#include "ep1/event_queue.h"
#include "ep1/process.h"
#include "typedef.h"
#include <semaphore.h>
#include <stdlib.h>

// TODO: preciso definir quanto é um quantum

// semaphore for the linked list
sem_t queue_s;
c_ll_item *queue;
int proc_count, proc_idle;
const int QUANTUM = 2000; // in milliseconds

void c_ll_insert(c_ll_item **root, process *proc)
{
    c_ll_item *ptr = *root;

    // finds last place in list
    for (int i = 0; ptr && ptr->next != *root && i < proc_count;
         ptr = ptr->next)
    {
        i++;
    }

    // allocates memory for new node
    c_ll_item *new_item = malloc(sizeof(struct c_ll_item));

    // if list is empty
    if (!*root)
    {
        *root = new_item;
        new_item->next = *root;
    }
    else
    {
        ptr->next = new_item;
        new_item->next = *root;
        printf("ptr = %s, new = %s, next = %s", ptr->proc->name, new_item->proc->name);
    }

    proc->state = 0;
    new_item->proc = proc;
}

int rr_add_job(process *job)
{
    sem_init(&(job->sem), 0, 0);
    pthread_create(&(job->thread), 0, &process_t, (void *)job);
    sem_wait(&queue_s);
    c_ll_insert(&queue, job);
    sem_post(&queue_s);
    proc_count++;
    proc_idle++;
    return 0;
}

void rr_init(void *sch_init)
{
    if (globals.extra)
        printf("[RR ] Initializing Scheduler...\n");

    // Initializes a linked list
    queue = 0;
    proc_count = 0;
    proc_idle = 0;

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

    // MULTICORES
    // This is the currently running processes
    unsigned int cpu_cnt = def->cpu_count;
    int *free_cpu_stack = malloc(cpu_cnt * sizeof(int));
    unsigned int *startt = malloc(cpu_cnt * sizeof(unsigned int));
    process **running_p = malloc(cpu_cnt * sizeof(process *));

    for (int i = 0; i < cpu_cnt; i++)
        running_p[i] = 0;

    // Runs indefinitely
    while (1)
    {
        if (proc_count > 0)
        {
            if (p_pointer == 0)
                p_pointer = queue;
            // For each cpu
            for (int i = 0; i < cpu_cnt; i++)
            {
                // If cpu has no proccess in it
                if (running_p[i] == 0 && proc_idle)
                {
                    printf("proc = %d | idle = %d\n", proc_count, proc_idle);
                    // gets the next process
                    while (p_pointer->proc->state != 0)
                    {
                        printf("FART\n");
                        sem_wait(&queue_s);
                        pp_pointer = p_pointer;
                        p_pointer = p_pointer->next;
                        sem_post(&queue_s);
                    }
                    running_p[i] = p_pointer->proc;
                    running_p[i]->state = 1;
                    if (globals.extra)
                        printf("[CPU%d] Is free and will get process %s \n", i, running_p[i]->name);
                    // Sets the start time
                    startt[i] = getttime();
                    // Unlocks current thread and let it run
                    sem_post(&(running_p[i]->sem));
                    if (globals.extra)
                        printf("[RR ] Process %s resumed \n", running_p[i]->name);
                    proc_idle--;
                }
                // If process has ended
                if (running_p[i] && running_p[i]->dt_dec == -1)
                {
                    if (globals.extra)
                        printf("[CPU%d] IS with the ENDED process %s\n", i, running_p[i]->name);
                    running_p[i]->state = 2;
                    // frees semaphore
                    sem_destroy(&(p_pointer->proc->sem));
                    // Removes from list
                    pp_pointer->next = p_pointer->next;
                    proc_count--;
                    running_p[i] = 0;
                }
                // Gets the time now
                unsigned int t_now = getttime();
                if (running_p[i] && t_now - startt[i] >= QUANTUM)
                {
                    printf("PASSSSOu um quantum na cpu %d\n", i);
                    // One quantum has passed and the process isnt finished
                    // Change process
                    // Pause thread
                    sem_wait(&running_p[i]->sem);
                    // Set state as waiting
                    running_p[i]->state = 0;
                    proc_idle++;
                    // gets the next process
                    while (p_pointer->proc->state != 0)
                    {
                        printf("FART2\n");
                        sem_wait(&queue_s);
                        pp_pointer = p_pointer;
                        p_pointer = p_pointer->next;
                        sem_post(&queue_s);
                    }
                    if (globals.extra)
                        printf("[RR ] CPU %d changing processes from %s to %s\n",
                               i, running_p[i]->name, p_pointer->proc->name);
                    running_p[i] = p_pointer->proc;
                    running_p[i]->state = 1;
                    // Sets the start time
                    startt[i] = getttime();
                    // Unlocks new thread and let it run
                    sem_post(&(running_p[i]->sem));
                    proc_idle--;
                }
            }
        }
        else if (def->ended)
        {
            if (globals.extra)
                printf("\n\n\nMORRIII\n\n\n");
            fflush(stdout);
            break;
        }
    }
    sem_destroy(&queue_s);
}
