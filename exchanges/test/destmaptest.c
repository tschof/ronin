#include "tests.h"
#include "testmacros.h"
#include "destmap.h"
#include <string.h>

int destmaptest(const char **testname, const char **extrareporting)
{
    STARTTEST;
    const char *buffer = "236:TSES,237:CDNX";
    struct destmap_t *destmap = destmap_fromstr(buffer);
    REQCOND(destmap != 0);
    char *exdest = 0;
    int findresult = destmap_find(destmap, "236", &exdest);
    CHKCOND(findresult == 0);
    if (findresult == 0) {
        CHKCOND(strcmp(exdest, "TSES") == 0);
    }
    findresult = destmap_find(destmap, "237", &exdest);
    CHKCOND(findresult == 0);
    if (findresult == 0) {
        CHKCOND(strcmp(exdest, "CDNX") == 0);
    }
    findresult = destmap_find(destmap, "100", &exdest);
    CHKCOND(findresult == -1);
    buffer = "236:TSES";
    destmap = destmap_fromstr(buffer);
    REQCOND(destmap != 0);
    findresult = destmap_find(destmap, "236", &exdest);
    CHKCOND(findresult == 0);
    if (findresult == 0) {
        CHKCOND(strcmp(exdest, "TSES") == 0);
    }
    STOPTEST;
}
