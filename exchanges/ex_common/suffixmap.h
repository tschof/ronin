#ifndef __SUFFIX_MAP_H__
#define __SUFFIX_MAP_H__

#include <stdlib.h>

typedef struct suffixmap suffixmap_t;

int buildsuffixmap(suffixmap_t ** map, const char *datafile);
int lookupsuffix(suffixmap_t * map, const char *suffix, const char **val);
int lookupsuffix2(suffixmap_t * map, const char *suffix,
                  size_t suffixlen, const char **val);
void destroysuffixmap(suffixmap_t * map);

#endif                          //__SUFFIX_MAP_H__
