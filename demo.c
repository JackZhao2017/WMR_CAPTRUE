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
#include "Decoder.h"
#include "device_camera.h"
#include "device_timer.h"
#include "sort_data_from_radar.h"
#include "param_for_data_sort.h"
#include "RECORD_PROCESS.h"
#include "device_imagout.h"
#include "MWR_Recorder.h"


static DecoderHandle g_handle=NULL;
static int g_width      =640;
static int g_height     =480;
static int g_fmt        =V4L2_PIX_FMT_UYVY;
static int g_fps        =30;

char name_path[128];

int  getframedata(unsigned char *buf,int *len)
{
    static int  isRead=0; 
    int ret=0;
    

    if(!isRead){             
        g_handle = Decoder_open("hardh264v2",name_path,&g_width,&g_height,&g_fps,g_fmt); 
        if(!g_handle){
           return -1;
        }else{
           isRead=1;
        }
    }
    ret=Decoder_read(g_handle,buf,len);
    if(ret<0){
       Decoder_close(g_handle);
       isRead=0;
    }
    return 0;
}

int main(int argc, char const *argv[])
{
	/* code */
	printf("%s\n",argv[1] );

	char name_fn[128];

	memset(name_fn,0,sizeof(name_fn));
	sprintf(name_fn,"%s.bin",argv[1] );
	MWR_RecorderInit(name_fn);

	memset(name_fn,0,sizeof(name_fn));
	sprintf(name_fn,"%s.h264",argv[1] );
	memcpy(name_path,name_fn,128);

    unsigned char *g_framebuf=(unsigned char *)calloc(1,640*480*2);
    if(g_framebuf==NULL){   
    	err_msg("calloc faild \n");   
        return -1;
    }
	capture_timeInit(TIMER_BASE_HZ);

	int time_id = register_timer("main",TIMER_15_HZ);


	int datalen = 640*480*2;

	int recordlen =  640*480*2;

	CAPTURE_IMAGOUT_INFO info;
	
	memset(&info,0,sizeof(info));
    info.in_width=640;
    info.in_height=480;
    info.display_width=640;
    info.display_height=480;
	
	if(v4l_output_init(&info)<0){
		return -1;
	}

	RecordRst gradar;

	while(1)
	{
		int ret = MWR_RecorderRead(&gradar);

		getframedata(g_framebuf,&recordlen);

		printf("ret %d  cnt  %d speed %.1f \n",ret ,gradar.cnt, gradar.speed);

		if(gradar.cnt){
			printf("dis %.1f\n",gradar.obj[0].disx );
		}

		v4l_output_process(g_framebuf,datalen);

		wait_timersignal(time_id);

	}


	return 0;
}

