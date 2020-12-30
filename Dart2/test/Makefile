#MAKEFILE

CC=cc 

CFLAGS=-g -D_REENTRANT

SPREAD_DEBUG=0
ifeq ($(SPREAD_DEBUG),1)
CFLAGS=-g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_SPREAD_N_READY_
endif

SYB_LIBS=-L../rex -lrex -lpthread -L../dartlib -ldartlib -L../utp_dma -lutpdma -L../oats -loats -L../dom_con -ldom_con -L../eurex -leurex -L../otto -lotto -L../cfeboe -lcfe -L../pillar -lpillar -ldl 

INCLUDES=-I../rex -I../dartlib -I../utp_dma -I../eurex -I../otto -I../cfeboe -I../pillar

SRC = test.c databuftest.c suffixmaptests.c hashtest.c reset_calculator.c rb_tree_test.c tm_to_str_test.c test_message_queue.c test_config.c test_dart_order_obj.c test_allocator.c rom_tester.c fix_handler_test.c test_fix_trans.c test_common_structs.c test_utp_dma.c otto_tests.c boe_tester.c

MAKEDEPEND = gcc -M -o $*.d $<

all: test

test: $(SRC:.c=.o)
	$(CC) $(CFLAGS) $(INCLUDES) -fpic -Wall -ggdb -o $@ $^ $(SYB_LIBS)

%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d
include $(SRC:.c=.P)

clean:
	rm -f test *.o INET_*
indent:
	find -name '*.[ch]' | xargs indent -kr --no-tabs

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
