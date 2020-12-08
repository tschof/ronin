all: timer-test

timer-test: timer-test.c
	gcc -g -D_GNU_SOURCE -DTESTING -Wall timer-test.c ../dart_timer.c ../timeval.c test_helper.c -lpthread -o timer-test

clean:
	rm -f timer-test

tags:
	etags *.[ch]
	etags -a ../dart_timer.[ch]
