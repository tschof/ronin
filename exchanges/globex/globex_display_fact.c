/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "globex_display_fact.h"
#include "fix_rom_trans.h"
hashmap_t load_display_factors(const char *filename)
{
    hashmap_t h = create_map(1024);
    build_map_from_file(filename, h);
    return h;
}

hashmap_t load_clearing_ids(const char* filename)
{
    hashmap_t h = create_map(8);
    build_map_from_file(filename, h);
    return h;
}

hashmap_t load_sender_sub_ids(const char* filename)
{
    hashmap_t h = create_map(32);
    build_map_from_file(filename, h);
    return h;
}
