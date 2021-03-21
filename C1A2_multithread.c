#include <pthread.h>
#include <stdio.h>
#include <syslog.h>

#define NUM_THREADS 128

typedef struct
{
    int threadIdx;
} threadParams_t;


pthread_t threads[NUM_THREADS]; // array of thread ID
threadParams_t threadParams[NUM_THREADS]; // array of thread params

void * counterThread(void * threadp)
{
    int sum = 0;
    threadParams_t * threadParams = (threadParams_t *)threadp;

    for(int i=1; i < (threadParams->threadIdx)+1; i++)
        sum=sum+i;
 
    printf("[COURSE:1][ASSIGNMENT:2]: Thread idx=%d, sum[0...%d]=%d\n", 
    threadParams->threadIdx,
    threadParams->threadIdx, sum);
	
    syslog(LOG_DEBUG, "Thread idx=%d, sum[0...%d]=%d\n",
    threadParams->threadIdx,
    threadParams->threadIdx, sum);    
    
    return NULL;
}


int main (int argc, char *argv[])
{
    int i;
    
    openlog("[COURSE:1][ASSIGNMENT:2]", LOG_NDELAY, LOG_USER);
    
    for(i=0; i < NUM_THREADS; i++)
    {
        threadParams[i].threadIdx=i;
        pthread_create(&threads[i],   // pointer to thread descriptor
                      (void *)0,     // use default attributes
                      counterThread, // thread function entry point
                      (void *)&(threadParams[i]) // parameters to pass in
                      );

    }

    for(i=0;i<NUM_THREADS;i++)
        pthread_join(threads[i], NULL);

    return 0;
}
