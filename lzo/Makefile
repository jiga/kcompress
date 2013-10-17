include ../makefile.inc

all: lzo.o kernel_lzo.o
 
lzo.o : minilzo.c minilzo.h lzoconf.h 
	${CC} ${CFLAGS} -o $@ -c minilzo.c 

kernel_lzo.o : minilzo.c minilzo.h lzoconf.h 
	${CC} ${MODCFLAGS} -o $@ -c minilzo.c
clean :
	\rm -f *.o *~
