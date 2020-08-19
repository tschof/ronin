#ifndef __ROMCOLLECTION_H__
#define __ROMCOLLECTION_H__

#include <stdlib.h>

struct romcollection;
typedef struct romcollection romcollection_t;

romcollection_t *make_romcollection(void);
void free_romcollection(romcollection_t *);
int romcollection_addrom(romcollection_t *, const char *, size_t);
int romcollection_removerom(romcollection_t *, const char *, size_t);

typedef enum romcollection_retval {
    remove_rom = -1,
    okay = 0,
    bail = 1,
} romcollection_retval;

typedef romcollection_retval(*romcollection_callback) (const char *,
                                                       size_t, void *);
int romcollection_foreach(romcollection_t *, romcollection_callback,
                          void *);

#endif
