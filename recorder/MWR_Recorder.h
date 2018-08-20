#ifndef _MWR_RECORDER_H_
#define _MWR_RECORDER_H_


#include "sort_data_from_radar.h"
#include "param_for_data_sort.h"

#define RECORDNUM  10

typedef struct _MWR_RECORD
{
	radarObj obj[RECORDNUM];
	int cnt;
	float speed;
}RecordRst;

int  MWR_RecorderInit(const char *path);
int MWR_RecorderRead(RecordRst *radar);
int MWR_RecorderWrite(radarRst *radar,float speed);
void MWR_RecorderRelease(void);

#endif