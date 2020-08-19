#include "destmap.h"
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <ctype.h>

struct destmap_entry {
    char *destid;
    long destnum;
    char *exdest;
    struct destmap_entry *next;
};

static const struct destmap_entry DESTMAP_ENTRY_STATIC_INITIALIZER =
    { 0, 0, 0, 0 };

struct destmap {
    char *buffer;
    struct destmap_entry *list;
};

/* The only dynamically allocated memory within each destmap_entry is
 * the actual structure itself.  The buffer that the char * variables
 * point into is allocated and handled outside the destmap_entry
 * structure. */
void free_destmap_entries(struct destmap_entry *entry)
{
    while (entry != 0) {
        struct destmap_entry *tmp = entry->next;
        free(entry);
        entry = tmp;
    }
}

void free_destmap(struct destmap *dest)
{
    free_destmap_entries(dest->list);
    free(dest->buffer);
    free(dest);
}

/* Parse just one entry from the buffer.  One entry consists of
 * 'num:dest'.  buffer is assumed to be a null-terminated containing
 * exactly one 'num:dest' pair. */
static struct destmap_entry *parse_entry(char *buffer)
{
    struct destmap_entry *entry = 0;
    if (buffer != 0 && strlen(buffer) > 0) {
        char *delim = strchr(buffer, ':');
        if (delim != 0) {
            entry = malloc(sizeof *entry);
            if (entry != 0) {
                for ( ; *buffer != '\0' && isspace(*buffer) != 0; ++buffer);
                entry->destid = buffer;
                *delim++ = '\0';
                entry->exdest = delim;
                entry->next = 0;
                errno = 0;
                char *endptr;
                entry->destnum = strtol(entry->destid, &endptr, 10);
                if (errno != 0 || endptr <= entry->destid
                    || *endptr != '\0') {
                    /* Error in parsing the integer */
                    free(entry);
                    entry = 0;
                }
            }
        }
    }
    return entry;
}

static struct destmap_entry *destmap_entry_list_from_str(char *buffer)
{
    struct destmap_entry *retval = 0, *current = 0;
    while (buffer != 0 && strlen(buffer) > 0) {
        char *delim = strchr(buffer, ',');
        if (delim != 0) {
            *delim++ = '\0';
        }
        if (retval == 0) {
            if ((retval = parse_entry(buffer)) == 0) {
                free_destmap_entries(retval);
                retval = 0;
            }
            current = retval;
        } else {
            if (current == 0 || (current->next = parse_entry(buffer)) == 0) {
                free_destmap_entries(retval);
                retval = 0;
            }
            current = current->next;
        }
        buffer = delim;
    }
    return retval;
}

struct destmap *destmap_fromstr(const char *buffer)
{
    struct destmap *result = 0;
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
                if ((result->list =
                     destmap_entry_list_from_str(result->buffer)) == 0) {
                    free_destmap(result);
                    result = 0;
                }
            }
        }
    }
    return result;
}

int destmap_find(struct destmap *destmap, const char *destination,
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

int destmap_findnum(struct destmap *destmap, long destnum, char **exdest)
{
    int result = -1;
    if (destmap != 0 && destnum > -1 && exdest != 0) {
        struct destmap_entry *node;
        for (node = destmap->list; node != 0; node = node->next) {
            if (node->destnum == destnum) {
                *exdest = node->exdest;
                result = 0;
                break;
            }
        }
    }
    return result;
}

#ifdef BAZTEST

#include "../baz/libbaz.h"

void test_destmap()
{
    /* Invalid inputs */

    /* Insufficient inputs */
    CHKCOND(destmap_fromstr(0) == 0);
    CHKCOND(destmap_fromstr("") == 0);
    CHKCOND(destmap_fromstr("foo") == 0);
    CHKCOND(destmap_fromstr("foo:") == 0);

    /* Sufficient input format, but left is not a number. */
    CHKCOND(destmap_fromstr("f2oo:bar") == 0);

    struct destmap *map;
    struct destmap_entry *entry;

    /* Valid */
    CHKCOND((map = destmap_fromstr("1:FOO")) != 0);
    CHKCOND(map->list != 0);
    CHKCOND(strcmp(map->buffer, "1") == 0);
    entry = map->list;
    CHKCOND(strcmp(entry->destid, "1") == 0);
    CHKCOND(strcmp(entry->exdest, "FOO") == 0);
    CHKCOND(entry->destnum == 1);
    CHKCOND(entry->next == 0);
    free_destmap(map);

    CHKCOND((map = destmap_fromstr("1:FOO,")) != 0);
    CHKCOND(map->list != 0);
    CHKCOND(strcmp(map->buffer, "1") == 0);
    entry = map->list;
    CHKCOND(strcmp(entry->destid, "1") == 0);
    CHKCOND(strcmp(entry->exdest, "FOO") == 0);
    CHKCOND(entry->destnum == 1);
    CHKCOND(entry->next == 0);
    free_destmap(map);

    CHKCOND((map = destmap_fromstr("1:FOO, 2:BAR")) != 0);
    CHKCOND(map->list != 0);
    CHKCOND(strcmp(map->buffer, "1") == 0);
    entry = map->list;
    CHKCOND(strcmp(entry->destid, "1") == 0);
    CHKCOND(strcmp(entry->exdest, "FOO") == 0);
    CHKCOND(entry->destnum == 1);
    CHKCOND(entry->next != 0);
    entry = entry->next;
    CHKCOND(strcmp(entry->destid, "2") == 0);
    CHKCOND(strcmp(entry->exdest, "BAR") == 0);
    CHKCOND(entry->destnum == 2);
    CHKCOND(entry->next == 0);
    free_destmap(map);
}

#endif
