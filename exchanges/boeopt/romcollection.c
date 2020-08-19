#include "romcollection.h"
#include <string.h>
#include <pthread.h>
#include <errno.h>
#include <stdio.h>

typedef struct romlistnode {
    char *nameptr;
    size_t namelen;
    struct romlistnode *next;
} romlistnode_t;

struct romcollection {
    pthread_mutex_t mutex;
    struct romlistnode *nodes;
};

romlistnode_t *make_romlistnode(const char *nameptr, size_t namelen)
{
    romlistnode_t *retval = 0;
    if (nameptr && namelen) {
        retval = malloc(sizeof *retval);
        if (retval) {
            retval->next = 0;
            retval->namelen = namelen;
            retval->nameptr = malloc(retval->namelen + 1);
            if (retval->nameptr) {
                memcpy(retval->nameptr, nameptr, retval->namelen);
                retval->nameptr[retval->namelen] = '\0';
            } else {
                free(retval);
                retval = 0;
            }
        }
    }
    return retval;
}

romcollection_t *make_romcollection(void)
{
    romcollection_t *retval = malloc(sizeof *retval);
    if (retval) {
        retval->nodes = 0;
        int initval = pthread_mutex_init(&retval->mutex, 0);
        if (initval) {
            errno = initval;
            perror("initializing mutex");
            free(retval);
            retval = 0;
        }
    }
    return retval;
}

void free_romlistnode(romlistnode_t * node)
{
    if (node) {
        free(node->nameptr);
    }
    free(node);
}

void free_romcollection(romcollection_t * roms)
{
    romlistnode_t *node = roms->nodes;
    while (node != 0) {
        romlistnode_t *next = node->next;
        free_romlistnode(node);
        node = next;
    }
    free(roms);
}

int romcollection_addrom(romcollection_t * roms, const char *nameptr,
                         size_t namelen)
{
    int retval = 0, pthread_retval = pthread_mutex_lock(&roms->mutex);
    if (pthread_retval) {
        errno = retval;
        perror("romcollection_addrom() locking mutex");
        return -1;
    }
    if (!roms->nodes) {
        if ((roms->nodes = make_romlistnode(nameptr, namelen)) == 0) {
            /* Encountered an error */
            retval = -1;
        }
    } else {
        romlistnode_t *node, *last;
        for (node = roms->nodes, last = 0; node != 0;
             last = node, node = node->next) {
            if (node->namelen == namelen
                && strncmp(node->nameptr, nameptr, namelen) == 0) {
                /* Something fishy going on, we are trying to add a
                 * ROM name that is already in the list. */
                retval = -1;
                goto done;
            }
        }
        if (last && !node) {
            if ((last->next = make_romlistnode(nameptr, namelen)) == 0) {
                retval = -1;
            }
        } else {
            /* Somehow we are in an inconsistent state, bail out */
            retval = -1;
        }
    }
  done:
    pthread_retval = pthread_mutex_unlock(&roms->mutex);
    if (pthread_retval) {
        errno = pthread_retval;
        perror("romcollection_addrom() unlocking mutex");
        retval = -1;
    }
    return retval;
}

int romcollection_removerom(romcollection_t * roms, const char *nameptr,
                            size_t namelen)
{
    int retval = -1, pthread_retval = pthread_mutex_lock(&roms->mutex);
    if (pthread_retval) {
        errno = retval;
        perror("romcollection_removerom() locking mutex");
        return -1;
    }
    if (roms && nameptr && namelen) {
        romlistnode_t *node, *last;
        for (last = 0, node = roms->nodes; node != 0;
             last = node, node = node->next) {
            if (namelen == node->namelen
                && strncmp(node->nameptr, nameptr, namelen) == 0) {
                /* Found it */
                if (last == 0) {
                    roms->nodes = node->next;
                } else {
                    last->next = node->next;
                }
                free_romlistnode(node);
                retval = 0;
                break;
            }
        }
    }
    pthread_retval = pthread_mutex_unlock(&roms->mutex);
    if (pthread_retval) {
        errno = pthread_retval;
        perror("romcollection_removerom() unlocking mutex");
        retval = -1;
    }
    return retval;
}

int romcollection_foreach(romcollection_t * roms,
                          romcollection_callback cbfunc, void *cbdata)
{
    int retval = 0, pthread_retval = pthread_mutex_lock(&roms->mutex);
    if (pthread_retval) {
        errno = retval;
        perror("romcollection_foreach() locking mutex");
        return -1;
    }
    if (roms && cbfunc) {
        romlistnode_t *node = roms->nodes, *last = 0;
        while (node != 0) {
            romcollection_retval cbresult =
                cbfunc(node->nameptr, node->namelen, cbdata);
            if (cbresult == remove_rom) {
                if (last == 0) {
                    roms->nodes = node->next;
                    free_romlistnode(node);
                    node = roms->nodes;
                } else {
                    last->next = node->next;
                    free_romlistnode(node);
                    node = last;
                }
            } else if (cbresult != okay) {
                break;
            } else {
                last = node;
                node = node->next;
            }
        }
    }
    pthread_retval = pthread_mutex_unlock(&roms->mutex);
    if (pthread_retval) {
        errno = pthread_retval;
        perror("romcollection_foreach() unlocking mutex");
        retval = -1;
    }
    return retval;
}

