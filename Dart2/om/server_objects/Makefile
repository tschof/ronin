#MAKEFILE

CC=cc 

DEBUG=0
ifeq ($(DEBUG),0)
CFLAGS=-g -shared -O3 -march=native -std=c99 -Wall -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
SYB_LIBS=-pthread -lrt -lcrypto -lm
else
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE
SYB_LIBS=-pthread -lrt -lcrypto -lm
endif

TPOS=0
ifeq ($(TPOS),1)
CFLAGS=-g -shared -O3 -march=native std=c99 -Wall -D_GNU_SOURCE
endif
MIFID=0
ifeq ($(MIFID),1)
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_SPREAD_N_READY_ -D_MICRO_STAMPS_
endif
SPREAD=0
ifeq ($(SPREAD),1)
CFLAGS=-g -shared -O3 -march=native -std=c99 -Wall -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_GNU_SOURCE -D_SPREAD_N_READY_
endif
CHECK_SEQ=0
ifeq ($(CHECK_SEQ),1)
CFLAGS=-g -shared -O3 -march=native -std=c99 -Wall -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_CHECK_SEQ__
endif

SPREAD_DEBUG=0
ifeq ($(SPREAD_DEBUG),1)
CFLAGS=-shared -g -std=c99 -Wall -D_REENTRANT -D_POSIX_C_SOURCE=199309L -D_GNU_SOURCE -D_SPREAD_N_READY_
endif


INCLUDES=

SRC =  dart_allocator.c hashmap.c hmac_helper.c transaction_recorder.c sti_tree.c message_queue.c  parser_func.c rom_handler.c fix_handler.c fix_trans.c config_loader.c  dart_thr_pool.c dart_seq_request.c dart_resend_request.c databuf.c rb_tree.c gatekeeper.c async_server.c tm_to_str.c common_structs.c debug_logger.c dart_obj_factory.c dart_order_obj.c order_obj_fact.c master_config.c connection_interface.c ex_config_helper.c mailer.c lifecycle_master.c sock_recv_manager.c call_back_wrapper.c order_token_generator.c rom_fix_trans_funcs.c service_bureau.c suffix_map.c

MAKEDEPEND = $(CC) -M -o $*.d $<

all: libserver_obj.so

libserver_obj.so: $(SRC:.c=.o)
	$(CC) $(CFLAGS) $(INCLUDES) -fpic -Wall -ggdb -o $@ $^ $(SYB_LIBS)
%.o: %.c
	$(CC) $(INCLUDES) $(CFLAGS) -fpic -Wall -g -c $< -MD -o $@
	@cp $*.d $*.P; \
	sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e's/$$/ :/' < $*.d >> $*.P; \
	rm -f $*.d

include $(SRC:.c=.P)
clean:
	rm -f libserver_obj.so *.o
indent:
	find -name '*.[ch]' | xargs indent -kr --no-tabs

clobber: clean
	rm -f *~
depend:
	makedepend -- $(CFLAGS) -- $(SRC)

.PHONY: clean clobber# DO NOT DELETE
