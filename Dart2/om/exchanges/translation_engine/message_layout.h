#ifndef __MESSAGE_LAYOUT_H__
#define __MESSAGE_LAYOUT_H__

#include <stdlib.h>

/* The message_layout_t is an extremely simple representation of the
 * ordering of fields in a message.  We assume that all messages we
 * are dealing with have discrete fields that are uniquely numbered.
 * CSV messages are uniqely identified by their zero-based index, so
 * the layout would simply be 0-n in order.  FIX messages are
 * identified by a tag and would be represented by an ordered sequence
 * of tag numbers.  The layout does not care.  This only identifies
 * the ordering of the numbers currently.  Eventually this could be
 * fleshed out to include things like whether a field is required or
 * not (so reject if it is not found), or validation rules, or
 * rejection rules for various fields based on their value.  All of
 * this could be run while processing individual fields or as a final
 * step before sending out.  On an input message we could eventually
 * specify attributes for fields such as whether they should be saved
 * to the book, or whether the value on the book can be used in
 * subsequent messages, etc. */

typedef struct message_layout {
    size_t *ordered_fields;
    size_t num, capacity;
} message_layout_t;

message_layout_t *make_message_layout(size_t *, size_t);
message_layout_t *make_empty_message_layout(void);
int append_to_message_layout(message_layout_t *, size_t);

#endif
