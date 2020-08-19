#include "FileFilter.h"
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <regex.h>
#include <stdlib.h>

struct FileFilter {
    regex_t fileRegex;
};

struct FileFilter *makeFileFilter(const char *regex)
{
    struct FileFilter *filter = malloc(sizeof *filter);
    int regexResult = regcomp(&filter->fileRegex, regex, REG_EXTENDED);
    if (regexResult != 0) {
        free(filter);
        filter = 0;
        errno = regexResult;
    }
    return filter;
}

int applyFilter(struct FileFilter *filter, const char *directoryName,
                onFileCallback callback, void *callbackArg)
{
    DIR *handle = opendir(directoryName);
    if (handle == 0)
        return -1;
    struct dirent *entry;
    while ((entry = readdir(handle)) != 0)
        if (regexec(&filter->fileRegex, entry->d_name, 0, 0, 0) == 0)
            callback(callbackArg, directoryName, entry->d_name);
    if (closedir(handle) == -1)
        return -1;
    return 0;
}

void freeFileFilter(struct FileFilter *filter)
{
    regfree(&filter->fileRegex);
    free(filter);
}
