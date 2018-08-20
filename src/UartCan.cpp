
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <sys/ioctl.h>


#include "UartCan.h"
#include "UartCanSend.h"
#include "UartCanReceive.h"



namespace uartcan{
	

UartCan::UartCan()
{
	m_devfd=-1;
	m_isInited=0;
	printf("%s\n",__func__ );
}
UartCan::~UartCan()
{
	printf("%s\n",__func__ );
}

speed_t UartCan::baudrate_map(unsigned long b)
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

int UartCan::devInit(const char *dev,int  baudrate)
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

void UartCan::devRelease(void)
{
	printf("%s m_devfd %d \n",__func__ ,m_devfd);
	if(m_devfd<0)
		return;
	close(m_devfd);
	
	m_devfd=-1;
}

int UartCan::UartCanInit(const char *dev,int bandrate,bool sendData)
{
	if(m_isInited)
		return 0;
	if(m_devfd<0){
		m_devfd	  = devInit(dev,bandrate);
		if(m_devfd<0){
			goto err0;
		}
	}
	if(m_cansend.Init(m_devfd)){
			goto err1;
	}
	if(m_canreceive.Init(m_devfd)){
			goto err2;
	}
	mSendData=sendData;
	m_isInited=1;
	return 0;
err2:
	m_cansend.Release();		   	
err1:
	devRelease();	
err0:
	printf("%s faild \n",__func__ );
	return -1;
}



void UartCan::UartCanRelease(void)
{	
	if(!m_isInited)
		return ;
	m_cansend.Release();	
	m_canreceive.Release();
	devRelease();
	m_isInited=0;
}

void UartCan::GetUartCanSpeedresult(float *speed)
{
	m_canreceive.GetReceiveSpeed(speed);
}

void  UartCan::setParseCallback(ParseCallback callback) 
{
	m_canreceive.SetReceiveSpeedCallback(callback);
}
void UartCan::SendCarSpeed(int speed)
{
	m_cansend.CanSendSpeed(speed);
}
void UartCan::SetMWRCallback(MWRCallback mwrcallback)
{
	m_canreceive.SetParseMWRCallback(mwrcallback);
}

void UartCan::PutAdasinfoToUartCan(const ADAS_INFO *adasInfo)
{
	if(!m_isInited)
		return ;
	if(!mSendData){
		return ;
	}
	m_cansend.StartCanSendOne(adasInfo);
}

};





