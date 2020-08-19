#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#include "suffixmap.h"

typedef struct suffixpair suffixpair_t; /* Defined below */

/* suffixmap definition
 * The suffixmap is purposefully kept opaque in the header and should only be
 * manipulated via the functions provided in this compilation unit.
 */
struct suffixmap {
    char *filedata;
    suffixpair_t *mappings;
    size_t nmappings;
    size_t nlongestsuffix;
    char *tmpsuffixbuf;
};

struct suffixpair {
    const char *pkey;
    const char *pval;
};

/* Internal functions */
static int loadfile(const char *filename, char **buf, size_t * buflen);
static int indexkeys(char *buffer, size_t buflen, suffixpair_t ** mappings,
                     size_t * nkeys, size_t * nlongestsuffix);
static int cmpsuffixpair(const void *p1, const void *p2);

/* suffixmap_t interface functions */
int buildsuffixmap(suffixmap_t ** map, const char *datafile)
{
    int result = -1;
    if (map != 0 && datafile != 0) {
        *map = malloc(sizeof(suffixmap_t));
        if (*map) {
            suffixmap_t *pmap = *map;
            size_t filelen = 0;
            memset(pmap, 0, sizeof(suffixmap_t));
            if (loadfile(datafile, &(pmap->filedata), &filelen) == 0
                && indexkeys(pmap->filedata, filelen,
                             &(pmap->mappings), &(pmap->nmappings),
                             &(pmap->nlongestsuffix)) == 0) {
                pmap->tmpsuffixbuf = malloc(pmap->nlongestsuffix + 1);
                if (pmap->tmpsuffixbuf) {
                    /* Sort the array */
                    qsort(pmap->mappings, pmap->nmappings,
                          sizeof(suffixpair_t), cmpsuffixpair);
                    result = 0;
                }
            }
            if (result == -1) {
                destroysuffixmap(*map);
            }
        }
    } else {
        errno = EINVAL;
    }

    return result;
}

int lookupsuffix(suffixmap_t * map, const char *suffix, const char **val)
{
    int result = -1;
    if (map != 0 && suffix != 0 && suffix[0] != '\0' && val != 0) {
        suffixpair_t tmppair;
        void *psearchres = 0;
        tmppair.pkey = suffix;
        tmppair.pval = 0;
        psearchres = bsearch(&tmppair, map->mappings, map->nmappings,
                             sizeof(suffixpair_t), cmpsuffixpair);
        result = 0;
        if (psearchres) {
            *val = ((suffixpair_t *) psearchres)->pval;
        }
    } else {
        result = EINVAL;
    }
    return result;
}

int lookupsuffix2(suffixmap_t * map, const char *suffix, size_t suffixlen,
                  const char **val)
{
    int result = -1;
    if (map != 0 && suffix != 0 && suffixlen != 0 && suffix[0] != '\0'
        && val != 0) {
        /* nlongestsuffix is pre-calculated, no sense in looking the suffix up
           if we know it's bigger than any suffix we have in the map as a key.
           This also ensures our pre-allocated buffer is always big enough. */
        if (suffixlen <= map->nlongestsuffix) {
            suffixpair_t tmppair;
            void *psearchres = 0;
            /* Since all of the bsearch keys depend on a null-terminated string,
               and this function doesn't necessarily take a null-terminated
               string, we will copy the suffix to be looked up into a temporary
               buffer. */
            memcpy(map->tmpsuffixbuf, suffix, suffixlen);
            map->tmpsuffixbuf[suffixlen] = '\0';
            tmppair.pkey = map->tmpsuffixbuf;
            tmppair.pval = 0;
            psearchres = bsearch(&tmppair, map->mappings, map->nmappings,
                                 sizeof(suffixpair_t), cmpsuffixpair);
            result = 0;
            if (psearchres) {
                *val = ((suffixpair_t *) psearchres)->pval;
            }
        }
    } else {
        result = EINVAL;
    }
    return result;
}

