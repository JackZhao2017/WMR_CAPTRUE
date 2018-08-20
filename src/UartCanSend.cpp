#include "UartCanSend.h"
#include "UartCanThread.h"
#include "UartCanEncode.h"

#include "UartCanMWR.h"

#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>

#define   ADASCANID    0x18FB05E8

namespace uartcan{


UartCanSend::UartCanSend()
{	
	dev_fd=-1;
	isInited=0;
	isReady=1;
	printf("UartCanSend()\n" );
}

UartCanSend::~UartCanSend()
{
	printf("~UartCanSend()\n" );
}

void UartCanSend::StartCanSendOne(const ADAS_INFO *adasinfo)
{
	uartcan_lock();
	memcpy(&Adas_Info,adasinfo,sizeof(ADAS_INFO));
	uartcan_unlock();
	uartcan_sem_post();
}

void UartCanSend::WaitCanSend(ADAS_INFO *adasinfo)
{
	uartcan_sem_wait();
	if(!isReady)
		return ;
	isReady=0;
	uartcan_lock();
	memcpy(adasinfo,&Adas_Info,sizeof(ADAS_INFO));
	uartcan_unlock();
}
void UartCanSend::CanSendSpeed(int speed)
{
	uartcan_sem_post();
}
void UartCanSend::WaitSendSpeed()
{
	uartcan_sem_wait();
}


void *UartCanSend::UartCanSendfunc(void *arg)
{	
	int i=0;
	UartCanSend* ptr = reinterpret_cast<UartCanSend*>(arg);	
	while(1)
	{
		ptr->WaitSendSpeed();
		if(ptr->thread_exit){
			break;
		}	

		


		ptr->isReady=1;
		
		if(ptr->thread_exit){
			break;
		}
	}	
	pthread_exit(NULL);
	return NULL;
}

int  UartCanSend::Init(int fd)
{
	int ret = 0;
	if(fd<0){
		return -1;
	}
	dev_fd=fd;
	thread_exit=0;
	
	ret=launchThread(UartCanSendfunc,this);
	if(ret==0){
		isInited=1;
	}
	return (ret);
}
void UartCanSend::Release(void)
{
	if(isInited==0){
		return;
	}
	thread_exit=1;
	uartcan_sem_post();
	exitThread();
	printf("%s UartCanSend\n",__func__ );
}

};

