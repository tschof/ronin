#include <stdlib.h>
#include <string.h>
#include "db_struct.h"


void gendb_destroy_v(gendb_v* g)
{
    if(g->type_in_union == DB_ARRAY &&
            g->arr_len > 0) 
    {
        free(g->g_un.un_array);
    }
    free(g);
}

int stringcopy(char* buf, int buf_len, const char* source)
{
    int rc = 0;
    
    if (strlen(source) < buf_len)
    {
        strcpy(buf, source);
        rc = strlen(source);
    }
    else
    {
        rc = buf_len - 1;
        strncpy(buf, source, rc);
        buf[rc] = '\0';
    }
    
    return rc;
}