#ifdef BAZTEST

#include "../baz/libbaz.h"

struct romcollection_testdata {
    char *str, *saveptr;
    char *names;
};

romcollection_retval
validate_fields(const char *name, size_t len, void *cbdata)
{
    romcollection_retval retval = okay;
    CHKCOND(cbdata);
    if (cbdata) {
        struct romcollection_testdata *testdata = cbdata;
        char *token = strtok_r(testdata->str, ",", &testdata->saveptr);
        testdata->str = 0;
        CHKCOND(token);
        if (token) {
            size_t tokenlen = strlen(token);
            CHKCOND(tokenlen == len);
            if (tokenlen == len) {
                CHKCOND(strncmp(name, token, tokenlen) == 0);
            }
        }
    }
    return retval;
}

romcollection_retval
make_sure_bail_works(const char *name, size_t len, void *cbdata)
{
    int *ncallbacks = cbdata;
    CHKCOND(*ncallbacks == 0);
    ++(*ncallbacks);
    return bail;
}

romcollection_retval
remove_second_rom(const char *name, size_t len, void *cbdata)
{
    romcollection_retval retval = okay;
    int *ncallbacks = cbdata;
    if (*ncallbacks == 1) {
        retval = remove_rom;
    }
    ++(*ncallbacks);
    return retval;
}

romcollection_retval remove_all(const char *name, size_t len, void *cbdata)
{
    return remove_rom;
}

romcollection_retval
remove_this_particular_string(const char *name, size_t len, void *cbdata)
{
    const char *remove_this = cbdata;
    if (strlen(remove_this) == len && strncmp(remove_this, name, len) == 0) {
        return remove_rom;
    } else {
        return okay;
    }
}

void test_romcollection(void)
{
    romcollection_t *roms = make_romcollection();
    CHKCOND(roms != 0);
    CHKCOND(romcollection_addrom(roms, "Foo", 3) == 0);
    CHKCOND(romcollection_removerom(roms, "Bar", 3) == -1);
    CHKCOND(romcollection_addrom(roms, "Foo", 3) == -1);
    CHKCOND(romcollection_removerom(roms, "Foo", 2) == -1);
    CHKCOND(romcollection_removerom(roms, "Foo", 4) == -1);
    CHKCOND(romcollection_removerom(roms, "Foo", 3) == 0);
    CHKCOND(romcollection_removerom(roms, "Foo", 3) == -1);
    CHKCOND(romcollection_addrom(roms, "one", 3) == 0);
    CHKCOND(romcollection_addrom(roms, "two", 3) == 0);
    CHKCOND(romcollection_addrom(roms, "three", 5) == 0);
    struct romcollection_testdata validator = {
        .names = strdup("one,two,three"),
        .saveptr = 0,
    };
    validator.str = validator.names;
    CHKCOND(romcollection_foreach(roms, validate_fields, &validator) == 0);
    free(validator.names);
    int ncallbacks = 0;
    CHKCOND(romcollection_foreach(roms, make_sure_bail_works, &ncallbacks)
            == 0);
    ncallbacks = 0;
    CHKCOND(romcollection_foreach(roms, remove_second_rom, &ncallbacks) ==
            0);
    validator.str = validator.names = strdup("one,three");
    validator.saveptr = 0;
    CHKCOND(romcollection_foreach(roms, validate_fields, &validator) == 0);
    free(validator.names);
    free_romcollection(roms);
    roms = make_romcollection();
    CHKCOND(roms != 0);
    CHKCOND(romcollection_addrom(roms, "something", 9) == 0);
    CHKCOND(romcollection_addrom(roms, "else", 4) == 0);
    CHKCOND(romcollection_addrom(roms, "entirely", 8) == 0);
    CHKCOND(romcollection_foreach(roms, remove_all, 0) == 0);
    CHKCOND(romcollection_addrom(roms, "remove", 6) == 0);
    CHKCOND(romcollection_addrom(roms, "this", 4) == 0);
    CHKCOND(romcollection_addrom(roms, "too", 3) == 0);
    CHKCOND(romcollection_foreach
            (roms, remove_this_particular_string, "this") == 0);
    CHKCOND(romcollection_foreach
            (roms, remove_this_particular_string, "too") == 0);
    CHKCOND(romcollection_foreach
            (roms, remove_this_particular_string, "remove") == 0);
    CHKCOND(roms->nodes == 0);
    free_romcollection(roms);
}

#endif
