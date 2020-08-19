#MAKEFILE

CC= cc
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -g -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
else
CFLAGS=-Wall -Wextra -shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
endif

SYB_LIBS = -L../server_objects -lserver_obj -lpthread -L../ex_db -ldartdb_obj -L../smtp -ldartsmtp -L../ex_common -ldartexcommon

INCLUDES = -I../server_objects -I../ex_db -I../smtp -I../ex_common

EXECUTABLE = liffe-us

SRC=generic_fix_interface.c $(EXECUTABLE).c symbol-map.c
TEST_SRC=symbol-map.c liffe-us-tests.c
TEST=$(EXECUTABLE)-tests.so

OBJECTS = $(SRC:.c=.o)

MAKEDEPEND = gcc -M -o $*.d $<

all: $(SRC) $(EXECUTABLE) $(TEST)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(SYB_LIBS)

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d
include $(SRC:.c=.P)

$(TEST): $(TEST_SRC)
	$(CC) $(TEST_SRC) -CFLAGS -o $@ -g -shared -fPIC -DBAZTEST -L../baz -lbaz -L ../server_objects -lserver_obj -I .. -L ../ex_common -ldartexcommon

clean:
	rm -f $(EXECUTABLE) *.o

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

