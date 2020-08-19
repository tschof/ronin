#MAKEFILE

CC=cc 
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
else
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
endif

#CFLAGS=-shared -g -std=c99 -pedantic -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE

SYB_LIBS=-L../server_objects -lserver_obj -lpthread -L../ex_common -ldartexcommon -L../smtp -ldartsmtp -L../complex -ldartcomplex

INCLUDES=-I../server_objects -I../ex_common -I../complex

SRC = fix_server.c 

MAKEDEPEND = gcc -M -o $*.d $<

all: fix_server

fix_server: fix_server.o
	$(CC)  $< -o $@ $(SYB_LIBS)

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d
include $(SRC:.c=.P)

clean:
	rm -f fix_server *.o fix_server-*.tar.gz

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE

dist:
	../../tools/build/make-dart-dist dist-targets
