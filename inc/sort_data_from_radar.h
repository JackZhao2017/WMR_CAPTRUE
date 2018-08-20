#ifndef _SORT_DATA_H_
#define _SORT_DATA_H_

#include "param_for_data_sort.h"
#include <string.h>
//input data struct
typedef struct radarObj{
	int   idx;
	int   flag;
	float disx;
	float disy;
	float dis;
	float vrel;
	float ang;
}radarObj;

typedef struct radarRst{
	radarObj obj[MAX_RADAR_DATA_NUM];
	int cnt;
}radarRst;

int sort_data_from_radar(radarRst* dst, radarRst* src, int num_input);

#endif