#define _GNU_SOURCE
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <sched.h>
#include <syslog.h>

#define NUM_THREADS 128
#define CPU_AFFINITY 3

typedef struct
{
    int threadIdx;
} threadParams_t;

pthread_t startthread;
pthread_t threads[NUM_THREADS];
threadParams_t threadParams[NUM_THREADS];

pthread_attr_t fifo_sched_attr; // thread attribute variable, such as scheduling policy or thread stack-size
struct sched_param fifo_param;  // only one parameter is supported in 'struct sche_param' : int sched_priority

void print_scheduler(void)
{
    int schedType = sched_getscheduler(getpid());

    switch(schedType)
    {
        case SCHED_FIFO:
            printf("Pthread policy is SCHED_FIFO\n");
            break;
        case SCHED_OTHER:
            printf("Pthread policy is SCHED_OTHER\n");
            break;
        case SCHED_RR:
            printf("Pthread policy is SCHED_RR\n");
            break;
        default:
            printf("Pthread policy is UNKNOWN\n");
    }
}

void set_scheduler(void)
{
    cpu_set_t cpuset;  // declare cpu_set_t variable for CPU_* macros

    printf("INITIAL "); print_scheduler();

    pthread_attr_init(&fifo_sched_attr);                                        // initialize thread attribute object
    pthread_attr_setinheritsched(&fifo_sched_attr, PTHREAD_EXPLICIT_SCHED);     // threads created using 'fifo_sched_attr' taks scheduling attributes from thread attribute object
    pthread_attr_setschedpolicy(&fifo_sched_attr, SCHED_FIFO);                  // threads created using 'fifo_sched_attr' applies real time 'SCHED_FIFO' policy
    
    CPU_ZERO(&cpuset);                                                          // initialize CPU set to be an empty set                                                                
    CPU_SET(CPU_AFFINITY, &cpuset);                                             // add CPU number 3 (CPU affinity) to CPU set
    pthread_attr_setaffinity_np(&fifo_sched_attr, sizeof(cpu_set_t), &cpuset);  // set CPU affinity attribute to 'fifo_sched_attr'

    fifo_param.sched_priority = sched_get_priority_max(SCHED_FIFO);             // set the maximum priority value of SCHED_FIFO to scheduler parameter
    pthread_attr_setschedparam(&fifo_sched_attr, &fifo_param);                  // bind schedule parameter to schedule attribute

    if((sched_setscheduler(getpid(), SCHED_FIFO, &fifo_param)) < 0)             // set both the scheduling policy and parameters for the thread whose ID returned by getpid()
        perror("sched_setscheduler");

    printf("ADJUSTED "); print_scheduler();
}

void *counterThread(void *threadp)
{
    int sum=0, i;
    threadParams_t *threadParams = (threadParams_t *)threadp;

    for(i=1; i < (threadParams->threadIdx)+1; i++)
        sum=sum+i;
    
    printf("Thread idx=%d, sum[0...%d]=%d, Running on core : %d\n", 
           threadParams->threadIdx,
           threadParams->threadIdx,
           sum,
           sched_getcpu()
          );
    
    syslog(LOG_DEBUG, "Thread idx=%d, sum[0...%d]=%d, Running on core : %d\n",
           threadParams->threadIdx,
           threadParams->threadIdx,
           sum,
           sched_getcpu()
          ); 
        
    return NULL;
}

void *starterThread(void *threadp)
{
    int i=0;

    printf("starter thread running on core : %d\n", sched_getcpu());

    for(i=0; i < NUM_THREADS; i++)
    {
        threadParams[i].threadIdx=i;

        pthread_create(&threads[i],              // pointer to thread descriptor
                      &fifo_sched_attr,          // use FIFO RT max priority attributes
                      counterThread,             // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                      );

    }

    for(i=0;i<NUM_THREADS;i++)
        pthread_join(threads[i], NULL);
    
    return NULL;
}

int main(int argc, char * argv[])
{
    set_scheduler();
    
    openlog("[COURSE:1][ASSIGNMENT:3]", LOG_NDELAY, LOG_USER);
    
    pthread_create(&startthread,         // pointer to thread descriptor
                  &fifo_sched_attr,      // use FIFO RT max priority attributes
                  starterThread,         // thread function entry point
                  (void *)0              // parameters to pass in
                  );

    pthread_join(startthread, NULL);

    printf("\nTEST COMPLETE\n");    
	return 0;
}
