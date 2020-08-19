#include "BATS-Binary-Textualizer.h"
#include "BATS-Binary-Fields.h"
#include "BATS-Binary-Macros.h"
#include "server_objects/databuf.h"
#include "../DataTypes.h"

#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <stdio.h>

#pragma pack(push, 1)

struct Header {
    uint16_t startOfMessage;
    uint16_t messageLength;
    uint8_t messageType;
};

#pragma pack(pop)

static ssize_t totalMessageLength(const struct Header *header)
{
    return header->messageLength + sizeof header->startOfMessage;
}

/* Returns n > 0 as the number of bytes in the payload assuming a
 * complete message is included, implying the parsing is correct and
 * this is a valid message according to the decoder.  A return of 0
 * indicates the message is again so far correct but incomplete.  A
 * return of -1 indicates this is not a message of this protocol and
 * cannot be decoded. */
ssize_t dart_msg_frame(const void *p, size_t n)
{
    const struct Header *header = p;
    if (n < sizeof *header) {
        return 0;
    }
    if (header->startOfMessage != 0xbaba) {
        return -1;
    }
    if (header->messageLength > n) {
        return 0;
    }
    return header->messageLength + sizeof header->startOfMessage;
}

#include "BATS-Binary-BZX-Message-Definitions.incl"

int printText(const struct Field *desc, const void *p, size_t n,
              struct databuf *out)
{
    if (n < desc->nbytes)
        return -1;
    size_t actualLength = strnlen(p, desc->nbytes);
    if (databuf_write(out, "%.*s", (int) actualLength, (const char *) p) ==
        -1)
        return -1;
    return 0;
}

int printBinary(const struct Field *desc, const void *p, size_t n,
                struct databuf *out)
{
    if (n < desc->nbytes)
        return -1;
    int64_t val;
    switch (desc->nbytes) {
    case 1:
        val = *((int8_t *) p);
        break;
    case 2:
        val = *((int16_t *) p);
        break;
    case 4:
        val = *((int32_t *) p);
        break;
    case 8:
        val = *((int64_t *) p);
        break;
    default:
        return -1;
    }
    if (databuf_write(out, "%ld", val) == -1)
        return -1;
    return 0;
}

int printBinaryHex(const struct Field *desc, const void *p, size_t n,
                   struct databuf *out)
{
    if (n < desc->nbytes)
        return -1;
    uint val = 0;
    int ndigits = 2;
    switch (desc->nbytes) {
    case 1:
        val = *((uint8_t *) p);
        break;
    case 2:
        val = *((uint16_t *) p);
        ndigits = 4;
        break;
    case 4:
        val = *((uint32_t *) p);
        ndigits = 8;
        break;
    case 8:
        {
            uint64_t tmp = *((uint64_t *) p);
            /* The gymnastics here is because the hexadecimal conversion
             * specified takes an unsigned int.  If our value would
             * overflow an unsigned int we simply print it out as a
             * decimal int64_t value like we would for printBinary.
             * Otherwise, we truncate it to an unsigned int and print the
             * hexadecimal value of that.  This function really should
             * never be used for a large 64-bit integer. */
            if (tmp > UINT_MAX) {
                if (databuf_write(out, "%ld", tmp) == -1) {
                    return -1;
                } else {
                    return 0;
                }
            }
            val = tmp;
            ndigits = 16;
        }
        break;
    default:
        return -1;
    }
    if (databuf_write(out, "0x%0*x", ndigits, val) == -1)
        return -1;
    return 0;
}

int printFieldValue(const void *p, size_t n, struct databuf *out,
                    const struct Field *field)
{
    int nwritten = -1;
    switch (field->type) {
    case None:
        nwritten = databuf_write(out, "<None>");
        break;
    case Text:
        nwritten = printText(field, p, n, out);
        break;
    case Binary:
        nwritten = printBinary(field, p, n, out);
        break;
    case BinaryPrice:
        {
            price_t prc = *((price_t *) p);
            nwritten = binaryPriceToString(prc, out);
        }
        break;
    case DateTime:
        databuf_reserve(out, 24);
        {
            uint64_t t = *((uint64_t *) p);
            nwritten =
                binaryDateTimeToStr(t, out->wr_ptr,
                                    databuf_space_left(out));
        }
        out->wr_ptr += 24;
        break;
    case BinaryHex:
        nwritten = printBinaryHex(field, p, n, out);
        break;
    default:
        break;
    }
    return nwritten;
}

int printField(const void *p, size_t n, struct databuf *out,
               const struct Field *field)
{
    databuf_write(out, "%s=", field->name);
    int nwritten = printFieldValue(p, n, out, field);
    databuf_write(out, ";");
    return nwritten;
}

int printFixedFields(const void *p, size_t n, struct databuf *out,
                     const struct Field *pfields, size_t nfields)
{
    size_t i;
    const void *begin = p;
    for (i = 0; i < nfields; ++i) {
        if (i != 0)
            databuf_write(out, " ");
        int nwritten = printField(p, n, out, &pfields[i]);
        if (nwritten == -1)
            databuf_write(out, "<error converting field to text>");
        if (n < pfields[i].nbytes)
            return -1;
        p += pfields[i].nbytes;
        n -= pfields[i].nbytes;
    }
    return p - begin;
}

uint8_t rotate_left(uint8_t a, uint8_t shift)
{
    return a << shift | a >> (sizeof(a) * 8 - shift);
}

