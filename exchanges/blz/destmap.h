#ifndef __DESTMAP_H__
#define __DESTMAP_H__

struct destmap_t;
struct destmap_t *destmap_fromstr(const char *);
int destmap_find(struct destmap_t *destmap, const char *destination,
                 char **exdest);

#endif                          //__DESTMAP_H__
