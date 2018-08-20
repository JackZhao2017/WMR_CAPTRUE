#ifndef _UARTCANREAD_H_
#define _UARTCANREAD_H_


#include "UartCanThread.h"
#include "UartCanParse.h"
#include <sys/epoll.h>
#include <signal.h>
#include <pthread.h>
#include <stdio.h>
namespace uartcan{



class UartCanRead
{
public:
	UartCanRead();
	~UartCanRead();
	int Init(int fd, UartCanParse *parse);
	void Release(void);
	static void *UartCanReadfunc(void *arg);
private:
	UartCanParse    *mParse;
	pthread_t 		thread_pid;
	int        		thread_exit;
	int  			isInited;

	int g_dev_fd;
	int g_tim_fd;
	int g_epfd;

	int epollfd_init(void);
	int epoll_add_fd(int fd);
	int timerfd_init();
	
};

};

#endif


