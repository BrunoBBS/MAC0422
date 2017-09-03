#include "ep1/process.h"

typedef struct rusage rusage;
typedef struct timeval timeval;

void process_t(void* args)
{
    // We receive a process struct from the scheduler
    process *proc = (process*) args;
    if (!proc)
        return;

    // Here we initialize some variables (time is translated to milliseconds)
    sem_t *semaphore = &(proc->sem);
    unsigned int dt_mil = proc->dt_dec*100;

    // Some variables we will need for time calculation
    rusage usage_info;
    unsigned int millis;

    do {
        // So we can block the process from the scheduler
        sem_wait(semaphore);
        sem_post(semaphore);

        // Spends some cpu time
        int j = 0;
        for (int i = 0; i < 100; i++)
            j++;

        // Calculates user land time the process thread has spent
        getrusage(RUSAGE_THREAD, &usage_info);
        timeval time_spent = usage_info.ru_utime;
        millis = (time_spent.tv_sec * 1000) +
            (time_spent.tv_usec / 1000);

        // Stops running when process has run for enough time
    } while (millis < dt_mil);

    // Signals scheduler that the process has ended running
    proc->dt_dec = -1;
}
