#ifndef __TRANSACTION_RECORDER_TOOLS_H__
#define __TRANSACTION_RECORDER_TOOLS_H__

#include <stdlib.h>
#include "server_objects/common_structs.h"

/* Some utility functions to use with the transaction recorder's
 * message structure. */

/* Has characteristics of memcpy and strcpy, but some differences as
 * well. The input should not be null-terminated (like memcpy but
 * unlike strcpy), and the output will then be null-terminated (like
 * strcpy, but not memcpy). */
char *memdup_nullterminate(const char *p, size_t n);

/* Make a message structure, malloc'ing the message structure, a copy
 * of the data, and a copy of the name. */
struct message *make_message(const char *name,
                             size_t name_length,
                             const char *data, size_t data_length);

/* Do a deep copy on the message structure, copying the data as
 * well. */
struct message *dup_message(struct message *message);

#endif
