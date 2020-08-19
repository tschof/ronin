#MAKEFILE
# See note in test.c for use of whole-program optimizations
#

CC=cc 
CFLAGS=-g -D_REENTRANT -O3 -fwhole-program
SYB_LIBS=-L../server_objects -lserver_obj -lpthread -lm
INCLUDES=-I../server_objects 
SRC = mock_tmlight.c test.c
MAKEDEPEND = gcc -M -o $*.d $<

all: mock_tmlight

mock_tmlight: ${SRC:.c=.o}
	$(CC) $(CFLAGS) $(INCLUDES) -o $@ $^ $(SYB_LIBS)

%.o: %.c
	$(CC) $(CFLAGS) $(INCLUDES)-fpic -Wall -g -c $< -MD -o $@

#	@cp $*.d $*.P; \
#	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
#	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
#	rm -f $*.d
#	include $(SRC:.c=.P)

clean:
	rm -rf mock_tmlight *.o *.log *.LOG

depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
