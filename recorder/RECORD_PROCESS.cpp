#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include "RECORD_PROCESS.h"
#include "Encoder.h"
#include "printlog.h"
#include "sort_data_from_radar.h"
#include "param_for_data_sort.h"


#include "Decoder.h"
#include "MWR_Recorder.h"

static char g_record_path[128];

static long long currenttimeSec(){
	struct timeval now;
	gettimeofday(&now, NULL);
	long long when = now.tv_sec ;
	return when;
}

static void gettimer(char *t)
{
	long long time = currenttimeSec();
	//int sec=g_time/1000%60;
	int fn_min =time/60%60;
	int fn_hour=time/60/60%24;
	sprintf(t,"%02d%02d",fn_hour,fn_min);
}

static void getEncoderName(const char*path ,char *fn)
{
	char timer[56];
	memset(timer,0,sizeof(timer));
	gettimer(timer);
	sprintf(fn,"%s/%s.h264",path,timer);
	init_msg("%s\n",fn);
}
static void getWmrFileName(const char*path ,char *fn)
{
	char timer[56];
	memset(timer,0,sizeof(timer));
	gettimer(timer);
	sprintf(fn,"%s/%s.bin",path,timer);
	init_msg("%s\n",fn);
}	
void RECORD_Param(const char *path ){

	memset(g_record_path,0,sizeof(g_record_path));
	strcpy(g_record_path,path);
}

static EncoderHandle Encoder_handler=NULL;
static int isRestartEncode=0;
static long long st_record=0;
static int g_Encoder_count=0;

int  RECORD_PROCESS(unsigned char *buf,unsigned int len,int width ,int height,int fmt,radarRst *radar,float speed)
{
   
    int isRecord=0,ret=0,retval=0;
    long long cur_record=0;
    FILE *fp=NULL;
    if(!isRestartEncode){
		char fn[128];           
		memset(fn,0,sizeof(fn));
		getEncoderName(g_record_path,fn);  
		Encoder_handler=Encoder_open("hardh264v2",fn,width,height,15,fmt);
		if(Encoder_handler==NULL){
			err_msg("%s Encoder_handler creat failed\n",__func__);
			return -1;
		} 
		memset(fn,0,sizeof(fn));
		getWmrFileName(g_record_path,fn);
		MWR_RecorderInit(fn);

		st_record=currenttimeSec();       
		isRestartEncode=1;
		retval=1;
    }

    if(isRestartEncode){

		ret=Encoder_write(Encoder_handler, buf, len);
		if(ret<0){
			err_msg("%s encoder h264 write  faild\n",__func__);
			isRecord=1;
			retval=-1;
		}
		MWR_RecorderWrite(radar,speed);
		cur_record=currenttimeSec();
		++g_Encoder_count;
		if(cur_record-st_record>180){
			isRecord=1;
		}

    }

    if(isRecord){
		info_msg("<%s>every %lld seconds write %d frame\n",__func__,(cur_record-st_record), g_Encoder_count);
		g_Encoder_count=0;
		Encoder_close(Encoder_handler);
		MWR_RecorderRelease();
		isRestartEncode=0;
		retval=0;
    }
    
    return retval;
}



