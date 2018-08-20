#ifndef _UARTCANMWR_H_
#define _UARTCANMWR_H_

#include "UartCanEncode.h"
#include "sort_data_from_radar.h"
#include "param_for_data_sort.h"
#include <stdio.h>
#include <string>
using namespace std;
namespace uartcan{


#define CmdNORMAL 0
#define CmdAIMING 1
#define CmdAIMING_CLEAR 2

typedef void (*MWRCallback)(const radarRst *orgRadar ,const radarRst *newRadar,unsigned char speed);

static const  int kIDSum=190;

class UartCanMWR:public UartCanCrc8
{
public:
	UartCanMWR();
	~UartCanMWR();
	
	float GetObjectDistanceX(unsigned char *CanByteData);
	float GetObjectDistanceY(unsigned char *CanByteData);
	float GetObjectDistance(float distanceX ,float distanceY);
	float GetObjectSpeed(unsigned char *CanByteData);	
	float GetObjectAngle(float distanceX,float distanceY);
	int   GetObjectProp(unsigned char *CanByteData);
	string  ObjectPropName(int prop);
	void  ParseTargetObject(unsigned int ID,unsigned char *CanByteData);
	void  SetMWRCallback(MWRCallback mwrcallback){mMwrCallback=mwrcallback;}
private:
	radarRst 	  mOrgRadarRst;
	radarRst 	  mNewRadarRst;
	unsigned char g_speed;

	int mIndexSum;
	FILE *fp;
	MWRCallback   mMwrCallback;

};


}
#endif