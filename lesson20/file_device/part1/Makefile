obj-m += test1_init.o

all:
	make -C /lib/modules/6.17.0-35-generic/build M=$(PWD) modules

clean:
	make -C /lib/modules/6.17.0-35-generic/build M=$(PWD) clean