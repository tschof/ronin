#include "../dart_timer.h"
typedef struct dart_timer dart_timer;
#include "test_helper.h"
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <errno.h>
#include <stdlib.h>
#include <stdint.h>

void dart_timer_callback(struct dart_timer *t, void *data)
{
    printf("%s: ", __FUNCTION__);
    dump_current_thread_id();
    printf("\n");
    uintptr_t i = (uintptr_t) data;
    if (i == 0x1) {
        struct timeval next;
        gettimeofday(&next, 0);
        next.tv_usec += 1;
        add_dart_timer(t, next, dart_timer_callback, 0x0);
    }
}

void *worker(void *thread_arg)
{
    dart_timer *t = thread_arg;
    time_t end = time(0) + 5;
    struct timeval next;
    for (gettimeofday(&next, 0), next.tv_usec += 100;
         next.tv_sec < end; gettimeofday(&next, 0), next.tv_usec += 100) {
        add_dart_timer(t, next, dart_timer_callback, (void *) 0x1);
        struct timespec wait_time = {
            .tv_sec = 0,
            .tv_nsec = 10000,
        }, remaining_time;
        int retval;
        while ((retval = nanosleep(&wait_time, &remaining_time)) == -1
               && errno == EINTR) {
            wait_time = remaining_time;
        }
        if (retval == -1 && errno != EINTR) {
            perror("nanosleep");
            return 0;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    /* Basic single-threaded test. */
    dart_timer *dart_timer = create_dart_timer();
    struct timeval t1;
    gettimeofday(&t1, 0);
    t1.tv_sec += 5;
    add_dart_timer(dart_timer, t1, dart_timer_callback, "Hello, World!");
    size_t i;
    for (i = 0; i < 25; ++i, sleep(1)) {
        struct timeval now;
        gettimeofday(&now, 0);
        dispatch_dart_timers(dart_timer, now);
    }

    /* More involved multi-threaded test. */
    pthread_t workers[6];
    for (i = 0; i < sizeof workers / sizeof workers[0]; ++i) {
        int retval =
            pthread_create(&workers[i], 0, worker, (void *) dart_timer);
        if (retval != 0) {
            errno = retval;
            perror("spawning worker thread");
            exit(EXIT_FAILURE);
        }
    }
    /* Run for 30 seconds, dispatching dart_timers. */
    time_t end = time(0) + 5;
    struct timeval now;
    for (gettimeofday(&now, 0); now.tv_sec < end; gettimeofday(&now, 0)) {
        dispatch_dart_timers(dart_timer, now);
    }
    for (i = 0; i < sizeof workers / sizeof workers[0]; ++i) {
        void *retptr;
        int retval = pthread_join(workers[i], &retptr);
        if (retval != 0) {
            errno = retval;
            perror("joining thread");
            exit(EXIT_FAILURE);
        }
    }
    return 0;
}
