
#include "UartCanMWR.h"
#include "UartCanEncode.h"
#include "UartCanCrc8.h"
#include "sort_data_from_radar.h"
#include "param_for_data_sort.h"
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <cmath>

namespace uartcan{

UartCanMWR::UartCanMWR()
{
    mIndexSum=0;
    mMwrCallback=NULL;
    fp=NULL;
    fp=fopen("/media/card/wmrlog.txt","w+");
    memset(&mOrgRadarRst,0,sizeof(radarRst));
    memset(&mNewRadarRst,0,sizeof(radarRst));
    printf("%s\n",__func__ );
}
UartCanMWR::~UartCanMWR()
{
    fclose(fp);
    printf("%s\n",__func__ );
}
float UartCanMWR::GetObjectDistanceX(unsigned char *CanByteData)
{
    int disX;
    float distanceX;
    disX = (CanByteData[1]<<5)+((CanByteData[2]&0xf8)>>3);
    distanceX =disX*0.2-500;
   
    return distanceX;	
}
float UartCanMWR::GetObjectDistanceY(unsigned char *CanByteData)
{
    int disY;
    float distanceY;


    disY = ((CanByteData[2]&0x7)<<8)+CanByteData[3];
    distanceY = disY*0.2-204.6;

    return distanceY;    
}

float UartCanMWR::GetObjectDistance(float distanceX ,float distanceY)
{
    
    float distance = sqrt(distanceX*distanceX+distanceY*distanceY);
    return distance;    
}
float UartCanMWR::GetObjectSpeed(unsigned char *CanByteData)
{
    int rel_speedX,rel_speedY;
    float relative_speed, relative_speedX,relative_speedY;
    rel_speedX = (CanByteData[4]<<2)+(CanByteData[5]>>6);
    relative_speedX =  rel_speedX*0.25-128;

    rel_speedY = (CanByteData[5]<<3)+(CanByteData[6]>>5);
    relative_speedY =  rel_speedY*0.25-64;

    relative_speed = sqrt(relative_speedX*relative_speedX+relative_speedY*relative_speedY);
    //printf("%s %0.2f\n",__func__, relative_speed);
    return relative_speedX;
}
float UartCanMWR::GetObjectAngle(float distanceX,float distanceY)
{
 
    float Angle = atan(distanceY/distanceX);
    return Angle;
}
int  UartCanMWR::GetObjectProp(unsigned char *CanByteData)
{
    int prop =CanByteData[6]&0x07;

    return prop;
}
// 0x0: moving
// 0x1: stationary
// 0x2: oncoming
// 0x3: stationary
// candidate
// 0x4: unknown
// 0x5: crossing
// stationary
// 0x6: crossing
// moving
// 0x7: stopped
string UartCanMWR:: ObjectPropName(int prop)
{
    switch(prop){
        case 0x0:
            return "moving";
        case 0x1:
            return "stationary";
        case 0x2:
            return "oncoming";
        case 0x3:
            return "stationary candidate";
        case 0x4:
            return "unknown";
        case 0x5:
            return "crossing stationary";
        case 0x6:
            return "crossing moving";
        case 0x7:
            return "stopped";
    }
}
void UartCanMWR::ParseTargetObject(unsigned int ID,unsigned char *CanByteData)
{
    int i=0;

    if(ID==0x0a){ 
        mIndexSum=0;     
        memset(&mOrgRadarRst,0,sizeof(mOrgRadarRst));
        mIndexSum=CanByteData[0];
        if(mIndexSum>MAX_RADAR_DATA_NUM){
            mIndexSum=MAX_RADAR_DATA_NUM;
        }
        g_speed = CanByteData[4];
        if(mIndexSum==0){
            memset(&mNewRadarRst,0,sizeof(radarRst));
            if(mMwrCallback!=NULL){
                mMwrCallback(&mOrgRadarRst,&mNewRadarRst,g_speed);
            }           
        }
    }else if(ID==0xb){

        char  buf[128];   
        memset(buf,0,sizeof(buf));    
        sprintf(buf,"ID %d 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x \n",
                    ID,CanByteData[0],CanByteData[1],CanByteData[2],CanByteData[3],CanByteData[4],CanByteData[5],CanByteData[6],CanByteData[7]);
        fwrite(buf,1,strlen(buf),fp); 
    
        float disX = GetObjectDistanceX(CanByteData);
        float disY = GetObjectDistanceY(CanByteData);
        float dis  = GetObjectDistance(disX,disY);
        float angle= GetObjectAngle(disX,disY);
        float speed= GetObjectSpeed(CanByteData);
        int   prop = GetObjectProp(CanByteData);

        if(mOrgRadarRst.cnt>MAX_RADAR_DATA_NUM-1){
            printf("object is full\n");
            return;
        }
            
        mOrgRadarRst.obj[mOrgRadarRst.cnt].idx=mOrgRadarRst.cnt;
        mOrgRadarRst.obj[mOrgRadarRst.cnt].flag=prop;
        mOrgRadarRst.obj[mOrgRadarRst.cnt].disx=disX;
        mOrgRadarRst.obj[mOrgRadarRst.cnt].disy=disY;
        mOrgRadarRst.obj[mOrgRadarRst.cnt].dis=dis;
        mOrgRadarRst.obj[mOrgRadarRst.cnt].vrel=speed;
        mOrgRadarRst.obj[mOrgRadarRst.cnt].ang=angle;
        mOrgRadarRst.cnt+=1;


        if(mOrgRadarRst.cnt>=mIndexSum){ 
            memset(&mNewRadarRst,0,sizeof(radarRst));
            sort_data_from_radar(&mNewRadarRst,&mOrgRadarRst,mOrgRadarRst.cnt); 
            for(i=0;i<mOrgRadarRst.cnt;i++){
                memset(buf,0,sizeof(buf)); 
                sprintf(buf, "[ORG] index = %d disx = %.1f disy = %.1f dis = %.1f speed = %.1f ang =%.1f %s \n",
                        mOrgRadarRst.obj[i].idx,mOrgRadarRst.obj[i].disx,mOrgRadarRst.obj[i].disy,mOrgRadarRst.obj[i].dis,mOrgRadarRst.obj[i].vrel, mOrgRadarRst.obj[i].ang,ObjectPropName(mOrgRadarRst.obj[i].flag).c_str());
                fwrite(buf,1,strlen(buf),fp); 
            }  
            for(i=0;i<mNewRadarRst.cnt;i++){
                memset(buf,0,sizeof(buf)); 
                sprintf(buf, "[NEW] index = %d disx = %.1f disy = %.1f dis = %.1f speed = %.1f ang =%.1f %s \n",
                        mNewRadarRst.obj[i].idx,mNewRadarRst.obj[i].disx,mNewRadarRst.obj[i].disy,mNewRadarRst.obj[i].dis,mNewRadarRst.obj[i].vrel, mNewRadarRst.obj[i].ang,ObjectPropName(mNewRadarRst.obj[i].flag).c_str());
                fwrite(buf,1,strlen(buf),fp); 
            } 
            fwrite("\n\n\n",1,strlen("\n\n\n"),fp);       
            if(mMwrCallback!=NULL){
                mMwrCallback(&mOrgRadarRst,&mNewRadarRst,g_speed);
            }
        }
    }else if(ID ==0x0d){
        printf("ID %d\n",ID );
    }else if(ID ==0x0c){
        printf("ID %d\n",ID );
    }
}


}