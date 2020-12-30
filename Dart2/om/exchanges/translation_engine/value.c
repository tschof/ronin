#include "value.h"
#include "util.h"

void free_val(value_t * val)
{
    if (val == 0 || val->type == data_type_undefined) {
        return;
    }
    if (val->type == data_type_scalar) {
        free(val->data.scalar.iov_base);
    } else if (val->type == data_type_map) {
        free_string_map(val->data.map);
    }
    val->type = data_type_undefined;
}
