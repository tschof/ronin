#include "DartLogParser.h"
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>


enum RegexpMatchType {
    RegexpAnyDigit,
    RegexpChar,
    RegexpString,
    RegexpWhitespace,
};

union RegexpMatchData {
    char c;
    char *p;
};

#define REPEAT_ONE_OR_MORE -1
#define FLAGS_NOT

struct Regexp;

typedef int (*matchfunc) (struct Slice, struct Regexp *);

struct Regexp {
    matchfunc func;
    /* enum RegexpMatchType type; */
    union RegexpMatchData data;
    int32_t repeat;
    uint32_t flags;
};

int matchAnyDigit(struct Slice slice, struct Regexp *regexp)
{
    return isdigit(*slice.p) != 0 ? 1 : -1;
}

int matchSpecificChar(struct Slice slice, struct Regexp *regexp)
{
    return *slice.p == regexp->data.c ? 1 : -1;
}

int matchString(struct Slice slice, struct Regexp *regexp)
{
    size_t nlen = strlen(regexp->data.p);
    if (slice.n < nlen)
        return -1;
    return strncmp(slice.p, regexp->data.p, nlen) == 0 ? (int) nlen : -1;
}

int regexpMatch(struct Slice data, struct Regexp *pmatch, size_t nmatch)
{
    /* size_t i = 0; */
    /* struct Slice remaining = data; */
    /* size_t nrepeat = 1, nconsumed = 1; */
    /* while (i < nmatch && remaining.n > 0) { */
    /*     int matchResult = pmatch[i].func(remaining, &pmatch[i]); */
    /*     if (matchResult == -1) */
    /*         return -1; */

    /* } */
    return -1;
    /*     switch (pmatch[i].type) { */
    /*     case RegexpAnyDigit: */
    /*         if (isdigit(*p) == 0) */
    /*             goto unmatched; */
    /*         break; */
    /*     case RegexpChar: */
    /*         if (*p != pmatch[i].data.c) */
    /*             goto unmatched; */
    /*         break; */
    /*     case RegexpString: */
    /*         { */
    /*             size_t matchLength = strlen(pmatch[i].data.p); */
    /*             if ((end - p) < matchLength */
    /*                 || strncmp(p, pmatch[i].data.p, matchLength) != 0) */
    /*                 goto unmatched; */
    /*             else */
    /*                 nconsumed = matchLength; */
    /*         } */
    /*         break; */
    /*     case RegexpWhitespace: */
    /*         if (isspace(*p) == 0) */
    /*             goto unmatched; */
    /*         break; */
    /*     } */
    /*     if (pmatch[i].repeat == REPEAT_ONE_OR_MORE) { */
    /*         ++nrepeat; */
    /*     } else if (nrepeat < pmatch[i].repeat) { */
    /*         ++nrepeat; */
    /*         p += nconsumed; */
    /*     } else { */
    /*         nrepeat = 1; */
    /*         ++i; */
    /*         p += nconsumed; */
    /*     } */
    /*     nconsumed = 1; */
    /*     continue; */
    /*   unmatched: */
    /*     if (pmatch[i].repeat == REPEAT_ONE_OR_MORE && nrepeat > 1) { */
    /*         ++i; */
    /*         ++p; */
    /*     } else { */
    /*         i = 0; */
    /*         ++p; */
    /*     } */
    /* } */
    /* if (i == nmatch) */
    /*     return p - data.p; */
    /* else */
    /*     return -1; */
}

/* int regexpMatch(struct Slice data, struct Regexp *pmatch, size_t nmatch) */
/* { */
/*     size_t i = 0; */
/*     char *end = data.p + data.n; */
/*     char *p = data.p; */
/*     size_t nrepeat = 1, nconsumed = 1; */
/*     while (i < nmatch && p != end) { */
/*         switch (pmatch[i].type) { */
/*         case RegexpAnyDigit: */
/*             if (isdigit(*p) == 0) */
/*                 goto unmatched; */
/*             break; */
/*         case RegexpChar: */
/*             if (*p != pmatch[i].data.c) */
/*                 goto unmatched; */
/*             break; */
/*         case RegexpString: */
/*             { */
/*                 size_t matchLength = strlen(pmatch[i].data.p); */
/*                 if ((end - p) < matchLength */
/*                     || strncmp(p, pmatch[i].data.p, matchLength) != 0) */
/*                     goto unmatched; */
/*                 else */
/*                     nconsumed = matchLength; */
/*             } */
/*             break; */
/*         case RegexpWhitespace: */
/*             if (isspace(*p) == 0) */
/*                 goto unmatched; */
/*             break; */
/*         } */
/*         if (pmatch[i].repeat == REPEAT_ONE_OR_MORE) { */
/*             ++nrepeat; */
/*         } else if (nrepeat < pmatch[i].repeat) { */
/*             ++nrepeat; */
/*             p += nconsumed; */
/*         } else { */
/*             nrepeat = 1; */
/*             ++i; */
/*             p += nconsumed; */
/*         } */
/*         nconsumed = 1; */
/*         continue; */
/*       unmatched: */
/*         if (pmatch[i].repeat == REPEAT_ONE_OR_MORE && nrepeat > 1) { */
/*             ++i; */
/*             ++p; */
/*         } else { */
/*             i = 0; */
/*             ++p; */
/*         } */
/*     } */
/*     if (i == nmatch) */
/*         return p - data.p; */
/*     else */
/*         return -1; */
/* } */

#define ANY_DIGIT(repeatcount)                          \
    { .func = matchAnyDigit, .repeat = repeatcount }
#define SPECIFIC_CHAR(whichchar,repeatcount)                        \
    { .func = matchSpecificChar, .repeat = repeatcount, .data = { .c = whichchar } }
#define SPECIFIC_STRING(str, repeatcount)       \
    { .func = matchString, .repeat = repeatcount, .data = { .p = str } }

struct Regexp match[] = {
    ANY_DIGIT(8),
    SPECIFIC_CHAR('-', 1),
    ANY_DIGIT(2),
    SPECIFIC_CHAR(':', 1),
    ANY_DIGIT(2),
    SPECIFIC_CHAR(':', 1),
    ANY_DIGIT(2),
    SPECIFIC_CHAR('.', 1),
    ANY_DIGIT(6),
    SPECIFIC_CHAR('-', 2),
};

struct Slice getPayload(struct Slice data)
{
    int nconsumed;
    struct Slice payload = data;
    if ((nconsumed =
         regexpMatch(payload, match,
                     sizeof match / sizeof match[0])) == -1) {
        return makeSlice(0, 0);
    }
    consumeBytes(&payload, nconsumed);
    struct Regexp matchTo = SPECIFIC_STRING("To", 1);
    struct Regexp matchFrom = SPECIFIC_STRING("From", 1);
    if ((nconsumed = regexpMatch(payload, &matchTo, 1)) == -1
        || (nconsumed = regexpMatch(payload, &matchFrom, 1)) == -1) {
        return makeSlice(0, 0);
    }
    consumeBytes(&payload, nconsumed);
    return payload;
}
