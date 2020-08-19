#ifndef __MACROS_H__
#define __MACROS_H__

#define jumpassert(condition, label)            \
    if (!(condition)) {                         \
        goto label;                             \
    }

/* #define csvptr(indexed_message_ptr, iterator_ptr)   \ */
/*     ((indexed_message_ptr)->buffer->buffer + (iterator_ptr)->field->offset) */
/* #define csvlen(iterator_ptr) ((iterator_ptr)->field->length) */
#define jump_on_fail(cond, label, msgvar, msgstr)           \
    if (!(cond)) {                                          \
        (msgvar) = strdup((msgstr));                        \
        goto label;                                         \
    }

#define csvptr(obj, field)                                  \
    ((obj)->data->buffer + (obj)->positions[(field)].beg)

#define csvlen(obj, field)                      \
    (obj)->positions[(field)].val_len

#endif