int printOptionalFieldBitmaps(const void *p, size_t n, struct databuf *out,
                              const struct Field *pfields, size_t nfields)
{
    uint8_t bitmask = 1;
    uint nflagbytes = nfields / 8;
    if (nfields % 8 != 0)
        ++nflagbytes;
    size_t byte, bitcount, nflagsInByte;
    for (byte = 0, bitcount = 0, nflagsInByte = 0; byte < nflagbytes;
         ++byte, nflagsInByte = 0) {
        databuf_write(out, "Optional field byte %zu: 0x%x",
                      byte, (int) (((char *) p)[byte]));
        size_t i;
        for (i = 0; i < 8;
             ++i, ++bitcount, bitmask = rotate_left(bitmask, 1)) {
            if ((((uint8_t *) p)[byte]) & bitmask) {
                if (nflagsInByte > 0)
                    databuf_write(out, "|");
                databuf_write(out, "%s(%d) ", pfields[bitcount].name,
                              (int) bitmask);
            }
        }
        databuf_write(out, "; ");
    }
    return nflagbytes;
}

size_t countOptionalBitmaskBytes(const struct Field * pfields,
                                 size_t nfields)
{
    size_t i, nbits, nbytes;
    for (i = 0, nbits = 0; i < nfields; ++i) {
        if (pfields[i].type != None) {
            ++nbits;
        } else {
            nbits += pfields[i].nbytes;
        }
    }
    nbytes = nbits / 8;
    if (nbits % 8 != 0) {
        ++nbytes;
    }
    return nbytes;
}

int printOptionalFieldPayloads(const void *p, size_t n,
                               struct databuf *out,
                               const struct Field *pfields, size_t nfields)
{
    size_t nflagbytes = countOptionalBitmaskBytes(pfields, nfields);
    uint8_t bitmask = 1;
    const uint8_t *bitfield, *bitfieldend;
    const char *ppayload = p + nflagbytes;
    const void *end = p + n;

    size_t optfield, nreserved;
    /* This outer loop iterates over every bitfield byte. */
    for (optfield = 0, nreserved = 0, bitfield = p, bitfieldend =
         p + nflagbytes; bitfield < bitfieldend; ++bitfield) {
        if (*bitfield == 0) {
            /* No flags for this byte, skip and go to the next. */
            continue;
        }
        databuf_write(out, " Opt field byte %zu: ",
                      bitfield - (uint8_t *) p);

        size_t bit;

        /* This inner loop iterates over every bit within the current
           bitfield byte. */
        for (bit = 0; bit < 8 && optfield < nfields;
             ++bit, bitmask = rotate_left(bitmask, 1)) {
            if (nreserved == 0 && pfields[optfield].type == None) {
                /* None types are placeholders.  The nbytes field
                   indicates how many bits they represent in the
                   bitfield.  They simply allow a shorthand to specify
                   large numbers of contiguous Reserved bits easily
                   without having to specify an entry for each one.
                   Once encountered, nreserved is set to 1 and is
                   incremented for each bit in the bitfield up to
                   nbytes as specified in the Field structure for this
                   particular contiguous set of bits.  optfields is
                   not incremented because there is only one
                   descriptor Field structure for all of these
                   contiguous bits.  */
                nreserved = 1;
            }
            if (*bitfield & bitmask) {
                databuf_write(out, "%s(%d)=", pfields[optfield].name,
                              (int) bitmask);
                printFieldValue(ppayload, end - p, out,
                                &pfields[optfield]);
                if (pfields[optfield].type != None) {
                    ppayload += pfields[optfield].nbytes;
                }
                databuf_write(out, "; ");
            }
            if (nreserved != 0) {
                /* nreserved is counting the current iteration of
                   reserved bitfields. Once we reach the number of
                   reserved bits we can then increment the
                   optfield. */
                if (++nreserved > pfields[optfield].nbytes) {
                    nreserved = 0;
                    ++optfield;
                }
            } else {
                ++optfield;
            }
        }
    }
    return nflagbytes;
}

int messageToText(const void *p, size_t n, struct databuf *out,
                  const struct Message *msgdef)
{
    const char *start = p;
    int nconsumed = printFixedFields(p, n, out, msgdef->pfixed,
                                     msgdef->nfixed);
    if (nconsumed != -1) {
        p += nconsumed;
        n -= nconsumed;
        if (msgdef->hasOptFieldFlagBytesOnly == 0) {
            nconsumed =
                printOptionalFieldPayloads(p, n, out, msgdef->popt,
                                           msgdef->nopt);
        } else {
            nconsumed =
                printOptionalFieldBitmaps(p, n, out, msgdef->popt,
                                          msgdef->nopt);
        }
        if (nconsumed != -1) {
            p += nconsumed;
            n -= nconsumed;
        }
    }
    return ((const char *) p) - start;
}

int dart_msg_tostr(const void *p, size_t n, databuf_t * out)
{
    const struct Header *header = p;
    if (n < sizeof *header || n < totalMessageLength(header)) {
        return -1;
    }
    size_t i;
    for (i = 0; i < narray(messages); ++i) {
        if (messages[i].type == header->messageType) {
            return messageToText(p, n, out, &messages[i]);
        }
    }
    const char *bytes = p;
    for (i = 0; i < n; ++i) {
        int c = bytes[i];
        c = c & 0xff;
        databuf_write(out, "%s%02x", i == 0 ? "" : " ", c);
    }
    return 0;
}