void destroysuffixmap(suffixmap_t * map)
{
    if (map) {
        free(map->filedata);
        free(map->mappings);
        free(map->tmpsuffixbuf);
        free(map);
    }
}

static int loadfile(const char *filename, char **buf, size_t * buflen)
{
    int result = -1;
    if (filename != 0 && buf != 0 && buflen != 0) {
        FILE *f = fopen(filename, "rb");
        if (f) {
            if (fseek(f, 0, SEEK_END) == 0) {
                long fsize = ftell(f);
                if (fsize > -1) {
                    *buflen = (size_t) fsize;
                    if (fseek(f, 0, SEEK_SET) == 0) {
                        *buf = malloc((*buflen) + 1);
                        if (*buf != 0) {
                            if (fread(*buf, 1, *buflen, f) == *buflen) {
                                (*buf)[(*buflen)] = '\0';
                                result = 0;
                            } else {
                                /* Short read */
                                free(*buf);
                                *buf = 0;
                            }
                        }
                    }
                }
            }
            (void) fclose(f);
        }
    } else {
        errno = EINVAL;
    }
    return result;
}

#define STARTOFLINE 0
#define STARTOFKEY  1
#define INKEY       2
#define KEYVALSEP   3
#define INVAL       4
#define RESTOFLINE  5
#define LINETERM    6

static int
indexkeys(char *buffer, size_t buflen, suffixpair_t ** mappings,
          size_t * nkeys, size_t * nlongestsuffix)
{
    int result = 0;
    if (buffer != 0 && buflen != 0 && mappings != 0 && nkeys != 0) {
        size_t nalloc = 256;
        int state = STARTOFLINE;
        const char *end = buffer + buflen;
        const char *pkey = 0;
        const char *pval = 0;
        char *p = 0;
        *mappings = malloc(sizeof(suffixpair_t) * nalloc);
        *nkeys = 0;
        if ((*mappings) == 0) {
            goto INDEXERROR;
        }

        for (p = buffer; p < end; ++p) {
            switch (state) {
            case STARTOFLINE:
                if (pkey != 0 || pval != 0) {
                    if ((*nkeys) >= nalloc) {
                        nalloc *= 2;
                        *mappings = realloc(*mappings,
                                            sizeof(suffixpair_t) * nalloc);
                        if ((*mappings) == 0) {
                            goto INDEXERROR;
                        }
                    }
                    if (nlongestsuffix != 0) {
                        const size_t nlen = strlen(pkey);
                        if (nlen > *nlongestsuffix) {
                            *nlongestsuffix = nlen;
                        }
                    }
                    (*mappings)[*nkeys].pkey = pkey;
                    (*mappings)[*nkeys].pval = pval;
                    ++(*nkeys);
                }
                pkey = pval = 0;
                if (*p == '<')
                    state = STARTOFKEY;
                break;
            case STARTOFKEY:
                pkey = p;
                state = INKEY;
                break;
            case INKEY:
                if (*p == '=') {
                    state = KEYVALSEP;
                    *p = '\0';
                }
                break;
            case KEYVALSEP:
                pval = p;
                state = INVAL;
                break;
            case INVAL:
                if (*p == '>') {
                    state = RESTOFLINE;
                    *p = '\0';
                }
                break;
            case RESTOFLINE:
                if (*p == '\r' || *p == '\n')
                    state = LINETERM;
                break;
            case LINETERM:
                if (*p != '\r' && *p != '\n') {
                    state = STARTOFLINE;
                    --p;
                }
                break;
            default:
                /* Unknown, horrible error */
                goto INDEXERROR;
            }
        }
    } else {
        errno = EINVAL;
    }

  INDEXERROR:

    return result;
}

size_t nfailures = 0;

int cmpsuffixpair(const void *p1, const void *p2)
{
    suffixpair_t *left = (suffixpair_t *) p1;
    suffixpair_t *right = (suffixpair_t *) p2;
    return strcmp(left->pkey, right->pkey);
}
