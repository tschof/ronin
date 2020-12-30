#include "message_layout.h"

message_layout_t *make_message_layout(size_t * fields, size_t num)
{
    message_layout_t *ret = malloc(sizeof *ret);
    if (ret != 0) {
        ret->ordered_fields = fields;
        ret->num = num;
        ret->capacity = num;
    }
    return ret;
}

message_layout_t *make_empty_message_layout(void)
{
    message_layout_t *ret = malloc(sizeof *ret);
    if (ret != 0) {
        ret->ordered_fields = 0;
        ret->num = 0;
        ret->capacity = 0;
    }
    return ret;
}

int append_to_message_layout(message_layout_t * layout, size_t field)
{
    int ret = -1;
    if (layout != 0) {
        if (layout->capacity == 0) {
            layout->capacity = 8;
            layout->ordered_fields =
                malloc(layout->capacity * sizeof(size_t));
        } else if (layout->capacity == layout->num) {
            layout->capacity *= 2;
            layout->ordered_fields =
                realloc(layout->ordered_fields,
                        layout->capacity * sizeof(size_t));
        }
        if (layout->ordered_fields != 0) {
            layout->ordered_fields[layout->num++] = field;
        }
        ret = 0;
    }
    return ret;
}
