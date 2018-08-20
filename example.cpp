
#include "UartCan.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h> 
#include <signal.h>
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h> 
#include <sys/time.h>
#include <errno.h>
#include <termios.h>
#include <linux/videodev2.h>
#include "printlog.h"
#include "mxc_v4l2.h"

#include "device_camera.h"
#include "device_timer.h"
#include "sort_data_from_radar.h"
#include "param_for_data_sort.h"
#include "RECORD_PROCESS.h"

using namespace uartcan;





UartCan g_uartcan;
	
radarRst g_radar;

long long  start=0; 

static long long currenttime(){
    struct timeval now;
    gettimeofday(&now, NULL);
    long long when = now.tv_sec * 1000LL + now.tv_usec / 1000;
    return when;
}
static 	unsigned char  buf[23]={0x4B, 0x59, 0x4B, 0x4A ,0x77, 0x10, 0xff ,0x0, 0xff ,0x00, 0x08, 0xff , 0xff , 0 };

static pthread_mutex_t g_mutex_wmr;

static float g_speed;

static radarRst  g_RecordRadarRst;

void MwrCallback(const radarRst *radar,const radarRst *sradarRst, unsigned char speed)
{

	g_speed = speed;

	pthread_mutex_lock(&g_mutex_wmr);
	memcpy(&g_RecordRadarRst,sradarRst,sizeof(radarRst));
	pthread_mutex_unlock(&g_mutex_wmr);

}
void getWMRInfo(radarRst *radar)
{
	pthread_mutex_lock(&g_mutex_wmr);
	memcpy(radar,&g_RecordRadarRst,sizeof(radarRst));
	pthread_mutex_unlock(&g_mutex_wmr);

}


int process_cmdline(int argc, const char **argv)
{

    int i;

    for (i = 1; i < argc; i++) {

    	if(strcmp(argv[i], "-r") == 0){
            RECORD_Param(argv[++i]);
        }
    }
    return 0;
}



int main(int argc, char const *argv[])
{
	int count=0;
	int i=0;
	int ID=0x0;
	process_cmdline(argc, argv);

	g_uartcan.UartCanInit("/dev/ttymxc1",115200);	

	g_uartcan.SetMWRCallback(MwrCallback);


    unsigned char *g_framebuf=(unsigned char *)calloc(1,640*480*2);
    if(g_framebuf==NULL){   
    	err_msg("calloc faild \n");   
        return -1;
    }

	int ret = capture_init(640,480,V4L2_PIX_FMT_UYVY,"/dev/video0");
	if(ret<0)
	{
		err_msg("capture_init faild \n");
		return -1;
	}


	capture_timeInit(TIMER_BASE_HZ);

	int time_id=register_timer("main",TIMER_15_HZ);

	int datalen = 640*480*2;

	int recordlen=  640*480*2;

	pthread_mutex_init(&g_mutex_wmr,NULL);

	radarRst  mRecordRadarRst;
	
	while(1)
	{

		wait_timersignal(time_id);

		capture_getframedata(g_framebuf,&datalen);

		getWMRInfo(&mRecordRadarRst);

		RECORD_PROCESS(g_framebuf,recordlen,640 ,480,V4L2_PIX_FMT_UYVY,&mRecordRadarRst,g_speed);
		
	}
	g_uartcan.UartCanRelease();
	capture_release();
	return 0;
}


