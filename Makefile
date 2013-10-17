WARN 	:= -W -Wall -Wstrict-prototypes -Wmissing-prototypes
INCLUDE	:= -I /lib/modules/`uname -r`/build/include
CFLAGS	:= -O2 -DMODULE -D__KERNEL__ ${INCLUDE}
CC	:= gcc

all: lkmcompress.o compressor

lkmcompress.o:	kcompress.o ./zlib/zlib.o ./lzo/kernel_lzo.o fiok.o
	ld -r -L. kcompress.o fiok.o ./lzo/kernel_lzo.o ./zlib/zlib.o -o lkmcompress.o

kcompress.o: kcompress.c kcompress.h 
	${CC} ${CFLAGS} -c kcompress.c

fiok.o:	fiok.c fiok.h
	${CC} ${CFLAGS} -c fiok.c

compressor: compressor.c
	${CC} -o compressor compressor.c

.PHONY: clean

clean:
	rm -Rf *.o compressor

install:
	insmod ./lkmcompress.o

uninstall:
	rmmod lkmcompress
 
