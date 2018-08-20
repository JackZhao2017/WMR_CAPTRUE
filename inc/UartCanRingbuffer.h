#ifndef _UARTCANRINGBUFFER_H_
#define _UARTCANRINGBUFFER_H_


#include <unistd.h>
#include <stdio.h>
#include <semaphore.h>
#include <pthread.h>
#include <string.h>


namespace uartcan{

typedef struct _uartcanringbuffer
{
	int   			putaddr; 		//put first data index;
	int   			getaddr; 		//get first data index;
	int   			size;	 		//buffer size; 
	int         	mark;
	int             num;
	unsigned char   *data;
}RINGBUFFERINFO;


typedef struct {
	  unsigned char  TYPE;
	  unsigned char  COUNTER;
	  unsigned char  IDE_RTR;
	  unsigned char  ID[4]; 
	  unsigned char  DLC;	   
	  unsigned char  Data[8];
}CanRxMsg;


class UartCanRingbuffer
{

public:
	UartCanRingbuffer();
	virtual ~UartCanRingbuffer();

	int 		RingbufferInit(int size);
	void		CleanRingbuffer(void);
	void 		RingbufferRelease(void);

	int 		GetdataFrombuffer(unsigned char *buf,int len);
	int         GetbufferHeardata(unsigned char *buf,int len);
	int         AddBufferHearaddr(int len);
	int 		PutdataTobuffer(unsigned char *buf,int len);
	int         CheckCharBuffer(unsigned char sync);

private:
	RINGBUFFERINFO 		m_ringBufferinfo;
	pthread_mutex_t 	ringBufferMutex;
};


};
#endif