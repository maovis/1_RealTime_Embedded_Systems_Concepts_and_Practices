#include <pthread.h>
#include <stdio.h>
#include <syslog.h>


void * threadFunc(void * ptrThread)
{
	printf("Hello World from Thread!\n");
	syslog(LOG_DEBUG, "Hello World from Thread!\n");
	return NULL;
}

int main (int argc, char *argv[])
{
	printf("Hello World from Main!\n");
    openlog("[COURSE:1][ASSIGNMENT:1]", LOG_NDELAY, LOG_USER);
    syslog(LOG_DEBUG, "Hello World from Main!\n");
   
    pthread_t threadId; // Thread ID
    int err = pthread_create(&threadId, NULL, &threadFunc, NULL); // create a thread calling threadFunc
    if (err) printf("Thread creation failed\n"); // check if thread has been created successfully
   
    err = pthread_join(threadId, NULL);
    if (err) printf("Thread joind failed\n"); // check if thread has joined successfully
    
    return 0;
}
