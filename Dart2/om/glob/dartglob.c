#include "dartglob.h"

#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <dlfcn.h>

static const struct dartglob_entry dartglob_entry_defaults = {
    .shared_object_name = 0,
    .config_file_name = 0,
    .dlhandle = 0,
    .entrypoint = 0,
    .token = 0,
    .next = 0,
};

struct dartglob_entry *dartglob_parse_config(const char *fname)
{
    struct dartglob_entry *headnode = 0;
    FILE *file = fopen(fname, "r");
    if (file != 0) {
        char *lineptr = 0;
        size_t len = 0;
        ssize_t nread;
        size_t lineno = 0;
        struct dartglob_entry *pnode = 0;
        while ((nread = getline(&lineptr, &len, file)) > 2) {
            ++lineno;
            if (headnode == 0) {
                headnode = malloc(sizeof *headnode);
                pnode = headnode;
            } else {
                pnode = pnode->next = malloc(sizeof *pnode);
            }
            *pnode = dartglob_entry_defaults;
            char *saveptr;
            pnode->shared_object_name = strtok_r(lineptr, "\t ", &saveptr);
            if (pnode->shared_object_name != 0) {
                pnode->shared_object_name =
                    strdup(pnode->shared_object_name);
            } else {
                fprintf(stderr, "bad config entry %s:%zu\n", fname,
                        lineno);
                pnode->config_file_name = 0;
                continue;
            }
            pnode->config_file_name = strtok_r(0, "\n", &saveptr);
            if (pnode->config_file_name != 0) {
                pnode->config_file_name = strdup(pnode->config_file_name);
            } else {
                fprintf(stderr, "bad config entry %s:%zu\n", fname,
                        lineno);
                pnode->shared_object_name = 0;
            }
        }
        free(lineptr);
    } else {
        perror("opening dartglob config");
    }
    return headnode;
}

int dartglob_load_shared_objects(struct dartglob_entry *list)
{
    int retval = -1;
    while (list != 0) {
        if (list->shared_object_name != 0 && list->config_file_name != 0) {
            void *tmpfuncptr;
            if ((list->dlhandle =
                 dlopen(list->shared_object_name, RTLD_NOW)) != 0
                && (tmpfuncptr =
                    dlsym(list->dlhandle,
                          dartglob_entrypoint_name)) != 0) {
                /* Horrible workaround between ISO C and POSIX 2008 dlsym,
                   see http://gcc.gnu.org/bugzilla/show_bug.cgi?id=45289
                   for more. */
                memcpy(&(list->entrypoint), &tmpfuncptr,
                       sizeof tmpfuncptr);
                printf
                    ("successful shared object load\nfile: %s\nfunction: %s\n",
                     list->shared_object_name, dartglob_entrypoint_name);
                retval = 0;
            } else {
                char *errstr = dlerror();
                fprintf(stderr, "file: %s\n, function: %s\n",
                        list->shared_object_name,
                        dartglob_entrypoint_name);
                if (errstr != 0) {
                    fprintf(stderr, "%s\n", errstr);
                }
                goto done;
            }
        }
        list = list->next;
    }
  done:
    return retval;
}
