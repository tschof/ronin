#include "FileBuffer.h"
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

struct FileBuffer {
    char *buffer;
    size_t bufferSize, lowWaterMark;
    struct Slice validRegion;
    int fd;
    uint eof;
};

struct FileBuffer *allocFileBuffer(int fd, size_t hint)
{
    struct FileBuffer *fileBuffer = malloc(sizeof *fileBuffer);
    fileBuffer->bufferSize = hint > 0 ? hint : 4096;
    fileBuffer->lowWaterMark = fileBuffer->bufferSize / 4;
    fileBuffer->buffer = fileBuffer->validRegion.p =
        malloc(fileBuffer->bufferSize);
    fileBuffer->fd = fd;
    fileBuffer->validRegion.n = 0;
    fileBuffer->eof = 0;
    return fileBuffer;
}

struct FileBuffer *makeFileBuffer(const char *filename)
{
    int fd = open(filename, 0);
    if (fd == -1) {
        perror(filename);
        return 0;
    }
    return allocFileBuffer(fd, 4096);
}

static void reallocIfNeeded(struct FileBuffer *fileBuffer)
{
    if ((fileBuffer->bufferSize - fileBuffer->validRegion.n) < fileBuffer->lowWaterMark) {
        fileBuffer->bufferSize *= 2;
        fileBuffer->buffer = fileBuffer->validRegion.p =
            realloc(fileBuffer->buffer, fileBuffer->bufferSize);
    }
}

ssize_t readMore(struct FileBuffer *fileBuffer)
{
    repack(fileBuffer);
    reallocIfNeeded(fileBuffer);
    ssize_t nread = read(fileBuffer->fd,
                         fileBuffer->validRegion.p +
                         fileBuffer->validRegion.n,
                         fileBuffer->bufferSize -
                         fileBuffer->validRegion.n);
    if (nread > 0) {
        fileBuffer->validRegion.n += nread;
        return nread;
    } else if (nread == 0) {
        /* End of file */
        fileBuffer->eof = 1;
        return 0;
    } else {
        perror("reading from file");
        return -1;
    }
}

void repack(struct FileBuffer *fb)
{
    memmove(fb->buffer, fb->validRegion.p, fb->validRegion.n);
    fb->validRegion.p = fb->buffer;
}

slice *getReadData(struct FileBuffer *fb)
{
    return &fb->validRegion;
}

void freeFileBuffer(struct FileBuffer *fileBuffer)
{
    free(fileBuffer->buffer);
    free(fileBuffer);
}

void destroyFileBuffer(struct FileBuffer *fileBuffer)
{
    if (fileBuffer->fd > 0 && close(fileBuffer->fd) == -1)
        perror("closing file");
    freeFileBuffer(fileBuffer);
}

uint isEof(struct FileBuffer *fb)
{
    return fb->eof;
}
