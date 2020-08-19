#include <stdlib.h>
#include "basicstring.h"
#include "symbol-map.h"
#include "../server_objects/debug_logger.h"
#include "../server_objects/hashmap.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>
#include "perrchk.h"

struct symbolmap {
    hashmap_t map;
};

/* A valid line has the mapping from left symbol to right symbol,
 * separated by the two characters '->'.  All external whitespace to
 * the left and right of the symbols are removed before returning left
 * and right parts.  Internal whitespace is _not_ removed. */
static int parse_line(char *line, char **left, char **right)
{
    int result = -1;
    char *sep = strstr(line, "->");
    if (sep != 0 && sep > line) {
        char *p;
        *sep = 0;
        for (*left = line; isspace(**left) != 0; ++(*left));
        for (p = sep - 1; isspace(*p) != 0; *p = 0, --p);
        for (*right = sep + 2; isspace(**right) != 0; ++(*right));
        for (p = *right + strlen(*right) - 1; isspace(*p) != 0;
             *p = 0, --p);
        result = 0;
    }
    return result;
}

symbolmap_t *load_symbol_map(const char *filename)
{
    symbolmap_t *syms = 0;
    FILE *f = fopen(filename, "r");
    if (f == 0) {
        goto onerror;
    }
    syms = malloc(sizeof *syms);
    const unsigned int mapsize = 1024;
    if ((syms->map = create_map(mapsize)) == 0) {
        send_debug_message("%s: error creating hash map of %u buckets",
                           mapsize);
        goto onerror;
    }
    ssize_t nread;
    char *lineptr = 0;
    size_t linelen = 0, line_number = 1;
    while ((nread = getline(&lineptr, &linelen, f)) != -1) {
        char *from, *to;
        if (parse_line(lineptr, &from, &to) == 0) {
            if (!insert_map
                (syms->map, from, (int) strlen(from), to,
                 (int) strlen(to) + 1)) {
                send_debug_message
                    ("error inserting into map (from='%s', to='%s')",
                     from, to);
                goto onerror;
            }
        } else {
            send_debug_message
                ("Error parsing symbol mapping, file %s, line %zu",
                 filename, line_number);
            goto onerror;
        }
        ++line_number;
    }
    free(lineptr);
    goto done;
  onerror:
    if (f != 0) {
        fclose(f);
    }
    if (syms != 0) {
        if (syms->map != 0) {
            delete_map(syms->map);
        }
        free(syms);
        syms = 0;
    }
  done:
    return syms;
}

int lookup_symbol(symbolmap_t * sm, string_t from, string_t * to)
{
    int result =
        get_map_entry(sm->map, from.ptr, from.len, (void **) &to->ptr);
    if (result) {
        to->len = strlen(to->ptr);
        result = 0;
    } else {
        result = -1;
    }
    return result;
}

void free_symbol_map(symbolmap_t * syms)
{
    if (syms != 0) {
        if (syms->map != 0) {
            delete_map(syms->map);
        }
        free(syms);
    }
}

int find_future_root(const char *origsym, size_t origlen,
                     const char **rootsym, size_t * rootlen)
{
    int result = -1;
    *rootsym = origsym;
    if (origlen < 3) {
        *rootlen = origlen;
    } else {
        const char *p = origsym + origlen - 1;
        if (isdigit(*p--) != 0 && isupper(*p)) {
            *rootlen = p - origsym;
            result = 0;
        }
    }
    return result;
}

struct synchronized_symbolmap {
    pthread_mutex_t mutex;
    symbolmap_t *map;
};

synch_symbolmap_t *create_synchronized_symbolmap(const char *filename)
{
    synch_symbolmap_t *sm = malloc(sizeof *sm);
    perrchk(pthread_mutex_init(&sm->mutex, 0), done);
    if ((sm->map = load_symbol_map(filename)) == 0) {
        free(sm);
        sm = 0;
    }
  done:
    return sm;
}

int reload_synchronized_symbolmap(synch_symbolmap_t * sm,
                                  const char *filename)
{
    int result = -1;
    perrchk(pthread_mutex_lock(&sm->mutex), done);
    symbolmap_t *tmp = load_symbol_map(filename);
    if (tmp != 0) {
        free_symbol_map(sm->map);
        sm->map = tmp;
        result = 0;
    }
    perrchk(pthread_mutex_unlock(&sm->mutex), done);
  done:
    return result;
}

/* The from argument must be the root symbol, not the entire futures
 * contract.  The to return value is also just the root symbol.  */
int synchronized_lookup_symbol(synch_symbolmap_t * sm, string_t from,
                               string_t * to)
{
    int result = -1;
    perrchk(pthread_mutex_lock(&sm->mutex), done);
    if ((result = lookup_symbol(sm->map, from, to)) == 0) {
        result = 0;
    }
    perrchk(pthread_mutex_unlock(&sm->mutex), done);
  done:
    return result;
}

int parse_futures_symbol(string_t symbol, string_t * root,
                         string_t * expiration)
{
    int parsed = -1;
    if (symbol.len > 2) {
        expiration->ptr = symbol.ptr + symbol.len - 1;
        if (isdigit(*expiration->ptr--) && isupper(*expiration->ptr)) {
            expiration->len = 2;
            root->ptr = symbol.ptr;
            root->len = expiration->ptr - symbol.ptr;
            parsed = 0;
        }
    }
    return parsed;
}

