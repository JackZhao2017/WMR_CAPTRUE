#include "UartCanRingbuffer.h"

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <unistd.h>
#include <semaphore.h>
#include <pthread.h>

#define  lock pthread_mutex_lock(&ringBufferMutex);
#define  unlock pthread_mutex_unlock(&ringBufferMutex);

namespace uartcan{
UartCanRingbuffer::UartCanRingbuffer(void)
{
	pthread_mutex_init(&ringBufferMutex,NULL);
	printf("%s()\n",__func__ );
}

UartCanRingbuffer::~UartCanRingbuffer(void)
{
	pthread_mutex_destroy(&ringBufferMutex);
	printf("%s()\n",__func__);
}



int  UartCanRingbuffer::RingbufferInit(int size)
{
	if(size<=0){
		return -1;
	}


	memset(&m_ringBufferinfo,0,sizeof(RINGBUFFERINFO));

	m_ringBufferinfo.data=(unsigned char*)malloc(size);
	m_ringBufferinfo.size=size;
	m_ringBufferinfo.mark=size-1;
	return 0;
err:
	RingbufferRelease();
	return -1;
}

void UartCanRingbuffer::RingbufferRelease(void)
{
	lock;
	free(m_ringBufferinfo.data);
	m_ringBufferinfo.data=NULL;
	memset(&m_ringBufferinfo,0,sizeof(RINGBUFFERINFO));
	unlock;
}

int  UartCanRingbuffer::PutdataTobuffer(unsigned char *buf,int len)
{
	lock;
	if(m_ringBufferinfo.data==NULL){
		unlock;
		return 0;
	}
	if(len<=0){
		unlock;
		return 0;
	}

	

	if(len>m_ringBufferinfo.size - m_ringBufferinfo.num){
		printf(" len is out of range\n" );
		len=m_ringBufferinfo.size - m_ringBufferinfo.num;
	}

	if(m_ringBufferinfo.putaddr+len>m_ringBufferinfo.mark){
		memcpy(&m_ringBufferinfo.data[m_ringBufferinfo.putaddr],buf,m_ringBufferinfo.size-m_ringBufferinfo.putaddr);
		memcpy(&m_ringBufferinfo.data[0],&buf[m_ringBufferinfo.size-m_ringBufferinfo.putaddr],m_ringBufferinfo.putaddr+len-m_ringBufferinfo.size);
	}else{
		memcpy(&m_ringBufferinfo.data[m_ringBufferinfo.putaddr],buf,len);
	}

	m_ringBufferinfo.num	  +=len;
	m_ringBufferinfo.putaddr=(m_ringBufferinfo.putaddr+len)%m_ringBufferinfo.size; 
	
	unlock;
	return 1;
}
int  UartCanRingbuffer::GetbufferHeardata(unsigned char *buf,int len)
{
	lock;
	if(m_ringBufferinfo.data==NULL){
		unlock;
		return 0;
	}
	if(m_ringBufferinfo.num<len){
		len=m_ringBufferinfo.num;
	}	

	if(m_ringBufferinfo.getaddr+len>m_ringBufferinfo.mark){
		memcpy(buf,&m_ringBufferinfo.data[m_ringBufferinfo.getaddr],m_ringBufferinfo.size-m_ringBufferinfo.getaddr);
		memcpy(&buf[m_ringBufferinfo.size-m_ringBufferinfo.getaddr],&m_ringBufferinfo.data[0],m_ringBufferinfo.getaddr+len-m_ringBufferinfo.size);
	}else{
		memcpy(buf,&m_ringBufferinfo.data[m_ringBufferinfo.getaddr],len);
	}
	unlock;
	return len;
}


int  UartCanRingbuffer::AddBufferHearaddr(int len)
{

	lock;
	m_ringBufferinfo.num-=len;
	m_ringBufferinfo.getaddr=(m_ringBufferinfo.getaddr+len)%m_ringBufferinfo.size; 
	unlock;
	return 1;
}

int  UartCanRingbuffer::GetdataFrombuffer(unsigned char *buf,int len)
{
	if(m_ringBufferinfo.data==NULL)
		return 0;
	lock;
	if(len>m_ringBufferinfo.num){
		unlock;
		return 0;
	}

	if(m_ringBufferinfo.getaddr+len>m_ringBufferinfo.size){
		memcpy(buf,&m_ringBufferinfo.data[m_ringBufferinfo.getaddr],m_ringBufferinfo.size-m_ringBufferinfo.getaddr);
		memcpy(&buf[m_ringBufferinfo.size-m_ringBufferinfo.getaddr],&m_ringBufferinfo.data[0],m_ringBufferinfo.getaddr+len-m_ringBufferinfo.size);
	}else{
		memcpy(buf,&m_ringBufferinfo.data[m_ringBufferinfo.putaddr],len);
	}
	m_ringBufferinfo.num-=len;
	m_ringBufferinfo.getaddr=(m_ringBufferinfo.getaddr+len)%m_ringBufferinfo.size; 
	unlock;
	return 1;

}

int UartCanRingbuffer::CheckCharBuffer(unsigned char sync)
{
	int i=0;
	int sum=m_ringBufferinfo.num;

	for(i=0;i<sum;i++){

		if(m_ringBufferinfo.data[m_ringBufferinfo.getaddr]==sync){

			AddBufferHearaddr(1);		
			return 1;
		}

	}	
	
	return 0;
}

void UartCanRingbuffer::CleanRingbuffer(void)
{
	 m_ringBufferinfo.getaddr=0;
	 m_ringBufferinfo.putaddr=0;
	 m_ringBufferinfo.num=0;
}

};
