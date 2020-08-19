#ifndef __FILEOPS_H__
#define __FILEOPS_H__

struct FileFilter;

/* Takes an extended regular expression string as required by glibc's
 * regexp engine. */
struct FileFilter *makeFileFilter(const char *regexStr);

/* Callback function provided on each iteration of applying the file
 * filter.  Passes a private data token, the directory and the file
 * name. */
typedef void (*onFileCallback) (void *, const char *dirName,
                                const char *fileName);

/* Iterate over all the files in a particular directory, applying the
 * regex specified when making the FileFilter, calling back to the
 * callback function on every successful match. */
int applyFilter(struct FileFilter *filter, const char *dirName,
                onFileCallback callback, void *callbackArg);

void freeFileFilter(struct FileFilter *filter);

#endif
