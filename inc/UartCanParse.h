
#ifndef _UARTCANPARSE_H_
#define _UARTCANPARSE_H_

#include "UartCanThread.h"
#include "UartCanDecode.h"
#include "UartCanRingbuffer.h"
#include "UartCanSemaphore.h"

#include <stdio.h>

namespace uartcan{

static const int kBufferLength=2048;

class UartCanParse:public UartCanThread
{

public:
	UartCanParse();
	~UartCanParse();

	static void * 	UartCanParsefunc(void *arg);
	
	int 			Init(void);
	int 			GetSpeedParseResult(float *speed);
	void 			SetSpeedParseCallback(ParseCallback callback);
	void 			Release(void);	

	void  			PostParse(void);
	void 			PutdataToParse(unsigned char *buf,int len);
	void            WaitParse(void);
	UartCanDecode   m_decode;
private:
	
	
	UartCanRingbuffer   m_Ringbuffer;
	uartcan_semaphore_t uartcan_parse_sem;

	volatile int 	isInited;
	
};


};

#endif
