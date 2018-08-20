#ifndef _RECORD_PROCESS_H_
#define _RECORD_PROCESS_H_	

#ifdef __cplusplus
extern "C" {
#endif

#include "sort_data_from_radar.h"
#include "param_for_data_sort.h"

int  RECORD_PROCESS(unsigned char *buf,unsigned int len,int width ,int height,int fmt,radarRst *radar,float speed);
void RECORD_Param(const char *path );

#ifdef __cplusplus
};
#endif

#endif