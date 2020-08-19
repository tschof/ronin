#MAKEFILE

PSQLDIR=/usr/local/pgsql
CC=cc 
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -O3 -g -std=c99 -march=native -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_REENTRANT
else
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
endif

ifeq ($(DEBUG),0)
SYB_LIBS=-L../server_objects -lserver_obj -lpthread -ldl -L$(PSQLDIR)/lib -lpq
else
SYB_LIBS=-L../server_objects -lserver_obj -lpthread -ldl -L$(PSQLDIR)/lib -lpq
endif
INCLUDES=-I../server_objects -I$(PSQLDIR)/include -I$(PSQLDIR)/include/server

SRC = psql_db.c

MAKEDEPEND = gcc -M -o $*.d $<

all: libdartpsqldb_obj.so

libdartpsqldb_obj.so: $(SRC:.c=.o)
	$(CC) $(CFLAGS) $(INCLUDES) -fpic -Wall -ggdb -o $@ $^ $(SYB_LIBS) $(LDFLAGS)
%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

include $(SRC:.c=.P)
clean:
	rm -f libdartpsqldb_obj.so *.o

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
