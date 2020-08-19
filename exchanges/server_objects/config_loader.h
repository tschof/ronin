/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _CONFIG_LOADER_H__
#define _CONFIG_LOADER_H__

#if defined(__cplusplus)
extern "C" {
#endif

    typedef struct config_loader dart_config;

    dart_config *initialize_config(const char *filename, int len);

    char *get_val_for_tag(dart_config * df, const char *section,
                          int sec_len, const char *tag,
                          int tag_len, int *val_len);

    void set_val_for_tag(dart_config * df, const char *section,
                         int sec_len, const char *tag,
                         int tag_len, char *val, int val_len);

    void dart_load_map(dart_config * df);

    void dart_reset_maps(dart_config * df);

#if defined(__cplusplus)
}
#endif
#endif                          //_CONFIG_LOADER_H__
