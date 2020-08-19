#MAKEFILE

CC=cc 
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
else
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
endif
#CFLAGS=-shared -O3 -march=core2 -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1

#CFLAGS=-shared -g -std=c99 -pedantic -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE

SYB_LIBS=-L../server_objects -lserver_obj -lpthread -L../smtp -ldartsmtp -L../ex_common -ldartexcommon -L../complex -ldartcomplex -L../ex_db -ldartdb_obj

INCLUDES=-I../server_objects -I../smtp -I../ex_common -I../complex

SRC = phlx.c

EXECUTABLE = phlx

OBJECTS = $(SRC:.c=.o)

MAKEDEPEND = gcc -M -o $*.d $<

all: $(SRC) $(EXECUTABLE)

phlx: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(SYB_LIBS)  
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d
include $(SRC:.c=.P)

clean:
	rm -f phlx *.o

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
