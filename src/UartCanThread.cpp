#include "UartCanThread.h"
#include <stdio.h>
#include <sys/prctl.h>
#include <semaphore.h>
#include <pthread.h>

namespace uartcan{


UartCanThread::UartCanThread()
{
    thread_exit=0;
    uartcan_sem_init();
    uartcan_lock_init();
    printf("UartCanThread()\n");
}
UartCanThread::~UartCanThread()
{
    uartcan_sem_release();
    uartcan_lock_release();
    printf("~UartCanThread()\n");
}

int UartCanThread::launchThread(void *(*start_routine)(void *),void* user_data)
{
    /* launch the thread */
    int ret=pthread_create(&thread_pid,
                   NULL,
                   start_routine,
                   user_data);
    return ret;
}

int UartCanThread::setThreadName(const char* name)
{
    prctl(PR_SET_NAME, (unsigned long)name, 0, 0, 0);
    return 0;
}

void UartCanThread::uartcan_sem_wait(void)
{
    sem_wait(&thread_sem);
}

void UartCanThread::uartcan_sem_post(void)
{
    sem_post(&thread_sem);
}

void UartCanThread::uartcan_sem_init(void)
{
    sem_init(&thread_sem,0,0);
}
void UartCanThread::uartcan_sem_release(void)
{
    sem_destroy(&thread_sem);
}
void  UartCanThread::uartcan_lock_init(void)
{
    pthread_mutex_init(&thread_mutex,NULL);
}
void  UartCanThread::uartcan_lock_release(void)
{
    pthread_mutex_destroy(&thread_mutex);
}
void  UartCanThread::uartcan_lock(void)
{
    pthread_mutex_lock(&thread_mutex);
}

void  UartCanThread::uartcan_unlock(void)
{
    pthread_mutex_unlock(&thread_mutex);
}


int UartCanThread::exitThread(void)
{
    int rc = 0; 
    if(pthread_join(thread_pid, NULL) != 0) {
        
    }
    return rc;
}

};
