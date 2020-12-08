/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _GENERIC_DB_H__
#define _GENERIC_DB_H__

#include "message_queue.h"

#define DB_DOUBLE  1
#define DB_INT 2
#define DB_CHAR 3
#define DB_ARRAY 4
#define DB_LONG 5
#define DB_NULL 0


typedef struct gendb_var {
    int type_in_union;
    int arr_len;
    union {
        double un_double;
        int32_t un_int;
        int64_t un_long;
        char un_char;
        char* un_array;
    } g_un;
} gendb_v;


void gendb_destroy(gendb_v* g);
/*PSQL Functions and structures*/

typedef struct {
        //char name[128];
        char* name;
        gendb_v value;
} gendb_parm;

typedef void gendb_h;
gendb_h* create_dbhandle(  const char* login, int login_len,
                            const char* password, int password_len,
                            const char* database, int database_len,
                            const char *dsnname, int dsnname_len,
                            char *err, int err_len);

void destroy_dbhandle(gendb_h* db_h);

int gendb_exec(gendb_h* db_h,
               const char *sql_stmt,
               gendb_parm ** parms,
               int parm_len, queue_t results, char *err, int err_len, int* bad_db);
int psql_gendb_get_results(const char *dsnname, int dsnname_len,
                      const char* login, int login_len,
                      const char* password, int password_len,
                      const char* database, int database_len,
                      queue_t results, const char *sql_query,
                      char *err, int err_len);
#endif
