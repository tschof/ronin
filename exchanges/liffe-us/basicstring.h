#ifndef __BASICSTRING_H__
#define __BASICSTRING_H__

struct string {
    char *ptr;
    size_t len;
};

typedef struct string string_t;

#define string_init(str, len) { (str), (len) }

#endif
