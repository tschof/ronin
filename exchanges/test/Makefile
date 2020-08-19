#MAKEFILE

CC=cc 

CFLAGS=-g -D_REENTRANT

SYB_LIBS=-L../server_objects -lserver_obj -lpthread -L../ex_db -ldartdb_obj -L../smtp -ldartsmtp -L../ex_common -ldartexcommon -L../complex -ldartcomplex -L../utpdirect -lutpdirect

INCLUDES=-I../server_objects -I../ex_db -I../smtp -I../ex_common -I../complex -I../utpdirect

SRC = test.c databuftest.c suffixmaptests.c hashtest.c ex_tester.c db_tester.c fix_to_rom_tests.c complex_ord_tests.c reset_calculator.c rb_tree_test.c cx_tester.c tran_rec_test.c rom_tester.c mail_tester.c fix_handler_test.c tm_to_str_test.c fixmsg_addstr_test.c test_message_queue.c test_config.c utp_tester.c test_common_structs.c

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

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
