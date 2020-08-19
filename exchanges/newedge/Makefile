#MAKEFILE
CC=gcc
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -g -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
else
CFLAGS=-Wall -Wextra -shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
endif
LIB=-L../server_objects -lserver_obj -lpthread -L../ex_db -ldartdb_obj -L../smtp -ldartsmtp -L../ex_common -ldartexcommon
INC=-I../server_objects -I../ex_db -I../smtp -I../ex_common
EXE=newedge

SRC=generic_fix_interface.c $(EXE).c symbol-map.c destmap.c
TST_SRC=symbol-map.c newedge-tests.c destmap.c
TST=$(EXE)-tests.so
OBJ=$(SRC:.c=.o)
MAKEDEPEND=gcc -M -o $*.d $<

all: $(SRC) $(EXE) $(TST)

$(EXE): $(OBJ)
	$(CC) $(OBJ) -o $@ $(LIB)

%.o: %.c
	$(CC) $(INC) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d
include $(SRC:.c=.P)

$(TST): $(TST_SRC)
	$(CC) $(TST_SRC) $(CFLAGS) -Wno-implicit -o $@ -g -shared -fPIC -DBAZTEST -L../baz -lbaz -L ../server_objects -lserver_obj -I .. -L ../ex_common -ldartexcommon

clean:
	rm -f $(EXE) *.o
	rm -f $(EXE)-tests.so

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

dist:
	../../tools/build/make-dart-dist dist-targets

