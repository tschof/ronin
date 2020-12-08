#MAKEFILE

CC= cc
DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-shared -O3 -march=native -fomit-frame-pointer -funroll-loops -ftree-vectorize -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE 
else
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
endif

SPREAD=0
ifeq ($(SPREAD),1)
CFLAGS=-g -shared -O3 -march=native -std=c99 -Wall -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_GNU_SOURCE -D_SPREAD_N_READY_
endif

SPREAD_DEBUG=0
ifeq ($(SPREAD_DEBUG),1)
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_SPREAD_N_READY_
endif
SYB_LIBS = -L../rex -lrex -lpthread -L../dartlib -ldartlib -L../dom_con -ldom_con -L../oats -loats -L../pillar -lpillar -L../pilllib -lpillbsd -ldl 
INCLUDES = -I../pilllib -I../pillar
EXECUTABLE = ptest
SRC = main.c 
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

indent:
	find -name '*.[ch]' | xargs indent -kr --no-tabs
clobber: clean
	rm -f *~

depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE

tags:
	tags *.[ch]
	tags -a ../jigglypuff/*.[ch]
	tags -a ../ex_db/*.[ch]
	tags -a ../server_objects/*.[ch]
	tags -a ../smtp/*.[ch]
