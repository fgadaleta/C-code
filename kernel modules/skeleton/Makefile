UNAME := $(shell uname -r)
KERNEL30 := 3.0
KERNELVERSION := $(findstring $(KERNEL30),$(UNAME))

all:: user

ifeq ($(KERNELVERSION),3.0)

obj-m	:= skeleton.o

INCLUDE	:= -I/usr/include/asm/mach-default/
KDIR	:= /lib/modules/$(shell uname -r)/build
PWD		:= $(shell pwd)

all::
	$(MAKE) -C $(KDIR) $(INCLUDE) SUBDIRS=$(PWD) modules
	
else

TARGET	:= skeleton
INCLUDE	:= -I/lib/modules/`uname -r`/build/include -I/usr/include/asm/mach-default/
CFLAGS	:= -O2 -Wall -DMODULE -D__KERNEL__ -DLINUX
CC	:= gcc

all:: ${TARGET}.o

${TARGET}.o: ${TARGET}.c
	$(CC) $(CFLAGS) ${INCLUDE} -c ${TARGET}.c
	
endif

user: user.c
	gcc -o $@ $<
clean:
	rm -rf *.o *.ko
