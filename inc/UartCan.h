#ifndef _UARTCAN_H_
#define _UARTCAN_H_

#include <termios.h>
#include <unistd.h>

#include "UartCanReceive.h"
#include "UartCanSend.h"
#include "UartCanMWR.h"

namespace uartcan{


class UartCan
{

public:
	UartCan();
	~UartCan();

	int  UartCanInit(const char *dev,int bandrate, bool sendData = false);

	void UartCanRelease(void);

	void PutAdasinfoToUartCan(const ADAS_INFO *adasInfo);

	void GetUartCanSpeedresult(float *speed);	

	void setParseCallback(ParseCallback callback);

	void SendCarSpeed(int speed);

	void SetMWRCallback(MWRCallback mwrcallback);
	
private:
	bool   			mSendData;
	volatile int 	m_isInited;
	int  	 		m_devfd;
	int      		devInit(const char *dev,int bandrate);
	void     		devRelease(void);	
	UartCanSend 	m_cansend; 
	UartCanReceive  m_canreceive;	
	speed_t  		baudrate_map(unsigned long b);
};

};


#endif