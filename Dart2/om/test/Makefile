#MAKEFILE

CC=cc 

CFLAGS=-g -D_REENTRANT

SPREAD_DEBUG=0
ifeq ($(SPREAD_DEBUG),1)
CFLAGS=-g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_SPREAD_N_READY_
endif

SYB_LIBS=-L../server_objects -lserver_obj -lpthread -L../ex_db -ldartdb_obj -L../utpdirect -lutpdirect -L../ice -lice -L../jigglypuff -ljiggley -L../oats -loats -L../globex -lglobex -L../boe -lboe -L../ise -lise -lcrypto -lm

INCLUDES=-I../server_objects -I../ex_db -I../utpdirect -I../jigglypuff -I../oats -I../ice -I../globex -I../boe -I../ise

SRC = test.c databuftest.c suffixmaptests.c hashtest.c fix_handler_test.c rom_tester.c test_fix_trans.c reset_calculator.c rb_tree_test.c  tran_rec_test.c mail_tester.c  tm_to_str_test.c test_message_queue.c test_config.c utp_tester.c test_common_structs.c test_dart_order_obj.c jiggly_test.c  test_oats.c ice_tester.c test_allocator.c test_globex.c boe_tester.c ise_tester.c

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
