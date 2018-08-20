#ifndef _UARTCANRECEIVE_H
#define _UARTCANRECEIVE_H

#include "UartCanParse.h"
#include "UartCanDecode.h"
#include "UartCanRead.h"
#include "UartCanMWR.h"
namespace uartcan{

class UartCanReceive
{
public:
	UartCanReceive();
	~UartCanReceive();


	int  Init(int fd);
	int  GetReceiveSpeed(float *speed);
	void Release(void);
	void SetReceiveSpeedCallback(ParseCallback callback);
	void SetParseMWRCallback(MWRCallback mwrcallback);
private:
	UartCanParse  uartParse;
	UartCanRead   uartRead;
	volatile int  isInited;

};

};


#endif