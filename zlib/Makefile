include ../makefile.inc

all: zlib.o

zlib.o: zlib_deflate.o zlib_inflate.o
	ld -o $@ -r $?

zlib_deflate.o: deflate.o deftree.o 
	ld -o $@ -r $?

zlib_inflate.o: inflate.o infblock.o infcodes.o infutil.o inffast.o inftrees.o	
	ld -o $@ -r $?

deflate.o:deflate.c defutil.h 

deftree.o:deftree.c defutil.h

inflate.o:inflate.c infutil.h

infblock.o:infblock.c infblock.h

infcodes.o:infcodes.c infcodes.h

infutile.o:infutil.c infutil.h

inffast.o:inffast.c inffast.h

inftrees.o:inftrees.c inftrees.h

.c.o:
	${MYCC} ${BCC_MODCFLAGS} -c $< 
clean :
	\rm -f *.o *~
