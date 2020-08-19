#include "OptionalFields.h"
#include "server_objects/databuf.h"
#include "DataTypes.h"

#include <ctype.h>
#include <errno.h>
#include <limits.h>
#include <sys/uio.h>
#include <string.h>
#include <stdio.h>

void initOptionalFields(struct OptionalFields *optFields, uint8_t * pbits,
                        size_t nbits, struct iovec *piovec, size_t niovec,
                        dof_t * databufFactory)
{
    memset(pbits, 0, nbits * sizeof pbits[0]);
    optFields->bitsp = pbits;
    optFields->bitsn = nbits;
    memset(piovec, 0, niovec * sizeof piovec[0]);
    optFields->iovp = piovec;
    optFields->iovn = niovec;
    optFields->databufFactory = databufFactory;
    optFields->buffers = optFields->currentBuffer = 0;
}

#define idtobyte(id) ((id) / 8)
#define idtobit(id) (1 << ((id) % 8))

int setOptionalField(struct OptionalFields *optionalFields,
                     unsigned int id, void *p, size_t n)
{
    int result = -1;
    size_t byte = idtobyte(id);
    size_t bit = idtobit(id);
    if (p != 0 && n > 0 && byte < optionalFields->bitsn
        && id < optionalFields->iovn) {
        optionalFields->bitsp[byte] |= bit;
        optionalFields->iovp[id].iov_base = p;
        optionalFields->iovp[id].iov_len = n;
        result = 0;
    }
    return result;
}

int clearOptionalField(struct OptionalFields *optionalFields,
                       unsigned int id)
{
    int result = -1;
    size_t byte = idtobyte(id), bit = idtobit(id);
    if (byte < optionalFields->bitsn && id < optionalFields->iovn) {
        optionalFields->bitsp[byte] &= ~bit;
        optionalFields->iovp[id].iov_base = 0;
        optionalFields->iovp[id].iov_len = 0;
    }
    return result;
}

/* The OptionalFields uses one or more databufs to store the data
 * where the iovec structures point.  Since the iovec structures store
 * the actual pointer where the data resides, the databufs cannot be
 * allowed to realloc when writing data into them, so we set the
 * norealloc bit in them.  Since they cannot now realloc, and to make
 * this a rather general solution, we use the concept of a linked list
 * of databufs in which the first sizeof(void*) bytes at the rd_ptr
 * represent the pointer to the next databuf in the chain.  Initially
 * this pointer is zero, but as we allocate databufs for use we can
 * link them together in this fashion. */
static struct databuf *getAndPrepDatabuf(dof_t * factory, size_t natleast)
{
    struct databuf *retval = get_obj(factory);
    if (retval != 0) {
        databuf_reset(retval);
        struct databuf *next = 0;
        natleast += sizeof next;
        if (databuf_reserve(retval, natleast) != -1) {
            databuf_setnorealloc(retval);
            if (databuf_memcpy(retval, &next, sizeof next) == -1) {
                /* Something went horribly wrong, do not properly
                   dispose of the databuf and return 0 to indicate an
                   error. */
                retval = 0;
            }
            struct databuf *tmp = 0;
            memcpy(&tmp, retval->buffer, sizeof tmp);
            if (tmp != next) {
                retval = 0;
            }
        } else {
            destroy_elem(factory, retval);
            retval = 0;
        }
    }
    return retval;
}

int ensureSpace(struct OptionalFields *optfields, size_t nrequired)
{
    if (optfields->currentBuffer == 0
        || databuf_space_left(optfields->currentBuffer) < nrequired) {
        struct databuf *next =
            getAndPrepDatabuf(optfields->databufFactory, nrequired);
        if (next == 0) {
            return -1;
        }
        if (optfields->buffers == 0) {
            optfields->buffers = optfields->currentBuffer = next;
        } else {
            memcpy(optfields->currentBuffer->buffer, &next, sizeof next);
            optfields->currentBuffer = next;
        }
    }
    return 0;
}

