#MAKEFILE
CC=cc 
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -O3 -march=core2 -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
else
CFLAGS=-shared -g -std=c99 -pedantic -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
endif
#CFLAGS=-g -D_REENTRANT -pedantic -std=c99 -D_POSIX_C_SOURCE=200809
LIBS=-L../server_objects -lserver_obj -lpthread -L../ex_db -ldartdb_obj -L../smtp -ldartsmtp -L../ex_common -ldartexcommon
INCLUDES=-I../server_objects -I../ex_db -I../smtp -I../ex_common
SRC=blz.c generic_fix_interface.c destmap.c
EXECUTABLE = blz
OBJECTS = $(SRC:.c=.o)
MAKEDEPEND = gcc -M -o $*.d $<

all: $(SRC) $(EXECUTABLE)

blz: $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LIBS)  
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d
include $(SRC:.c=.P)

clean:
	rm -f blz *.o

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE

tags:
	etags *.[ch]
	etags -a ../ex_common/*.[ch]
	etags -a ../ex_db/*.[ch]
	etags -a ../server_objects/*.[ch]
	etags -a ../smtp/*.[ch]

lint:
	splint $(INCLUDES) blz_rom2fix.c
