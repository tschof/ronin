#include "conf_util.h"
#include "server_objects/config_loader.h"
#include <time.h>
#include <string.h>
#include <stdio.h>

void conf_map_to_vars(dart_config * dc, struct conf_map *mapping,
                      size_t nmappings)
{
    struct conf_map *end;
    for (end = mapping + nmappings; mapping != end; ++mapping) {
        if (mapping->value != 0)
            *mapping->value = 0;

        if (mapping->value_length != 0)
            *mapping->value_length = 0;

        if (mapping->section != 0 && mapping->key != 0
            && mapping->value != 0) {
            int value_length = -1;
            char *p = get_val_for_tag(dc, mapping->section,
                                      (int) strlen(mapping->section),
                                      mapping->key,
                                      (int) strlen(mapping->key),
                                      &value_length);
            if (p == 0)
                value_length = 0;
            else
                *mapping->value = p;

            if (mapping->value_length != 0)
                *(mapping->value_length) = value_length;
        }
    }
}

int make_current_date_string(char *buf, size_t len, struct tm *local)
{
    int retval = -1;
    struct tm tmptm;
    if (local == 0) {
        local = &tmptm;
    }
    time_t now = time(0);
    if (localtime_r(&now, local) != 0
        && snprintf(buf, len, "%04d%02d%02d", local->tm_year + 1900,
                    local->tm_mon + 1, local->tm_mday) <= len) {
        retval = 0;
    }
    return retval;
}

int should_reset(struct tm *current_local_time, int hour, int minute,
                 int window)
{
    int retval = 0;
    int current_min_from_midnight =
        (current_local_time->tm_hour * 60) + current_local_time->tm_min;
    int reset_min_from_midnight = (hour * 60) + minute;
    if ((current_min_from_midnight >= reset_min_from_midnight)
        && (current_min_from_midnight <=
            (reset_min_from_midnight + window))) {
        retval = 1;
    }
    return retval;
}
