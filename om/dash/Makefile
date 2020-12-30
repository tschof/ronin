#MAKEFILE

CC=cc 
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -g -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
SYB_LIBS=-L../server_objects -lserver_obj -pthread -lm
else
CFLAGS=-shared -g -std=c99 -pedantic -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
SYB_LIBS=-L../server_objects -lserver_obj -pthread -lm
endif
SPREAD_DEBUG=0
ifeq ($(SPREAD_DEBUG),1)
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_SPREAD_N_READY_
endif


INCLUDES=-I../server_objects 

SRC = dash.c

EXECUTABLE = libdash.so

OBJECTS = $(SRC:.c=.o)

MAKEDEPEND = gcc -M -o $*.d $<

all: $(SRC) $(EXECUTABLE)

libdash.so: $(OBJECTS)
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

