#ifndef _UARTCANCODE_H_
#define _UARTCANCODE_H_


#include "UartCanRingbuffer.h"
#include "UartCanCrc8.h"
#include "UartCanMWR.h"
#include <stdint.h>

namespace uartcan{

typedef void (*ParseCallback)(float* );

typedef struct {
	uint8_t  SYNC;
	uint8_t  TYPE;
	uint8_t  COUNTER;
	uint8_t  IDE_RTR;
	uint32_t ID;
	uint8_t  DLC;  
	uint8_t  Data[8];
	uint8_t  CRC8;
}UartCanMsg;

typedef struct {
	uint8_t  SYNC;
	uint8_t  TYPE;
	uint8_t  ID;
	uint8_t  Data[8];
	uint8_t  CRC8;
}UartMwrMsg;

class UartCanDecode:public UartCanCrc8
{
	
public:
	UartCanDecode();
	~UartCanDecode();

	void  SAE_J1939_Speed(unsigned char *buf,int index);
	int   GetCanSpeed(float *speed);
	void  setSpeedValid(int val);
	void  setSpeedCallback(ParseCallback callback){mSpeedCallback=callback;}

	int   DetectMsgSync(unsigned char *buf,int index );
	int   DetectMsgType(unsigned char *buf,unsigned char *type);
	int   DetectMsgCrc8(unsigned char *data,int start,int len);
	int   ParseMsg(unsigned char *buf,int type);

	UartCanMWR  mMillimeter;
	
private:
	ParseCallback mSpeedCallback;
	
	float    	carSpeed;
	int      	isSpeedValid;
	
};

};


#endif