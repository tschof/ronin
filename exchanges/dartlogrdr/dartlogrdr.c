#include "FileBuffer.h"
#include "server_objects/databuf.h"

#include <stdio.h>
#include <regex.h>
#include <errno.h>
#include <dlfcn.h>

typedef ssize_t(*framingfunc) (const void *, size_t);
typedef int (*textualizefunc) (const void *, size_t, databuf_t *);

struct protocol {
    framingfunc frame;
    textualizefunc tostr;
    void *so_handle;
};

static const char *memmatch(const char *bufp, const char *bufend,
                            const char *seqstart, const char *seqend)
{
    const char *revertp = 0, *p, *seqp, *retval = 0;
    for (revertp = 0, p = bufp, seqp = seqstart;
         p < bufend && seqp < seqend; ++p) {
        if (*p == *seqp) {
            if (revertp == 0) {
                revertp = p;
            }
            ++seqp;
        } else {
            if (revertp != 0) {
                p = revertp;
                revertp = 0;
            }
            seqp = seqstart;
        }
    }
    if (seqp == seqend) {
        retval = revertp;
    }
    return retval;
}

static regex_t prefix;

/* We must account for this regex hitting a null character and
 * skipping over that.  AFAIK the regex library assumes a
 * null-terminated C-style string, so the regexec function call
 * returns as soon as it finds a null character.  However, the log
 * file contains binary data that could very well contain null
 * characters.  Under normal circumstances we should not encounter
 * this situation, but it should be handled in case there is an error
 * in the protocol parsing. */
static int parseLogEntryHeader(slice data)
{
    regmatch_t match = { 0, 0 };
    int errcode = regexec(&prefix, data.p, 1, &match, 0);
    if (errcode == 0)
        return match.rm_eo;
    else if (errcode == REG_NOMATCH)
        return 0;
    else
        return -1;
}

static int parseLogEntryTrailer(slice parseBuffer)
{
    slice buffer = parseBuffer;
    const char *ending = " --end \n";
    const char *currentMatch = ending;
    while (buffer.n > 0) {
        if (*buffer.p != *currentMatch)
            currentMatch = ending;
        else
            ++currentMatch;
        consumeBytes(&buffer, 1);
        if (*currentMatch == '\0')
            break;
    }
    return buffer.p - parseBuffer.p;
}

static const char endMarker[] = " --end \n";
static const size_t endMarkerLength = sizeof endMarker - 1;

static int entry(slice chunk, databuf_t * out, framingfunc frameit,
                 textualizefunc textualizeit)
{
    /* The whole idea here is to adjust the chunk to be an entire log
     * entry. */

    const char *p = chunk.p;
    const char *end = chunk.p + chunk.n;

    int nconsumed = 0;

    /* Find the end */
    const char *trailer =
        memmatch(p, end, endMarker, endMarker + endMarkerLength);
    if (trailer == 0) {
        /* Not found, must not have the entire log entry. */
        goto done;
    }
    end = trailer + endMarkerLength;

    /* Match the beginning of the log entry. */
    regmatch_t match = { 0, 0 };
    int errcode = regexec(&prefix, p, 1, &match, 0);
    if (errcode == REG_NOMATCH) {
        /* For some reason we haven't found any header.  Simply dump
         * what we've got and exit. */
        nconsumed = end - p;
        databuf_memcpy(out, p, nconsumed);
        goto done;
    }

    if (errcode != 0) {
        /* Fatal error, print something bad and die. */
        size_t nerrbuf = regerror(errcode, &prefix, 0, 0);
        if (nerrbuf > 0) {
            ++nerrbuf;
            char *errbuf = malloc(nerrbuf);
            regerror(errcode, &prefix, errbuf, nerrbuf);
            fprintf(stderr,
                    "Problem using regex to parse beginning of log entry: %s\n",
                    errbuf);
            free(errbuf);
            nconsumed = -1;
            goto done;
        }
    }

    /* Successfully got the header and trailer.  The payload is in
     * between.  Parse the payload, if we can, and dump it all to the
     * output. */
    databuf_memcpy(out, p, match.rm_eo);
    nconsumed += match.rm_eo;
    p += match.rm_eo;

    ssize_t npayload = trailer - p;
    if (npayload < 0) {
        nconsumed = -1;
        goto done;
    }

    ssize_t nframedbytes = frameit(p, npayload);
    if (nframedbytes == npayload) {
        if (textualizeit(p, npayload, out) == -1) {
            databuf_memcpy(out, p, npayload);
        }
    } else {
        databuf_memcpy(out, p, npayload);
    }
    nconsumed += npayload;

    databuf_memcpy(out, trailer, end - trailer);
    nconsumed += end - trailer;

  done:
    return nconsumed;
}

