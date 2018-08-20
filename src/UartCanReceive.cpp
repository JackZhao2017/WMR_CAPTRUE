#include "UartCanParse.h"
#include "UartCanDecode.h"
#include "UartCanReceive.h"
#include "UartCanRingbuffer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

namespace uartcan{


UartCanReceive::UartCanReceive()
{
	isInited=0;
	printf("%s\n",__func__ );
}

UartCanReceive::~UartCanReceive()
{
	printf("%s\n",__func__ );
}

int UartCanReceive::Init(int fd)
{
	if(fd<=0){
		return -1;
	}
	if(uartParse.Init()){
		goto err0;
	}
	if(uartRead.Init(fd,&uartParse)){
		goto err;
	}
	isInited=1;
	printf("%s success\n", __func__);
	return 0;

err:
	uartParse.Release();
err0:
	printf("%s faild\n", __func__);
	return -1;
}
int UartCanReceive::GetReceiveSpeed(float *speed)
{
	return uartParse.GetSpeedParseResult(speed);
}
void UartCanReceive::SetReceiveSpeedCallback(ParseCallback callback)
{
	uartParse.SetSpeedParseCallback(callback);
}

void UartCanReceive::SetParseMWRCallback(MWRCallback mwrcallback)
{
	uartParse.m_decode.mMillimeter.SetMWRCallback(mwrcallback);
	return;
}

void UartCanReceive::Release(void)
{
	printf("UartCanReceive %s\n",__func__ );
	uartRead.Release();
	uartParse.Release();
	isInited = 0;
}

};