#include "destmap.h"
#include <stdlib.h>
#include <string.h>

struct destmap_entry {
    /*@dependent@ */ char *destid;
    /*@dependent@ */ char *exdest;
    /*@owned@ *//*@null@ */ struct destmap_entry *next;
};

static const struct destmap_entry DESTMAP_ENTRY_STATIC_INITIALIZER = { 0 };

struct destmap_t {
    /*@owned@ */ char *buffer;
    /*@only@ */ struct destmap_entry *list;
};

static
                                    /*@out@ */
 /*@null@ */
struct destmap_entry *destmap_entry_list_from_str( /*@dependent@ */ char
                                                  *buffer)
{
    /*@out@ */ struct destmap_entry *node = 0;
    if (buffer != 0) {
        node = malloc(sizeof *node);
        if (node != 0) {
            /*@dependent@ */ char *p = strchr(buffer, ':');
            *node = DESTMAP_ENTRY_STATIC_INITIALIZER;
            node->destid = buffer;
            if (p != 0 && p > buffer) {
                *p++ = '\0';
                node->exdest = p;
                p = strchr(p, ',');
                if (p != 0) {
                    *p++ = '\0';
                    node->next = destmap_entry_list_from_str(p);
                } else {
                    node->next = 0;
                }
            }
        }
    }
    return node;
}

struct destmap_t *destmap_fromstr(const char *buffer)
{
    struct destmap_t *result = 0;
    if (buffer != 0) {
        size_t buflen = strlen(buffer);
        if (buflen > 2) {
            /* Anything less than 3 is not valid, one char on the left,
               one on the right, and a colon separator character. */
            result = malloc(sizeof *result);
            if (result != 0) {
                result->buffer = malloc(buflen + 1);
                strncpy(result->buffer, buffer, buflen);
                result->buffer[buflen] = '\0';
                result->list = destmap_entry_list_from_str(result->buffer);
            }
        }
    }
    return result;
}

int destmap_find(struct destmap_t *destmap, const char *destination,
                 char **exdest)
{
    int result = -1;
    if (destmap != 0 && destination != 0 && destination[0] != '\0'
        && exdest != 0) {
        struct destmap_entry *node = destmap->list;
        if (node != 0) {
            while (1) {
                if (strcmp(destination, node->destid) == 0) {
                    *exdest = node->exdest;
                    result = 0;
                    break;
                } else {
                    node = node->next;
                    if (node == 0) {
                        break;
                    }
                }
            }
        }
    }
    return result;
}
