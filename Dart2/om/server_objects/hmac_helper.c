#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <openssl/evp.h>
#include <openssl/hmac.h>

#include "hmac_helper.h"
/* yuck but it's fast and const should make it a shared text page. */
static const uint8_t pr2six[256] =
{
    /* ASCII table */
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 62, 64, 64, 64, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 64, 64, 64, 64, 64, 64,
    64,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 64, 64, 64, 64, 64,
    64, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64,
    64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64, 64
};

static int base_64_decode_len(const char *bufcoded)
{
    int nbytesdecoded;
    register const uint8_t *bufin;
    register int nprbytes;

    bufin = (const uint8_t *) bufcoded;
    while (pr2six[*(bufin++)] <= 63);

    nprbytes = (bufin - (const uint8_t *) bufcoded) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    return nbytesdecoded + 1;
}

static int base_64_decode(char *bufplain, const char *bufcoded)
{
    int nbytesdecoded;
    register const uint8_t *bufin;
    register uint8_t *bufout;
    register int nprbytes;

    bufin = (const uint8_t *) bufcoded;
    while (pr2six[*(bufin++)] <= 63);
    nprbytes = (bufin - (const uint8_t *) bufcoded) - 1;
    nbytesdecoded = ((nprbytes + 3) / 4) * 3;

    bufout = (uint8_t *) bufplain;
    bufin = (const uint8_t *) bufcoded;

    while (nprbytes > 4) {
        *(bufout++) =
            (uint8_t) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
        *(bufout++) =
            (uint8_t) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
        *(bufout++) =
            (uint8_t) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
        bufin += 4;
        nprbytes -= 4;
    }

    /* Note: (nprbytes == 1) would be an error, so just ingore that case */
    if (nprbytes > 1) {
        *(bufout++) =
            (uint8_t) (pr2six[*bufin] << 2 | pr2six[bufin[1]] >> 4);
    }
    if (nprbytes > 2) {
        *(bufout++) =
            (uint8_t) (pr2six[bufin[1]] << 4 | pr2six[bufin[2]] >> 2);
    }
    if (nprbytes > 3) {
        *(bufout++) =
            (uint8_t) (pr2six[bufin[2]] << 6 | pr2six[bufin[3]]);
    }

    *(bufout++) = '\0';
    nbytesdecoded -= (4 - nprbytes) & 3;
    return nbytesdecoded;
}

static const char basis_64[] =
"ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static size_t base_64_encode_len(size_t len)
{
    return ((len + 2) / 3 * 4) + 1;
}

static int base_64_encode(uint8_t *encoded, const uint8_t *string, int len)
{
    int i;
    uint8_t *p;

    p = encoded;
    for (i = 0; i < len - 2; i += 3) {
        *p++ = basis_64[(string[i] >> 2) & 0x3F];
        *p++ = basis_64[((string[i] & 0x3) << 4) |
            ((int) (string[i + 1] & 0xF0) >> 4)];
        *p++ = basis_64[((string[i + 1] & 0xF) << 2) |
            ((int) (string[i + 2] & 0xC0) >> 6)];
        *p++ = basis_64[string[i + 2] & 0x3F];
    }
    if (i < len) {
        *p++ = basis_64[(string[i] >> 2) & 0x3F];
        if (i == (len - 1)) {
            *p++ = basis_64[((string[i] & 0x3) << 4)];
            *p++ = '=';
        } else {
            *p++ = basis_64[((string[i] & 0x3) << 4) |
                ((int) (string[i + 1] & 0xF0) >> 4)];
            *p++ = basis_64[((string[i + 1] & 0xF) << 2)];
        }
        *p++ = '=';
    }

    *p++ = '\0';
    return p - encoded;
}

size_t base64url_encode_alloc (const uint8_t *data, size_t len, uint8_t **out)
{
    size_t i,s;
    uint8_t *burl;

    s = base_64_encode_len(len);
    burl = malloc (s+1);
    memset (burl, 0, s+1);
    base_64_encode(burl, data, len);
    *out = burl;
    for (i = 0; i < s; i++) {
        if ('+' == *(burl+i))
            *(burl+i) = '-';
        else if ('/' == *(burl+i))
            *(burl+i) = '_';
        else if ('=' == *(burl+i))
            *(burl+i) = 0;
    }

    return strnlen ((char*)burl, s);
}


size_t base64url_decode_alloc (const uint8_t *data, size_t l, uint8_t **out)
{
    size_t i, s, pad;
    uint8_t *burl;
    size_t len;


    len = strnlen ((const char *)data, l);

    pad = len + (4 - len % 4) % 4;


    burl = malloc (pad + 1);
    memset (burl, 0, pad + 1);

    memcpy (burl, (const char *)data, len);

    for (i = 0; i < len; i++) {
        if ('-' == *(burl+i))
            *(burl+i) = '+';
        else if ('_' == *(burl+i))
            *(burl+i) = '/';
    }

    for (i = 0; i < (pad - len); i++) {
        burl[len + i] = '=';
    }
    int decode_len = base_64_decode_len((const char*)burl);
    char *tmp = malloc (decode_len);

    int dl = base_64_decode(tmp, (const char*)burl);
    *out = (uint8_t *)tmp;
    s = dl;
    free (burl);

    return s;
}



int encode_base64 (const uint8_t *to_enc, int inlen, char** out)
{

    size_t result = base64url_encode_alloc (to_enc, (size_t)inlen, (uint8_t**)out);
    return (int)result;
}

int decode_base64 (const uint8_t *text, int out_len, char** out)
{
    size_t ol = base64url_decode_alloc (text, out_len, (uint8_t **)out);
    return (int)ol;

}

uint8_t* dart_hmac_sha256(const void* key, int keylen,
        const uint8_t* data, size_t datalen, uint8_t* result,
        uint32_t* resultlen)
{
    return HMAC(EVP_sha256(), key, keylen,
            data, datalen, result,resultlen);
}
