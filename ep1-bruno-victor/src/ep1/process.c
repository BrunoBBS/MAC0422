#include "ep1/process.h"

void* process_t(void* args)
{
    // We receive a process struct from the scheduler
    process *proc = (process*) args;
    
    if (globals.extra)
        printf("[PROC] Process %s started!\n", proc->name);
    
    if (!proc)
        return 0;

    // Here we initialize some variables (time is translated to milliseconds)
    sem_t *semaphore = &(proc->sem);
    unsigned int dt_mil = proc->dt_dec*100;

    // Some variables we will need for time calculation
    unsigned int millis;

    do {
        // So we can block the process from the scheduler
        sem_wait(semaphore);
        sem_post(semaphore);

        // Spends some cpu time
        int j = 0;
        for (int i = 0; i < 100; i++)
            j++;
        printf("Sou um processo q roda!\n");

        // Calculates time the process thread has spent
        millis = getttime();

        // Stops running when process has run for enough time
    } while (millis < dt_mil);

    // Signals scheduler that the process has ended running
    proc->dt_dec = -1;
   
    if (globals.extra)
        printf("[PROC] Process %s ended!\n", proc->name);
    return 0;
}
