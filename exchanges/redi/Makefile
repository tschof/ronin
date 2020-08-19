#MAKEFILE

CC= cc
CFLAGS = -g -D_REENTRANT
SYB_LIBS = -L../server_objects -lserver_obj -lpthread -L../ex_db -ldartdb_obj -L../smtp -ldartsmtp -L../ex_common -ldartexcommon
INCLUDES = -I../server_objects -I../ex_db -I../smtp -I../ex_common
EXECUTABLE = redi
SRC = generic_fix_interface.c $(EXECUTABLE).c
OBJECTS = $(SRC:.c=.o)
MAKEDEPEND = gcc -M -o $*.d $<

all: $(SRC) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(SYB_LIBS)

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d
include $(SRC:.c=.P)

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
