#include "UartCanRead.h"
#include "UartCanRingbuffer.h"
#include "UartCanParse.h"
#include <stdio.h>
#include <stdlib.h>
#include <sys/epoll.h> 
#include <sys/timerfd.h>
#include <signal.h>
#include <fcntl.h> 
#include <unistd.h>
#include <errno.h> 
#include <sys/time.h>
#include <errno.h>

namespace uartcan{

UartCanRead::UartCanRead()
{
	 g_dev_fd=-1;
   isInited=0;
   mParse=NULL;
	 printf("%s()\n",__func__ );
}
UartCanRead::~UartCanRead()
{
	 printf("%s()\n",__func__ );
}

int UartCanRead::epoll_add_fd(int fd)
{
    int ret;
    int opts;
    struct epoll_event event;

    memset(&event, 0, sizeof(event));
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;

        
    opts=fcntl(fd,F_GETFL);
    if(opts<0)
    {
        perror("fcntl(fd,GETFL)");
        return -1;
    }
    opts = opts | O_NONBLOCK;
    if(fcntl(fd,F_SETFL,opts)<0)
    {
        perror("fcntl(fd,SETFL,opts)");
    }

    ret = epoll_ctl(g_epfd, EPOLL_CTL_ADD, fd, &event);
    if(ret < 0) {
        printf("epoll_ctl Add fd:%d error, Error:[%d:%s]\n", fd, errno, strerror(errno));
        return -1;
    }
    return 0;    
}
int UartCanRead::timerfd_init()
{
    int tmfd;
    int ret;
    struct itimerspec new_value;

    new_value.it_value.tv_sec = 1;
    new_value.it_value.tv_nsec = 0;
    new_value.it_interval.tv_sec = 1;
    new_value.it_interval.tv_nsec = 0;
    
    tmfd = timerfd_create(CLOCK_REALTIME, 0);
    if (tmfd < 0) {
        printf("timerfd_create error, Error:[%d:%s]\n", errno, strerror(errno));
        return -1;
    }

    ret = timerfd_settime(tmfd, 0, &new_value, NULL);
    if (ret < 0) {
        printf("timerfd_settime error, Error:[%d:%s]\n", errno, strerror(errno));
        close(tmfd);
        return -1;
    }

    if (epoll_add_fd(tmfd)) {
        close(tmfd);
        return -1;
    }
    g_tim_fd = tmfd;
    return 0;
}
int UartCanRead::epollfd_init(void)
{
    int epfd;

    epfd = epoll_create(256); 
    if (epfd < 0) {
        printf("epoll_create error, Error:[%d:%s]\n", errno, strerror(errno));
        return -1;
    }
    g_epfd = epfd;
    return epfd;
}

void *UartCanRead::UartCanReadfunc(void *arg)
{
  UartCanRead* ptr = reinterpret_cast<UartCanRead*>(arg);
  int i=0,j=0,k=0;
  int fd_cnt = 0;
  int sfd;
  struct epoll_event events[256];    
  unsigned char buf[1024];
  memset(events, 0, sizeof(events)); 
	while(1)
	{
    fd_cnt = epoll_wait(ptr->g_epfd, events, 256, -1); 
    if(ptr->thread_exit){
        break;
    }
    if (fd_cnt == -1) {
        if(errno == EINTR){
            continue;
        }
    }
    for(i = 0; i < fd_cnt; i++) 
    {   
        sfd = events[i].data.fd;
        if(events[i].events & EPOLLIN) 
        {   
            while(1){
              int size=read(sfd,buf,sizeof(buf)-1); 
              if(size<0){
                  if(errno==EAGAIN){
                    if(ptr->mParse!=NULL){
                        ptr->mParse->PostParse();
                    }
                    break;
                  }               
              }
              if(ptr->mParse!=NULL){
                  ptr->mParse->PutdataToParse(buf,size);
              } 
            }    
        }   
    }
  }
	pthread_exit(0);
  return NULL;
}
int UartCanRead::Init(int fd , UartCanParse *parse)
{
    int ret=0;
    if(fd<0){
      goto err0;
    }
    if(isInited){
      return 0;
    }
    g_dev_fd=fd;
    if(epollfd_init()<0){
      goto err0;
    }
    if(epoll_add_fd(fd)<0){
      goto err;
    }

    ret=pthread_create(&thread_pid,NULL,UartCanReadfunc,this);
    if(ret){
      goto err;
    }
    mParse=parse;
    isInited=1;
    thread_exit=0;
    printf("%s UartCanRead success\n",__func__ );
    return 0;
err:
    close(g_epfd);
err0:
    isInited=0;
    printf("%s UartCanRead faild\n",__func__ );
    return -1;
}

void UartCanRead::Release(void)
{   
     if(!isInited){
        return;
     }
     thread_exit=1;
     timerfd_init();
     if(pthread_join(thread_pid, NULL) != 0) {
        printf("%s faild\n",__func__ );
     }
     mParse=NULL;
     isInited=0;
}

};