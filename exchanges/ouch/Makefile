CC=cc
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -g -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_BOGUS_FIX_=1
else
CFLAGS=-shared -g -D_REENTRANT
endif

SYB_LIBS=-L../server_objects -lserver_obj -lpthread -L../ex_db -ldartdb_obj -L../smtp -ldartsmtp -L../ex_common -ldartexcommon
INCLUDES=-I../server_objects -I../ex_db -I../smtp -I../ex_common -I..
EXECUTABLE=ouch
TEST=ouch-tests.so
SRC=$(EXECUTABLE).c ouch_util.c soupbin.c exchange_interface.c conf_util.c ouch41.c dart_std_id_generator.c dart_to_ouch.c dstring.c
TEST_SRC= ouch-test.c soupbin.c conf_util.c ouch41.c ouch_util.c

OBJECTS=$(SRC:.c=.o)
MAKEDEPEND=gcc -M -o $*.d $<

all: $(SRC) $(EXECUTABLE) $(TEST)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(SYB_LIBS)

$(TEST): $(TEST_SRC)
	$(CC) $(TEST_SRC) -o $@ -g -shared -fPIC -L../baz -lbaz -L ../server_objects -lserver_obj -I .. -L ../ex_common -ldartexcommon

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d
include $(SRC:.c=.P)

clean:
	rm -f $(EXECUTABLE) *.o $(TEST)

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