void destroy_synchronized_symbolmap(synch_symbolmap_t * sm)
{
    perrchk(pthread_mutex_destroy(&sm->mutex), done);
    free_symbol_map(sm->map);
    free(sm);
  done:
    return;
}


#ifdef BAZTEST

#include "../baz/libbaz.h"
#include <unistd.h>

void test_line_parsing(void)
{
    char *p = malloc(1024);
    char *left, *right;

    strcpy(p, "foobar");
    CHKCOND(parse_line(p, &left, &right) == -1);

    strcpy(p, "foo->bar");
    CHKCOND(parse_line(p, &left, &right) == 0);
    CHKCOND(strcmp(left, "foo") == 0);
    CHKCOND(strcmp(right, "bar") == 0);

    strcpy(p, "  foo  ->  bar  ");
    CHKCOND(parse_line(p, &left, &right) == 0);
    CHKCOND(strcmp(left, "foo") == 0);
    CHKCOND(strcmp(right, "bar") == 0);

    strcpy(p, "  foo\t  ->\n  bar\t  ");
    CHKCOND(parse_line(p, &left, &right) == 0);
    CHKCOND(strcmp(left, "foo") == 0);
    CHKCOND(strcmp(right, "bar") == 0);

    free(p);
}

static int write_test_symbol_map_file(const char *filename,
                                      const char *contents)
{
    int result = -1, fd = creat(filename, S_IRUSR | S_IWUSR);
    if (fd != -1) {
        ssize_t nwrit = write(fd, contents, strlen(contents));
        if (nwrit != -1) {
            result = 0;
        } else {
            perror("writing file");
        }
        close(fd);
    } else {
        perror("creating file");
    }
    return result;
}

void test_symbol_map_loading(void)
{
    const char *filename = "symmap-test.in", *contents = " ";
    symbolmap_t *map = 0;
    free_symbol_map(map);
    CHKCOND(write_test_symbol_map_file(filename, contents) == 0);
    map = load_symbol_map(filename);
    CHKCOND(map == 0);
    free_symbol_map(map);
    CHKCOND(map == 0);
    free_symbol_map(map);
    contents = "ZVZZT->FOOBAR";
    CHKCOND(write_test_symbol_map_file(filename, contents) == 0);
    map = load_symbol_map(filename);
    CHKCOND(map != 0);
    string_t from = string_init("ZVZZT", 5);
    string_t to;
    CHKCOND(lookup_symbol(map, from, &to) == 0);
    CHKCOND(strcmp(to.ptr, "FOOBAR") == 0);
    free_symbol_map(map);
    contents = "   this\t->that\n12345  -> 67890   ";
    CHKCOND(write_test_symbol_map_file(filename, contents) == 0);
    CHKCOND((map = load_symbol_map(filename)) != 0);
    from.ptr = "foo";
    from.len = 3;
    CHKCOND(lookup_symbol(map, from, &to) == -1);
    from.ptr = "this";
    from.len = 4;
    CHKCOND(lookup_symbol(map, from, &to) == 0);
    CHKCOND(strcmp(to.ptr, "that") == 0);
    from.ptr = "12345";
    from.len = 5;
    CHKCOND(lookup_symbol(map, from, &to) == 0);
    CHKCOND(strcmp(to.ptr, "67890") == 0);
    free_symbol_map(map);
}

void test_find_future_root(void)
{
    const char *fullsym = "", *rootsym = 0;
    size_t fullsymlen = 0, rootsymlen = 0;

    CHKCOND(find_future_root(fullsym, fullsymlen, &rootsym, &rootsymlen) ==
            -1);

    fullsym = "AAPL";
    fullsymlen = 4;
    CHKCOND(find_future_root(fullsym, fullsymlen, &rootsym, &rootsymlen) ==
            -1);

    fullsym = "ESZ2";
    fullsymlen = 4;
    CHKCOND(find_future_root(fullsym, fullsymlen, &rootsym, &rootsymlen) ==
            0);
    CHKCOND(rootsymlen == 2);
    CHKCOND(strncmp(rootsym, "ES", rootsymlen) == 0);

    fullsym = "esz2";
    fullsymlen = 4;
    CHKCOND(find_future_root(fullsym, fullsymlen, &rootsym, &rootsymlen) ==
            -1);
}

void *threadfunc(void *arg)
{
    synch_symbolmap_t *sm = arg;
    string_t from = string_init("MFS", 3);
    string_t to;
    synchronized_lookup_symbol(sm, from, &to);
    from.ptr = "MME";
    synchronized_lookup_symbol(sm, from, &to);
    from.ptr = "foo";
    synchronized_lookup_symbol(sm, from, &to);
    return 0;
}

void multi_thread_test(void)
{
    if (write_test_symbol_map_file("symmap-test.in",
                                   "MFS->NMFS\nMME->NMME\n") != 0) {
        fprintf(stderr, "%s: Cannot write to file, bailing out of test\n",
                __FUNCTION__);
        return;
    }
    synch_symbolmap_t *sm =
        create_synchronized_symbolmap("symmap-test.in");
    CHKCOND(sm != 0);
    pthread_t t1, t2;
    CHKCOND(pthread_create(&t1, 0, threadfunc, (void *) sm) == 0);
    CHKCOND(pthread_create(&t2, 0, threadfunc, (void *) sm) == 0);
    void *retval1, *retval2;
    CHKCOND(pthread_join(t1, &retval1) == 0);
    CHKCOND(pthread_join(t2, &retval2) == 0);
    destroy_synchronized_symbolmap(sm);
}

#endif
