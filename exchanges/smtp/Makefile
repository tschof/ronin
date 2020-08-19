#MAKEFILE

CC=gcc 
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
else
CFLAGS=-shared -g -std=c99 -pedantic -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
endif

#CFLAGS=-shared -g -std=c99 -pedantic -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE

ifeq ($(DEBUG),0)
SYB_LIBS=-pthread
else
SYB_LIBS=-pthread 
endif
INCLUDES=-I../server_objects

SRC = mailer.c

MAKEDEPEND = gcc -M -o $*.d $<

all: libdartsmtp.so

libdartsmtp.so: $(SRC:.c=.o)
	$(CC) $(CFLAGS) $(INCLUDES) -fpic -Wall -ggdb -o $@ $^ $(SYB_LIBS)
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

include $(SRC:.c=.P)
clean:
	rm -f libdartsmtp.so *.o

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
