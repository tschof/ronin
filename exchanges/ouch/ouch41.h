#ifndef __OUCH41_H__
#define __OUCH41_H__

#include "dump_data.h"

#include <stdlib.h>
#include <stdint.h>

int ouch__timestamp_to_string(uint64_t ouch_timestamp, char *buffer,
                              size_t buffer_length);

/* Required because OUCH breaks alignment starting at
 * minimum_quantity. */
#pragma pack(push, 1)

/* From OUCH */

#define ouch41__message_begin(name, typecode)   \
    struct ouch41__##name {
#define ouch41__message_entry_scalar(type, name)    \
    type name;
#define ouch41__msg_element_char(name)          \
    char name;
#define ouch41__msg_element_uint32(name)        \
    uint32_t name;
#define ouch41__msg_element_uint64(name)        \
    uint64_t name;
#define ouch41__msg_element_char_array(name, arity) \
    char name[arity];
#define ouch41__message_end                     \
    };

#include "ouch41__message_definitions.h"

#undef ouch41__message_begin
#undef ouch41__msg_element_char
#undef ouch41__msg_element_uint32
#undef ouch41__msg_element_uint64
#undef ouch41__msg_element_char_array
#undef ouch41__message_end

#pragma pack(pop)

struct dart_order_obj;
int ouch41__csv_to_ouch_enter_order(struct dart_order_obj *csv,
                                    struct ouch41__enter_order *order,
                                    const char **errmsg);
int dart_csv__to__ouch41__replace_order(struct dart_order_obj
                                        *incoming_csv,
                                        struct dart_order_obj *book_order,
                                        struct ouch41__replace_order
                                        *replace, const char **errmsg);

/* Use this to dump a SoupBin TCP packet containing an OUCH 4.1
 * message.  The pointer should be to a fully-formed SoupBin TCP
 * packet.  The last void pointer is a function pointer to a function
 * of the same signature as this one. */
int ouch41__print_soupbin_encapsulated_message(void *, size_t,
                                               struct databuf *, void *);
int ouch41__print_message(void *p, size_t n, struct databuf *logbuf);

/* Serialize an OUCH 4.1 message to this databuf */
int ouch41__serialize(void *, size_t, struct databuf *);
int ouch41__deserialize(void *, size_t);

int ouch41__dump_message(void *, size_t, struct databuf *);

int ouch41__reject_reason_code_to_string(struct databuf *, char);
int ouch41__cancel_reason_code_to_string(struct databuf *, char);

#define OUCH_TIF_IOC 0
#define OUCH_TIF_MARKET_HOURS 99998
#define OUCH_TIF_SYSTEM_HOURS 99999
#define OUCH_TIF_DAY OUCH_TIF_SYSTEM_HOURS
#define OUCH_PRICE_MARKET 0x7fffffff

#define ouch41__message_begin(structure, typecode)                      \
    void ouch41__##structure##__init(struct ouch41__##structure *message);
#define ouch41__msg_element_char(field)
#define ouch41__msg_element_uint32(field)
#define ouch41__msg_element_uint64(field)
#define ouch41__msg_element_char_array(field, arity)
#define ouch41__message_end
#include "ouch41__message_definitions.h"
#undef ouch41__message_begin
#undef ouch41__msg_element_char
#undef ouch41__msg_element_uint32
#undef ouch41__msg_element_uint64
#undef ouch41__msg_element_char_array
#undef ouch41__message_end

#endif
