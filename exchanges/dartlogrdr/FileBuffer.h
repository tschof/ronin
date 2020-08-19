#ifndef __FILEBUFFER_H__
#define __FILEBUFFER_H__

#include "Slice.h"
#include <stdlib.h>

struct FileBuffer;

struct FileBuffer *makeFileBuffer(const char *);
void destroyFileBuffer(struct FileBuffer *);
ssize_t readMore(struct FileBuffer *);
void repack(struct FileBuffer *);
slice *getReadData(struct FileBuffer *);
uint isEof(struct FileBuffer *);

#endif
