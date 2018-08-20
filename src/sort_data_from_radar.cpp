#include "sort_data_from_radar.h"
#include "param_for_data_sort.h"
#include <math.h>
// #include <cmath>
#include <stdlib.h>



int compareDis(const void* r1,const void* r2)
{
	radarObj* data1 = (radarObj*)r1;
	radarObj* data2 = (radarObj*)r2;
	float delta ,delta1;
	//delta = data1->dis * cos(data1->ang) - data2->dis * cos(data2->ang); 
	//delta1 = data1->dis - data2->dis;
	delta = data1->disx  - data2->disx; 
	delta1 = data1->dis - data2->dis;
	if(delta < 0)
	{
		return -1;
	}
	else if (delta > 0)
	{
		return 1;
	}
	else
	{
		if (delta1 < 0)
			return -1;
		if (delta1 > 0)
			return 1;
		return 0;
	}
}
void sortByDis(radarObj *rc, int num)
{
	qsort(rc,num,sizeof(radarObj),compareDis);
}

int  sort_data_from_radar(radarRst* dst, radarRst* src, int num_input)
{
	float xdis;
	// select obj in lane
	int num_output = 0;
	//dst->cnt = src->cnt;
	for( int n=0; n<num_input; n++ )
	{
		//xdis = abs(src->obj[n].dis*sin(src->obj[n].ang));
		xdis = abs(src->obj[n].disy);
		if (xdis <= ROAD_WIDTH/2)
		{			
			memcpy(&dst->obj[num_output],&src->obj[n],sizeof(radarObj));
			num_output ++;	
		}
	}
	dst->cnt = num_output;
	//sort obj by distance
	sortByDis(dst->obj,dst->cnt);
	return 0;
}