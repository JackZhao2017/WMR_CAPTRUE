#include "UartCanEncode.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

namespace uartcan{

	
UartCanEncode::UartCanEncode()
{
	printf("UartCanEncode()\n" );
	msgcount=0;
	delay_time_right=0;
	delay_time_left=0;
}

UartCanEncode::~UartCanEncode()
{
	printf("~UartCanEncode()\n" );
	msgcount=0;
}

long long UartCanEncode::currenttime(){
    struct timeval now;
    gettimeofday(&now, NULL);
    long long when = now.tv_sec * 1000LL + now.tv_usec / 1000;
    return when;
}


int UartCanEncode:: msgcounter(void)
{
	int  val=msgcount;
    msgcount=(msgcount+1)&0xff;
    return val;
}

int UartCanEncode::convertuartMsg2canMsg(ADASMSG *adasMsg ,CanTxMsg *canMsg)
{
	memset(canMsg,0,sizeof(CanTxMsg));

	canMsg->Data[0]=adasMsg->sys_st|adasMsg->ldw_st|adasMsg->fcw_st|adasMsg->lamp_st;
	canMsg->Data[1]=adasMsg->ldw_l|adasMsg->ldw_r|adasMsg->fcw;
	canMsg->Data[2]=adasMsg->ldw_left_info;
	canMsg->Data[3]=adasMsg->ldw_right_info;
	canMsg->Data[4]=adasMsg->fcw_dis;
	canMsg->Data[5]=adasMsg->fcw_speed;
	canMsg->Data[6]=adasMsg->fcw_Ttc;
	canMsg->Data[7]=adasMsg->dfw_state|adasMsg->pd_state;
	return 1;
}
int UartCanEncode::convertcanMsg2stream(CanTxMsg *canMsg ,unsigned int canid,unsigned char *buf)
{
	buf[0]			=MSY_SYNC;
	canMsg->TYPE	=CANTXMSG;
	canMsg->COUNTER	=msgcounter();
	canMsg->ID 		=canid;
	canMsg->IDE_RTR	=0x10;
	canMsg->DLC		=8;
	memcpy(&buf[1],canMsg,sizeof(CanTxMsg));
	buf[CANTXMSG]=crc8creator(buf,1,sizeof(CanTxMsg));
	return  (sizeof(CanTxMsg)+2);
}


int UartCanEncode::convertadasInfo2msg(ADAS_INFO *adascan_info,ADASMSG *adasMsg)
{
		if(adascan_info->system_state){
				adasMsg->sys_st=SYS_ENABLE;
		}else{
				adasMsg->sys_st=DISABLE;
		}

		if(adascan_info->ldw_enable){
				adasMsg->ldw_st=LDW_ENABLE;
		}else{
				adasMsg->ldw_st=DISABLE;
		}

		if(adascan_info->fcw_enable){
				adasMsg->fcw_st=FCW_ENABLE;
		}else{
				adasMsg->fcw_st=DISABLE;
		}	

		if((adascan_info->ldw_state==1)||(delay_time_left)){

				adasMsg->ldw_r=DISABLE;
				adasMsg->ldw_right_info=DISABLE;
				adasMsg->ldw_left_info=((int )(adascan_info->ldw_dis/LDW_FACTOR))&0xff;
				if(adascan_info->ldw_dis>0){
					adasMsg->ldw_l=LDW_L_WARN;	
				}else{
					adasMsg->ldw_l=LDW_L_ALARM;
				}
				if(delay_time_left==0){
					delay_time_left=currenttime();
				}else if(currenttime()-delay_time_left>1000){
					delay_time_left=0;
				}

		}else if((adascan_info->ldw_state==2)||(delay_time_right)){

				adasMsg->ldw_l=DISABLE;
				adasMsg->ldw_left_info=DISABLE;
				adasMsg->ldw_right_info=((int )(adascan_info->ldw_dis/LDW_FACTOR))&0xff;
				if(adascan_info->ldw_dis>0){
					adasMsg->ldw_r=LDW_R_WARN;	
				}else{
					adasMsg->ldw_r=LDW_R_ALARM;
				}
				if(delay_time_right==0){
					delay_time_right=currenttime();
				}else if(currenttime()-delay_time_right>1000){
					delay_time_right=0;
				}
		}else{
				adasMsg->ldw_l=LDW_L_NORMAL;
				adasMsg->ldw_r=LDW_R_NORMAL;
		}	

		adasMsg->fcw=(adascan_info->fcw_state&0x0f)<<4;

		adasMsg->fcw_dis   =((unsigned char)((adascan_info->fcw_dis)/FCW_DIS_FACTOR))&0xff;
		adasMsg->fcw_speed =((unsigned char)adascan_info->fcw_speed)&0xff;
		adasMsg->fcw_Ttc   =(unsigned char)(adascan_info->fcw_ttc/FCW_TTC_FACTOR);

		adasMsg->dfw_state =(adascan_info->dfw_state&0x0f)<<4;
		adasMsg->pd_state  =(adascan_info->pd_state&0xf);

		return 1;
}

int UartCanEncode::convertadasInfo2uartstream(ADAS_INFO *adascan_info,unsigned int canid,unsigned char *buf)
{
		convertadasInfo2msg(adascan_info,&adasMsg);
		convertuartMsg2canMsg(&adasMsg ,&canMsg);
		convertcanMsg2stream(&canMsg ,canid,buf);
		return 1;
}

};