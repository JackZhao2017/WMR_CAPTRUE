#ifndef _UARTCANSEND_H_
#define _UARTCANSEND_H_

#include "UartCanThread.h"
#include "UartCanEncode.h"
#include "UartCanCrc8.h"
#include "UartCanMWR.h"

namespace uartcan{

class UartCanSend:public UartCanThread,public UartCanEncode
{
public:
	UartCanSend();
	~UartCanSend();
	ADAS_INFO       Adas_Info;
	
	void  			StartCanSendOne(const ADAS_INFO *adasinfo);
	int  			Init(int fd);
	void			Release(void);
	void 			WaitCanSend(ADAS_INFO *adasinfo);
    static void *	UartCanSendfunc(void *arg);
	void 			CanSendSpeed(int speed);
	void 			WaitSendSpeed();
private:	
	int 			dev_fd;
	volatile int 	isInited;
	volatile int 	isReady;
};

};

#endif