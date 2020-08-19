#MAKEFILE

CC=cc 
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -g -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
SYB_LIBS=-L../server_objects -lserver_obj -L../ex_db -ldartdb_obj -L../smtp -ldartsmtp -pthread
else
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
SYB_LIBS=-L../server_objects -lserver_obj -L../ex_db -ldartdb_obj -L../smtp -ldartsmtp -pthread 
endif

INCLUDES=-I../server_objects -I../ex_db -I../smtp

SRC = ex_common.c ex_config_helper.c order_token_creator.c suffixmap.c rom2fix.c cx_object.c cx_ouch_helper.c options_common.c string_table.c

MAKEDEPEND = gcc -M -o $*.d $<

all: libdartexcommon.so

libdartexcommon.so: $(SRC:.c=.o)
	$(CC) $(CFLAGS) $(INCLUDES) -fpic -Wall -ggdb -o $@ $^ $(SYB_LIBS) $(LDFLAGS)
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

include $(SRC:.c=.P)
clean:
	rm -f libdartexcommon.so *.o

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