int setOptionalAlphaNumeric(struct OptionalFields *optfields,
                            unsigned int id, const char *pfrom,
                            size_t nfrom, size_t fieldSize)
{
    int retval = -1;
    if (nfrom <= fieldSize && ensureSpace(optfields, fieldSize) != -1) {
        setAlphaNumeric(optfields->currentBuffer->wr_ptr, pfrom, nfrom,
                        fieldSize);
        retval =
            setOptionalField(optfields, id,
                             optfields->currentBuffer->wr_ptr, fieldSize);
        optfields->currentBuffer->wr_ptr += fieldSize;
    }
    return retval;
}

int setOptionalBinary(struct OptionalFields *optfields, unsigned int id,
                      const char *pfrom, size_t nfrom, size_t nbytes)
{
    int retval = -1;
    if (ensureSpace(optfields, nbytes) != -1
        && setBinaryNumFromStr(optfields->currentBuffer->wr_ptr, pfrom,
                               nfrom, nbytes) != -1) {
        retval =
            setOptionalField(optfields, id,
                             optfields->currentBuffer->wr_ptr, nbytes);
        optfields->currentBuffer->wr_ptr += nbytes;
    }
    return retval;
}

int setOptionalPrice(struct OptionalFields *optfields, unsigned int id,
                     const char *pfrom, size_t nfrom)
{
    int retval = -1;
    if (ensureSpace(optfields, 8) != -1
        && setSignedBinaryPrice(optfields->currentBuffer->wr_ptr, pfrom,
                                nfrom) != -1) {
        retval =
            setOptionalField(optfields, id,
                             optfields->currentBuffer->wr_ptr, 8);
        optfields->currentBuffer->wr_ptr += 8;
    }
    return retval;
}

size_t sumOptionalFieldLength(const struct OptionalFields * optfields)
{
    int i;
    size_t nbytes = 0;
    for (i = 0; i < optfields->iovn; ++i)
        nbytes += optfields->iovp[i].iov_len;
    return nbytes;
}

void dumpIndividualOptionalFieldData(struct iovec *iovp,
                                     struct databuf *buf)
{
    if (iovp->iov_base != 0 && iovp->iov_len != 0) {
        size_t i;
        for (i = 0; i < iovp->iov_len; ++i)
            databuf_write(buf, "%02x ", ((char *) (iovp->iov_base))[i]);
        databuf_write(buf, "'");
        for (i = 0; i < iovp->iov_len; ++i)
            if (isprint(((char *) iovp->iov_base)[i]) != 0)
                databuf_write(buf, "%c", ((char *) iovp->iov_base)[i]);
            else
                databuf_write(buf, ".");
        databuf_write(buf, "'");
    }
}

void dumpIndividualOptionalField(size_t byte, size_t iovindex, int bitmask,
                                 const struct OptionalFields *optfields,
                                 struct databuf *buf)
{
    databuf_write(buf, "\tBit %3d: %d ", bitmask,
                  (optfields->bitsp[byte] & bitmask) != 0);
    dumpIndividualOptionalFieldData(&optfields->iovp[iovindex], buf);
    databuf_write(buf, "\n");
}

uint8_t rotate_left(uint8_t a, uint8_t shift)
{
    return a << shift | a >> (sizeof(a) * 8 - shift);
}

void dumpOptionalFieldBits(const struct OptionalFields *optfields,
                           struct databuf *buf)
{
    uint8_t bitmask;
    size_t i;
    for (i = 0, bitmask = 1; i < optfields->iovn;
         ++i, bitmask = rotate_left(bitmask, 1)) {
        if (i % 8 == 0)
            databuf_write(buf, "Byte %zu\n", i / 8);
        dumpIndividualOptionalField(i / 8, i, bitmask, optfields, buf);
    }
}

/* The main thing to do here is just to hand back all of the databufs
 * back to the databuf factory. */
void destroyOptionalFields(const struct OptionalFields *optfields)
{
    if (optfields->buffers == 0) {
        return;
    }
    struct databuf *buf = optfields->buffers;
    while (buf != 0) {
        struct databuf *next = 0;
        memcpy(&next, buf->buffer, sizeof next);
        databuf_setrealloc(buf);
        destroy_elem(optfields->databufFactory, buf);
        buf = next;
    }
}

#ifdef BAZTEST

#include "../baz/libbaz.h"

void printBytesAndBits(void)
{
    int i;
    printf("\n");
    for (i = 1; i < 49; ++i)
        printf("%d: byte %d, bit %d\n", i, idtobyte(i), idtobit(i));
}

#endif
