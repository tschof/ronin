#ifndef __PERRCHK_H__
#define __PERRCHK_H__

#define perrchk(op, label)                                              \
    do {                                                                \
        int result = (op);                                              \
        if (result != 0) {                                              \
            const char *errormessage = strerror(result);                \
            if (errormessage == 0) {                                    \
                errormessage = "Unknown";                               \
            }                                                           \
            send_debug_message                                          \
                ("%s:%d (%s) operation: '%s' yielded error code %d:%s", \
                 __FILE__, __LINE__, __FUNCTION__, #op, result,         \
                 errormessage);                                         \
            goto label;                                                 \
        }                                                               \
    } while (0)

#endif
