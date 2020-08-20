#MAKEFILE

CC=cc 
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -O3 -march=native -g -std=c99 -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
else
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
endif


ifeq ($(DEBUG),0)
SYB_LIBS=-L../server_objects -lserver_obj -lpthread -L$(PSQLDIR)/lib -lpq
else
SYB_LIBS=-L../server_objects -lserver_obj -lpthread -L$(PSQLDIR)/lib -lpq
endif
INCLUDES=-I../server_objects -I$(PSQLDIR)/include -I$(PSQLDIR)/include/server 

SRC = generic_db.c

MAKEDEPEND = gcc -M -o $*.d $<

all: libdartdb_obj.so

libdartdb_obj.so: $(SRC:.c=.o)
	$(CC) $(CFLAGS) $(INCLUDES) -fpic -Wall -ggdb -o $@ $^ $(SYB_LIBS)
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

include $(SRC:.c=.P)
clean:
	rm -f libdartdb_obj.so *.o
indent:
	find -name '*.[ch]' | xargs indent -kr --no-tabs

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
