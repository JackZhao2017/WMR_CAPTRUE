
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

using namespace uartcan;

pthread_t  pidinit;
pthread_t  pidproc;
pthread_t  pidrelease;

ADAS_INFO adasinfo;
int exitthead=0;
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
int devInit(const char *dev,int  baudrate);
speed_t baudrate_map(unsigned long b);
int g_cred=0,g_dis;
void MwrCallback(const radarRst *radar,const radarRst *sradarRst)
{
	int i=0;
	for( i=0;i<radar->cnt;i++){
		if(i==1){
			break;
		}
		printf("index %d ,dis %.1f , speed %.f angle %.1f \n\n\n",i,radar->obj[i].dis,radar->obj[i].vrel,radar->obj[i].ang);
	}
	if(radar->cnt){
		g_cred = 1 ;
		buf[7]&=0xe7; 
		buf[7]|=0x08;
		g_dis =radar->obj[0].dis; 
		buf[13]=(g_dis*2)&0xff;
	}else{
		buf[7]&=0xe7;
		g_cred =0;
	}
}
void SpeedCallback(float *speed)
{	

	printf("%s %.1f  %lld\n",__func__,*speed,currenttime()-start);
	start = currenttime();
}

int main(int argc, char const *argv[])
{
	int count=0;
	int i=0;
	int ID=0x0;
	g_uartcan.setParseCallback(SpeedCallback);
	g_uartcan.UartCanInit("/dev/ttyMT3",115200);	
	g_uartcan.SetMWRCallback(MwrCallback);
	int fd = devInit("/dev/ttyMT2",19200);
	while(1)
	{
		write(fd,buf,23);

		usleep(50000);
	}
	g_uartcan.UartCanRelease();
	return 0;
}


int devInit(const char *dev,int  baudrate)
{
	struct termios options;
	int fd=-1;

	fd = open(dev, O_RDWR | O_NOCTTY);//O_NONBLOCK
	if (fd <0) {
		goto err;
	}
	fcntl(fd, F_SETFL, 0);
	tcgetattr(fd, &options);

	options.c_cflag &= ~CSTOPB;
	options.c_cflag &= ~CSIZE;
	options.c_cflag &= ~PARODD;
	options.c_cflag |= CS8;
	options.c_cflag &= ~CRTSCTS;

	options.c_lflag &= ~(ICANON | IEXTEN | ISIG | ECHO| ECHONL);
	options.c_oflag &= ~OPOST;
	options.c_iflag &= ~(ICRNL | INPCK | ISTRIP | IXON | BRKINT );

	options.c_cc[VMIN] = 1;
	options.c_cc[VTIME] = 0;
	options.c_cflag |= (CLOCAL | CREAD);

	if(baudrate){
		cfsetispeed(&options, baudrate_map(baudrate));
		cfsetospeed(&options, baudrate_map(baudrate));
	}
	tcflush(fd ,TCIFLUSH);
	tcsetattr(fd, TCSANOW, &options);
	printf("<%s>UART %d, %dbit, %dstop, %s, HW flow %s\n",__func__,baudrate,8,
	       (options.c_cflag & CSTOPB) ? 2 : 1,
	       (options.c_cflag & PARODD) ? "PARODD" : "PARENB",
	       (options.c_cflag & CRTSCTS) ? "enabled" : "disabled");
	printf("%s fd %d \n",__func__ ,fd);
	return fd;
err:   	
	fd =-1;
	return -1;
}
speed_t baudrate_map(unsigned long b)
{
    speed_t retval;
    switch(b)
    {
        case 9600:
            retval = B9600;
            break;

        case 19200:
            retval = B19200;
            break;

        case 38400:
            retval = B38400;
            break;

        case 57600:
            retval = B57600;
            break;

        case 115200:
            retval = B115200;
            break;
        default:
        	retval =0;
        	break;
    }
    return(retval);
}