WARN 	:= -W -Wall -Wstrict-prototypes -Wmissing-prototypes
INCLUDE	:= -I /lib/modules/`uname -r`/build/include
CFLAGS	:= -fpermissive -O2 -g
BCC_MODCFLAGS	:= $(CFLAGS) -DMODULE -D__KERNEL__ -DLINUX ${INCLUDE}
MYCC	:= gcc

