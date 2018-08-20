#ifndef _UARTCANENCODE_H_
#define _UARTCANENCODE_H_



#include "UartCanCrc8.h"

namespace uartcan{


#define   CANTXMSG      	0x11 
#define   MSY_SYNC    		0x55
#define   CANTXMSGSIZE      CANTXMSG+1      



#define   DISABLE           0x00
#define   SYS_ENABLE     	0x01
#define   SYS_ERR           0x02

#define   LDW_ENABLE        0x04
#define   LDW_ERR           0x08

#define   FCW_ENABLE        0x10
#define   FCW_ERR           0x20

#define   LAMP_EN           0x40
#define   LAMP_ERR          0x80


#define   LDW_L_NORMAL    	0x00
#define   LDW_L_WARN      	0x01
#define   LDW_L_ALARM     	0x02

#define   LDW_R_NORMAL    	0x00
#define   LDW_R_WARN      	0x04
#define   LDW_R_ALARM     	0x08

#define   FCW_NORMAL      	0x00
#define   FCW_LEVEL_1     	0x10
#define   FCW_LEVEL_2     	0x20
#define   FCW_LEVEL_3     	0x30


#define   LDW_FACTOR        0.03
#define   FCW_TTC_FACTOR    0.05
#define   FCW_DIS_FACTOR    0.5



typedef struct {
	  unsigned char  TYPE;
	  unsigned char  COUNTER;
	  unsigned char  IDE_RTR;
	  unsigned char  DLC;
	  unsigned int   ID;  
	  unsigned char  Data[8];
}CanTxMsg;


typedef struct _uarttxMsg
{		
	  unsigned char sys_st;;
	  unsigned char ldw_st;
	  unsigned char fcw_st;
	  unsigned char lamp_st;

	  unsigned char ldw_l; 	
	  unsigned char ldw_r;
	  unsigned char fcw;


	  unsigned char ldw_left_info;
	  unsigned char ldw_right_info;
	  unsigned char fcw_dis;
	  unsigned char fcw_speed;
	  unsigned char fcw_Ttc;

	  unsigned char dfw_state;
	  unsigned char pd_state;

}ADASMSG;


typedef struct _ADAS_INFO{
	float 		  ldw_dis; //偏离距离，小于0是内侧，大于0是外侧
	float 		  fcw_dis;//前车距离
	float 		  fcw_speed;//相对速度
	float 		  fcw_ttc; //碰撞时间
	unsigned char system_state;//0 不可用，1 运行
	unsigned char ldw_enable;//0 不可用，1 运行
	unsigned char ldw_state;//0 正常，1 左偏离，2 右偏离
	unsigned char fcw_enable;//0 不可用，1运行
	unsigned char fcw_state;// 0 正常，1 一级预警，2 二级预警，3 三级预警
	unsigned char dfw_state;
	unsigned char pd_state;
}ADAS_INFO;

class UartCanEncode:public UartCanCrc8
{

public:
	  UartCanEncode();
	  virtual ~UartCanEncode();

	  ADASMSG 		adasMsg;
	  CanTxMsg		canMsg;
	  unsigned char mStream[CANTXMSGSIZE*2];

	  int  convertuartMsg2canMsg(ADASMSG *adasMsg ,CanTxMsg *canMsg);
	  int  convertcanMsg2stream(CanTxMsg *canMsg,unsigned int canid,unsigned char *buf);
	  int  convertadasInfo2msg(ADAS_INFO *adasInfo,ADASMSG *adasMsg);
	  int  convertadasInfo2uartstream(ADAS_INFO *adascan_info,unsigned int canid,unsigned char *buf);

private:
	  int  count;
      int  msgcount;
      long long delay_time_right;
      long long delay_time_left;
	  int  msgcounter(void);
	  long long currenttime();
};

};


#endif