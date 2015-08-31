ifneq ($(KERNELRELEASE),)
  obj-m := memory.o trusted.o iigp1.o 

else
        KERNELDIR ?= /lib/modules/$(shell uname -r)/build
        PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules
endif

clean:
	rm *.o *.mod.o *.mod.c *.ko modules.order Module.symvers
