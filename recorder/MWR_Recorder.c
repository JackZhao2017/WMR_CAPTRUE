#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sort_data_from_radar.h"
#include "param_for_data_sort.h"


#include "MWR_Recorder.h"

static FILE *g_fp=NULL;


int MWR_RecorderInit(const char* path)
{
	g_fp=fopen(path,"rb");
	if(g_fp==NULL){
		printf("open faild path %s \n",path);
		return -1;
	}
	return 0;
}
int MWR_RecorderWrite(radarRst *radar,float speed)
{
  	if(g_fp==NULL)
    	return -1;
	RecordRst recordrst;
	if(radar->cnt>RECORDNUM){
		recordrst.cnt=RECORDNUM;
	}else{
		recordrst.cnt=radar->cnt;
	}
	recordrst.speed=speed;

	memcpy(recordrst.obj,radar->obj,sizeof(radarObj)*RECORDNUM);

	int ret=fwrite(&recordrst,1,sizeof(RecordRst),g_fp);
	if(ret<sizeof(RecordRst))
		return -1;
	return 0;
}

int MWR_RecorderRead(RecordRst *radar)
{
	int ret=0;
  	if(g_fp==NULL){
  		printf(" MWR_RecorderRead file null \n");
    	return -1;
  	}
	ret=fread(radar,1,sizeof(RecordRst),g_fp);
	if(ret<sizeof(RecordRst)){
		printf(" MWR_RecorderRead file num %d %d  \n",ret,sizeof(RecordRst));
		return -1;
	}
	return 0;
}
void MWR_RecorderRelease(void)
{
	fclose(g_fp);
	g_fp=NULL;
}


