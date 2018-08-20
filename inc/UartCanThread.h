#ifndef _UARTCANTHREAD_H_
#define _UARTCANTHREAD_H_

#include <pthread.h>
#include <semaphore.h>
namespace uartcan{

class UartCanThread
{
public:
	UartCanThread();
	virtual ~UartCanThread();

	pthread_t 		thread_pid;
    sem_t     		thread_sem;
	pthread_mutex_t thread_mutex;
    int             thread_exit;

    int32_t launchThread(void *(*start_routine)(void *), void* user_data);
    int32_t setThreadName(const char* name);
    int32_t exitThread();

    void    uartcan_sem_wait(void);
    void    uartcan_sem_post(void);
    void    uartcan_sem_init(void);
    void    uartcan_sem_release(void);

    void    uartcan_lock_init(void);
    void    uartcan_lock(void);
    void    uartcan_unlock(void);
    void    uartcan_lock_release(void);

};

};
#endif
