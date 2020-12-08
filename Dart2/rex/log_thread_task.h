#ifndef __LOG_THREAD_TASK_H__
#define __LOG_THREAD_TASK_H__

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct logging_thread_task {
    void (*task)(void* cb_obj);
    void* cb_obj;
} lt_task;
#if defined(__cplusplus)
}
#endif
#endif
