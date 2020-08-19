#MAKEFILE

CC=cc 
#CFLAGS=-shared -O3 -march=core2 -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -g -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
SYB_LIBS=-L../server_objects -lserver_obj -L../ex_db -ldartdb_obj -L../smtp -ldartsmtp -pthread -L../ex_common -ldartexcommon
else
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
SYB_LIBS=-L../server_objects -lserver_obj -L../ex_db -ldartdb_obj -L../smtp -ldartsmtp -pthread -L../ex_common -ldartexcommon 
endif


INCLUDES=-I../server_objects -I../smtp -I../ex_db -I../ex_common

SRC = arcadirect.c ad_parser.c 

EXECUTABLE = libarcadirect.so

OBJECTS = $(SRC:.c=.o)

MAKEDEPEND = gcc -M -o $*.d $<

all: $(SRC) $(EXECUTABLE)

libarcadirect.so: $(OBJECTS)
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

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)
tags:
	etags *.[ch]
	etags -a ../ex_common/*.[ch]
	etags -a ../ex_db/*.[ch]
	etags -a ../server_objects/*.[ch]
	etags -a ../smtp/*.[ch]
p_tags:
	tags *.[ch]
	tags -a ../ex_common/*.[ch]
	tags -a ../ex_db/*.[ch]
	tags -a ../server_objects/*.[ch]
	tags -a ../smtp/*.[ch]

.PHONY: clean clobber# DO NOT DELETE
