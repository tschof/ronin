#ifndef __DESTMAP_H__
#define __DESTMAP_H__

struct destmap;
struct destmap *destmap_fromstr(const char *);
int destmap_find(struct destmap *, const char *destination, char **exdest);
int destmap_findnum(struct destmap *, long destnum, char **exdest);
void free_destmap(struct destmap *);

#endif
