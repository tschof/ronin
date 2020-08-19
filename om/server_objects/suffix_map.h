/**************************************************************************
 * Copyright (c) 2015 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _SUFFIX_MAP_H__
#define _SUFFIX_MAP_H__

typedef struct symbol_suffix_mapper suffixm;

suffixm* create_suffix_map(char* filename);
suffixm* create_suffix_n_flip(char* filename, char* flip_name);

const char* get_suffix(suffixm* smap, char* rom_suffix, int rlen,
                       int* slen);
char* get_flip_account(suffixm* smap, char* account,
        int alen, int *slen);


#endif //_SUFFIX_MAP_H__


