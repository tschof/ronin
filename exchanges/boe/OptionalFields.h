#ifndef __OPTIONAL_FIELDS_H__
#define __OPTIONAL_FIELDS_H__

#include "server_objects/dart_obj_factory.h"
#include "server_objects/databuf.h"

#include <stdint.h>
#include <stdlib.h>

struct iovec;
struct databuf;

#pragma pack(push,1)

struct OptionalFields {
    uint8_t *bitsp;
    size_t bitsn;
    struct iovec *iovp;
    size_t iovn;
    dof_t *databufFactory;
    struct databuf *buffers, *currentBuffer;
};

#pragma pack(pop)

void initOptionalFields(struct OptionalFields *, uint8_t *, size_t,
                        struct iovec *, size_t, dof_t *);
int setOptionalField(struct OptionalFields *, unsigned int, void *,
                     size_t);
int clearOptionalField(struct OptionalFields *, unsigned int);
int setOptionalAlphaNumeric(struct OptionalFields *, unsigned int,
                            const char *, size_t, size_t);
int setOptionalBinary(struct OptionalFields *, unsigned int, const char *,
                      size_t, size_t);
int setOptionalPrice(struct OptionalFields *, unsigned int, const char *,
                     size_t);
size_t sumOptionalFieldLength(const struct OptionalFields *optfields);
void dumpOptionalFieldBits(const struct OptionalFields *optfields,
                           struct databuf *buf);
void destroyOptionalFields(const struct OptionalFields *);

#ifdef BAZTEST
void printBytesAndBits(void);
#endif

#endif
