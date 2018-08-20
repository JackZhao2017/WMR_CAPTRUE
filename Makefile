
# CROSS_COMPILE=/mnt/hgfs/android-ndk-r14b/my_toolchain/bin/aarch64-linux-android-

CROSS_COMPILE=/home/jack/Desktop/imax6/linux3.0.35/toolchain/gcc-linaro-arm-linux-gnueabihf-4.9-2014.09_linux/bin/arm-linux-gnueabihf-

OBJDIR = .
SRCDIR = .

AS		= $(CROSS_COMPILE)as
LD		= $(CROSS_COMPILE)ld
CC		= $(CROSS_COMPILE)gcc
CPLUS	= $(CROSS_COMPILE)g++
CPP		= $(CC) -E
AR		= $(CROSS_COMPILE)ar
NM		= $(CROSS_COMPILE)nm
STRIP	= $(CROSS_COMPILE)strip
OBJCOPY	= $(CROSS_COMPILE)objcopy
OBJDUMP	= $(CROSS_COMPILE)objdump
RM		= rm -f
MAKEDIR	= mkdir -p

CINC   :=-I./inc/
CINC   +=-I./
CINC   +=-I./encoder/include/
CINC   +=-I./recorder/


LIBSO:=$(SRCDIR)/encoder/lib/libencoder.so
LIBSO+=$(SRCDIR)/encoder/lib/libvpu.so.4


replay.out:demo.c device_timer.c device_camera.c  device_imagout.c  recorder/*.c
			    
	${CPLUS}  -O3 ${CINC}  ${LIBSO} $^ -o $@ -lpthread


UartCanCpp.out:example.cpp device_timer.c device_camera.c src/*.cpp recorder/*.cpp recorder/*.c
			    
	${CPLUS}  -O3 ${CINC}  ${LIBSO} $^ -o $@ -lpthread


DEMO=UartCanCpp.out

.PHONY: all
all : ${DEMO}


.PHONY: clean
clean :
	rm -f  ${DEMO} 
