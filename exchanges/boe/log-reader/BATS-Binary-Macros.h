#ifndef __BATS_BINARY_MACROS_H__
#define __BATS_BINARY_MACROS_H__

#include <stdint.h>

enum DataType { None, Text, Binary, BinaryHex, BinaryPrice, DateTime };

struct Field {
    enum DataType type;
    size_t nbytes;
    const char *name;
};

struct Message {
    uint8_t type;
    const struct Field *pfixed;
    size_t nfixed;
    const struct Field *popt;
    size_t nopt;
    uint hasOptFieldFlagBytesOnly;
};

#define narray(array) sizeof (array) / sizeof (array)[0]

#define FIXED_FIELDS_START(msgname) static const struct Field msgname##FixedFields[] = {
#define FIXED_FIELDS_END };

#define OPT_FIELDS_START(msgname) static const struct Field msgname##OptFields[] = {
#define OPT_FIELDS_END };

#define DECL_MSG(typenum, msgname)                  \
    {                                               \
        .type = typenum,                            \
        .pfixed = msgname##FixedFields,             \
        .nfixed = narray(msgname##FixedFields),     \
        .popt = msgname##OptFields,                 \
        .nopt = narray(msgname##OptFields),         \
        .hasOptFieldFlagBytesOnly = 0,              \
    },

#define DECL_MSG_OPT_FLAGS_ONLY(typenum, msgname)   \
    {                                               \
        .type = typenum,                            \
        .pfixed = msgname##FixedFields,             \
        .nfixed = narray(msgname##FixedFields),     \
        .popt = msgname##OptFields,                 \
        .nopt = narray(msgname##OptFields),         \
        .hasOptFieldFlagBytesOnly = 1,              \
    },

#define DECL_MSG_NO_OPT(typenum, msgname)           \
    {                                               \
        .type = typenum,                            \
        .pfixed = msgname##FixedFields,             \
        .nfixed = narray(msgname##FixedFields),     \
        .popt = 0,                                  \
        .nopt = 0,                                  \
        .hasOptFieldFlagBytesOnly = 1,              \
    },

#define MESSAGES_BEGIN static const struct Message messages[] = {
#define MESSAGES_END };


#endif
