#include "inputstream.h"
#include <string.h>
#include <stdio.h>

struct fileinputstream {
    struct inputstream parent;
    FILE *file;
};

ssize_t
fileinputstream_getline(struct inputstream *istream, dstr_t * result)
{
    ssize_t ret = -1;
    if (istream != 0 && result != 0) {
        struct fileinputstream *ifstream =
            (struct fileinputstream *) istream;
        ret = getline(&result->p, &result->capacity, ifstream->file);
        if (ret != -1) {
            result->n = strlen(result->p);
        }
    }
    return ret;
}

void fileinputstream_free(struct inputstream *istream)
{
    struct fileinputstream *ifstream = (struct fileinputstream *) istream;
    if (ifstream->file != 0) {
        fclose(ifstream->file);
    }
    free(ifstream);
}

struct inputstream *make_file_inputstream(const char *filename)
{
    struct fileinputstream *ret = 0;
    if (filename != 0) {
        ret = malloc(sizeof *ret);
        if (ret != 0) {
            ret->file = fopen(filename, "r");
            if (ret->file != 0) {
                ret->parent.getline = fileinputstream_getline;
                ret->parent.free = fileinputstream_free;
            } else {
                free(ret);
                ret = 0;
            }
        }
    }
    return (struct inputstream *) ret;
}

struct string_inputstream {
    char *allocp, *strp;
};

void string_free(struct inputstream *stream)
{
    struct string_inputstream *strstream = stream->impl;
    free(strstream->allocp);
    free(strstream);
    free(stream);
}

/* Todo: Test this more, changes made and not tested any, if at
   all. */
ssize_t string_getline(struct inputstream *stream, dstr_t * result)
{
    ssize_t ret = -1;
    if (stream != 0 && result != 0) {
        struct string_inputstream *strstream = stream->impl;
        if (strstream != 0 && strstream->strp != 0
            && strstream->strp[0] != '\0') {
            char *newline = strchr(strstream->strp, '\n');
            if (newline != 0) {
                dstr_copy(result, strstream->strp,
                          newline - strstream->strp + 1);
            } else {
                dstr_copy(result, strstream->strp,
                          strlen(strstream->strp));
            }
            strstream->strp += result->n;
            ret = 0;
        }
    }
    return ret;
}

struct inputstream *make_string_inputstream(const char *string)
{
    struct string_inputstream *impl = malloc(sizeof *impl);
    impl->allocp = impl->strp = strdup(string);
    struct inputstream *retval = malloc(sizeof *retval);
    retval->impl = impl;
    retval->getline = string_getline;
    retval->free = string_free;
    return retval;
}

#ifdef COMPILE_UNIT_TESTS
#include "../../unit-tester/unit-tester.h"

declare_unit_test(string_input_stream)
{
    struct inputstream *istream =
        make_string_inputstream("foo\nbar\n\nlast line");
    check(istream != 0);
    check(istream->getline != 0);
    check(istream->free != 0);
    check(istream->impl != 0);
    dstr_t s = dstr_init;
    check(istream->getline(istream, &s) == 0);
    check(s.p != 0 && s.n > 0);
    check(strcmp(s.p, "foo\n") == 0);
    check(s.n == 4);
    check(istream->getline(istream, &s) == 0);
    check(s.p != 0 && s.n > 0);
    check(strcmp(s.p, "bar\n") == 0);
    check(s.n == 4);
    check(istream->getline(istream, &s) == 0);
    check(strcmp(s.p, "\n") == 0);
    check(s.n == 1);
    dstr_delete(&s);
    istream->free(istream);
}

#endif
