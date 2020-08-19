#include "transaction_recorder_tools.h"
#include <string.h>

char *memdup_nullterminate(const char *p, size_t n)
{
    char *retval = malloc(n + 1);
    memcpy(retval, p, n);
    retval[n] = '\0';
    return retval;
}

struct message *make_message(const char *name,
                             size_t name_length,
                             const char *data, size_t data_length)
{
    struct message *retval = malloc(sizeof *retval);
    retval->name = memdup_nullterminate(name, name_length);
    retval->name_len = name_length;
    retval->data = memdup_nullterminate(data, data_length);
    retval->len = data_length;
    return retval;
}

struct message *dup_message(struct message *message)
{
    return make_message(message->name, message->name_len, message->data,
                        message->len);
}
