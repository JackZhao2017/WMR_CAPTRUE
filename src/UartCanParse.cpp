#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "UartCanParse.h"
#include "UartCanDecode.h"
#include "UartCanSemaphore.h"

namespace uartcan{

UartCanParse::UartCanParse()
{
	isInited=0;
	uartcan_semaphore_init(&uartcan_parse_sem,0);
	printf("%s()\n",__func__ );
}
UartCanParse::~UartCanParse()
{	
	uartcan_semaphore_destroy(&uartcan_parse_sem);
	printf("%s()\n",__func__ );
}
void UartCanParse::PutdataToParse(unsigned char *buf,int len)
{
	if(isInited==0){
		printf("%s Init failed\n",__func__ );
		return;
	}
	m_Ringbuffer.PutdataTobuffer(buf,len);
}
void UartCanParse::PostParse(void)
{
	if(isInited==0){
		printf("%s Init failed\n",__func__ );
		return;
	}
	uartcan_semaphore_post(&uartcan_parse_sem);
	return ;
}
void UartCanParse::WaitParse(void)
{
	uartcan_semaphore_wait(&uartcan_parse_sem);
}

void *UartCanParse::UartCanParsefunc(void *arg)
{
	UartCanParse* ptr = reinterpret_cast<UartCanParse*>(arg);
	unsigned char buf[kBufferLength];
	int getlen=0,i=0;
	char data[32];
	
	memset(buf,0,sizeof(buf));
	while(1)
	{
		int syncindex=0;
		unsigned char type=0;

		ptr->WaitParse();

		if(ptr->thread_exit){
			break;
		}	

		getlen=ptr->m_Ringbuffer.GetbufferHeardata(buf,kBufferLength);
		if(getlen>0){
		
			for(syncindex=0;syncindex<getlen;syncindex++)
			{   
				if(!(ptr->m_decode.DetectMsgSync(buf,syncindex))){
					continue;
				}
				 
				if(++syncindex>=getlen){
					syncindex-=1;
					break;
				}

				if(ptr->m_decode.DetectMsgType(&buf[syncindex],&type)){

					if(syncindex+type-1>=getlen){
						syncindex-=1;
						break;
					}

					if(ptr->m_decode.ParseMsg(&buf[syncindex],type)){  // if crc8detect  success syncindex  should be add type ,else  add 1
						syncindex+=type-1;
					}
				}else{
					syncindex-=1;
				}			 
		    }
		    ptr->m_Ringbuffer.AddBufferHearaddr(syncindex);
		
		}		
		if(ptr->thread_exit){
			break;
		}	  	
	}
	pthread_exit(0);
	return NULL;
}

int UartCanParse::GetSpeedParseResult(float *speed)
{
	return m_decode.GetCanSpeed(speed);
}
void UartCanParse::SetSpeedParseCallback(ParseCallback callback)
{
	return m_decode.setSpeedCallback(callback);
}


int UartCanParse::Init(void)
{
	if(isInited){
		printf("%s already\n", __func__);
		return 0;
	}
	if((m_Ringbuffer.RingbufferInit(kBufferLength))<0){
		goto err0;
	}
	if(launchThread(UartCanParsefunc,this)){
		goto err;
	}
	isInited=1;
	thread_exit=0;
	printf("%s success\n",__func__ );
	return 0;
err:
	m_Ringbuffer.RingbufferRelease();
err0:
	isInited=0;
	printf("%s failed\n",__func__ );
	return -1;
}

void UartCanParse::Release(void)
{
	if(isInited==0){
		return;
	}
	isInited=0;
	thread_exit=1;
	uartcan_semaphore_post(&uartcan_parse_sem);
	exitThread();
	m_Ringbuffer.RingbufferRelease();
	printf("%s UartCanParse\n",__func__ );
}

};