/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _DART_ALLOCATOR_H__
#define _DART_ALLOCATOR_H__

#if defined(__cplusplus)
extern "C" {
#endif
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>

#define DART_PG_SIZE 4096

unsigned int next_power_of_two(unsigned int size);
typedef struct dart_mem_glob dallocator;
/**
 * The allocator allocates memory one page
 * at a time and hands it out one "size"
 * chunk at a time. 
 */
dallocator* create_mem_glob(unsigned int size, void init_func(void *));
/**
 * A mem glob will now return a chunk
 * of memory to you at the size requested
 * when the glob was created.
 */
void* get_stuff(dallocator* dmg);
/**
 * In order to avoid unessisary allocations
 * return the stuff after you are done with it.
 */
void return_stuff(dallocator* dmg, void*);
/**
 * Clean up the whole mess, be sure you do not
 * have any pointers to stuff you may have gotten
 * as they will now be invalid.
 */
void destroy_mem_glob(dallocator* dmg);


#define DART_ALLOC(data,alignment, size, alloc_ret)                    \
    alloc_ret = posix_memalign((void**)&data, alignment, size);  \
    if(alloc_ret == EINVAL) {                           \
        printf("Alignment was not power of 2\n"); \
    } else if(alloc_ret == ENOMEM) {                    \
        printf("No memory for align.\n");         \
    } else {                                      \
        memset(data, '\0', size);                 \
    }

#if defined(__cplusplus)
}
#endif
#endif





