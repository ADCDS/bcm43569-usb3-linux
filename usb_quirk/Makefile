obj-m += bcm43569_config_quirk.o

KDIR ?= /lib/modules/$(shell uname -r)/build

all:
	$(MAKE) -C $(KDIR) M=$(CURDIR) -j2 modules

clean:
	$(MAKE) -C $(KDIR) M=$(CURDIR) clean