static void decodeLogFile(char *filename, struct databuf *out,
                          struct protocol *proto)
{
    struct FileBuffer *file;
    if ((file = makeFileBuffer(filename)) == 0) {
        fprintf(stderr, "Processing %s: error making file buffer\n",
                filename);
        return;
    }
    if (readMore(file) == -1) {
        fprintf(stderr, "Error reading more from file\n");
        return;
    }
    for (;;) {
        slice *data = getReadData(file);
        if (data->n == 0 && isEof(file) != 0) {
            break;
        }
        int nprocessed = entry(*data, out, proto->frame, proto->tostr);
        if (nprocessed > 0) {
            consumeBytes(data, nprocessed);
            printf("%.*s", (int) databuf_unread(out), out->rd_ptr);
            databuf_reset(out);
        } else if (nprocessed == 0) {
            if (readMore(file) == -1) {
                fprintf(stderr, "Error reading more from file\n");
                return;
            }
        } else {
            fprintf(stderr, "Error processing log data\n");
            return;
        }
    }
}

static int bind_protocol_so(const char *so_name, struct protocol *p)
{
    int retval = 0;
    const char *dlerrstr = 0, *action = "dlopen";
    p->so_handle = dlopen(so_name, RTLD_NOW);
    if (p->so_handle != 0) {
        if ((p->frame = dlsym(p->so_handle, "dart_msg_frame")) == 0) {
            action = "mapping dart_msg_frame function";
            goto on_error;
        }
        if ((p->tostr = dlsym(p->so_handle, "dart_msg_tostr")) == 0) {
            action = "mapping dart_msg_tostr function";
            goto on_error;
        }
        goto done;
    }

on_error:
    fprintf(stderr, "%s: ", action);
    if ((dlerrstr = dlerror()) != 0) {
        fprintf(stderr, "%s\n", dlerrstr);
    } else {
        fprintf(stderr, "unknown\n");
    }
    retval = -1;

done:
    return retval;
}

int main(int argc, char *argv[])
{
    int retval = EXIT_SUCCESS;
    if (argc < 3) {
        fprintf(stderr, "usage: boe-log-reader protocol-so file1 <file2...>\n");
        return EXIT_FAILURE;
    }
    struct protocol p;
    if (bind_protocol_so(argv[1], &p) == 0) {
        int regex_errcode;
        if ((regex_errcode =
             regcomp(&prefix,
                     "^[[:digit:]]{8}-[[:digit:]]{2}:[[:digit:]]{2}:"
                     "[[:digit:]]{2}\\.[[:digit:]]{3,6}--(To|From)--"
                     " [[:alnum:]]+  ", REG_EXTENDED)) != 0) {
            errno = regex_errcode;
            perror("compiling regex");
            return EXIT_FAILURE;
        }
        struct databuf *out = databuf_alloc(4096);
        int i;
        for (i = 2; i < argc; ++i) {
            decodeLogFile(argv[i], out, &p);
        }
        databuf_destroy(out);
        dlclose(p.so_handle);
    } else {
        retval = EXIT_FAILURE;
    }
    return retval;
}
