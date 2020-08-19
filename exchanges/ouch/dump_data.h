#ifndef __DUMP_DATA_H__
#define __DUMP_DATA_H__

#include <stdlib.h>

struct databuf;

typedef int (*dump_inner_data) (void *, size_t, struct databuf *);
typedef int (*dump_outer_data) (void *, size_t, struct databuf *,
                                dump_inner_data);

#endif
