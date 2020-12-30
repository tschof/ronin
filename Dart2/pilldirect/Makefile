#MAKEFILE

CC=cc 
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -g -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize 
SYB_LIBS=-L../rex -lrex -L../dartlib -ldartlib -L../dom_con -ldom_con -L../oats -loats -pthread -lonload_zf
else
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
SYB_LIBS=-L../rex -lrex -L../dartlib -ldartlib -L../dom_con -ldom_con -L../oats -loats -pthread  -lonload_zf
endif


INCLUDES=-I../rex -I../dartlib -I../oats -I../pillar

SRC = pilldirect.c ../pillar/pillar_oats_parser.c

EXECUTABLE = libpilldirect.so

OBJECTS = $(SRC:.c=.o)

MAKEDEPEND = gcc -M -o $*.d $<

all: $(SRC) $(EXECUTABLE)

libpilldirect.so: $(OBJECTS)
	$(CC) $(CFLAGS) $(INCLUDES) -fpic -Wall -ggdb -o $@ $^ $(SYB_LIBS)  
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d
include $(SRC:.c=.P)

clean:
	rm -f *.so *.o
indent:
	find -name '*.[ch]' | xargs indent -kr --no-tabs

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
