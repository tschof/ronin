#ifndef __GENERIC_FIX_CUSTOMIZATIONS_H__
#define __GENERIC_FIX_CUSTOMIZATIONS_H__

struct fix_interface {
    const char *config_file;
    int file_len;
    int (*onstart) (ex_config * exchange_config, orderbook * pbook);
    fieldxlatefunc pre_translate_func;
    char *(*id_generator) (size_t * nlen);
};

int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_interface *handler);

#endif
