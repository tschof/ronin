#MAKEFILE

CC=cc 

DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-g -shared -O3 -march=native -std=c99 -Wall -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
SYB_LIBS=-pthread -lrt
else
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
SYB_LIBS=-pthread -lrt 
endif
SPREAD=0
ifeq ($(SPREAD),1)
CFLAGS=-g -shared -O3 -march=native -std=c99 -Wall -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_GNU_SOURCE -D_SPREAD_N_READY_
endif

SPREAD_DEBUG=0
ifeq ($(SPREAD_DEBUG),1)
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_SPREAD_N_READY_
endif


INCLUDES=

SRC =  rex_allocator.c hashmap.c sti_tree.c message_queue.c parser_func.c databuf.c rb_tree.c tm_to_str.c ex_config_helper.c order_token_generator.c suffix_map.c config_loader.c rex_seq_request.c rex_resend_request.c rex_obj_factory.c log_mnger.c call_back_wrapper.c rex_thr_pool.c sock_recv_manager.c polling_server.c dart_order_obj.c dart_order_fact.c lifecycle_master.c fix_trans.c fix_parser.c fix_parser_factory.c fix_handler.c dart_handler.c connection_factory.c service_bureau.c boe_seq.c xorlist.c

MAKEDEPEND = $(CC) -M -o $*.d $<

all: librex.so

librex.so: $(SRC:.c=.o)
	$(CC) $(CFLAGS) $(INCLUDES) -fpic -Wall -ggdb -o $@ $^ $(SYB_LIBS)
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

include $(SRC:.c=.P)
clean:
	rm -f librex.so *.o
indent:
	find -name '*.[ch]' | xargs indent -kr --no-tabs

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
