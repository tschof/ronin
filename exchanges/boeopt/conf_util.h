#ifndef __EXCHANGE_INTERFACE_CONF_UTIL_H__
#define __EXCHANGE_INTERFACE_CONF_UTIL_H__

#include <stddef.h>
struct tm;
struct config_loader;

struct conf_map {
    const char *section;
    const char *key;
    char **value;
    size_t *value_length;
};

/* This is a table-driven approach to loading variables from the
 * configuration file. The idea is to simply declare an array of
 * struct conf_map, initializing them with the appropriate values,
 * then calling this and having it do all the work of mapping and
 * initializing variables. Think of it as a configuration mapping
 * engine. */
void conf_map_to_vars(struct config_loader *dc, struct conf_map *mapping,
                      size_t nmappings);

/* The return value is similar to snprintf, i.e. -1 on catastrophic
 * error, otherwise the number of bytes written, which should always
 * be 9 for the current date string (8 characters plus a null
 * terminator). The calling side should just check to see if the
 * return value is greater than the buffer size passed in; if so, call
 * it again passing in the return value buffer size or greater. */
int make_current_date_string(char *buf, size_t len, struct tm *local);

/* Returns 0 for false, 1 for true, takes a struct tm which contains
 * the current time, the hour and minute at which reset happens, and a
 * window, expressed in minutes, after which the reset time is valid,
 * i.e. the reset time is from hour:min to hour:(min+window) on the
 * current day. */
int should_reset(struct tm *current_local_time, int hour, int minute,
                 int window);

#endif
