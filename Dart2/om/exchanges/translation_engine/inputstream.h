#ifndef __INPUTSTREAM_H__
#define __INPUTSTREAM_H__

#include <stdlib.h>
#include "dartstring.h"

/* Similar to the std::istream in the C++ standard library, this is an
 * attempt to formalize the concept of a stream of input, tied on the
 * backend to some source, from which data can be read until the
 * stream has been exhausted. */
struct inputstream {
    /* Call to get a whole line.  The dstr_t gets the result.  Anything
       in an existing dstr_t will be overwritten.  The pointer is
       assumed to be realloc'able.  The dstr_t result must be valid,
       i.e. properly initialized to dstr_init (see dartstring.h) or have
       a valid pointer and size_t allocated using malloc. */
    ssize_t(*getline) (struct inputstream *, dstr_t *);
    void (*free) (struct inputstream *);
    void *impl;
};

typedef struct inputstream istream_t;

/* Pass a valid filename to make a file input stream.  Returns 0 on
   error, inspect errno to determine exact error. */
istream_t *make_file_inputstream(const char *);

/* Pass a string to make the istream.  The string is copied so further
   modification of the string is okay. Returns 0 on error, inspect
   errno to determine exact error. */
istream_t *make_string_inputstream(const char *);

#endif
