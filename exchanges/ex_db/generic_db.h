/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _GENERIC_DB_H__
#define _GENERIC_DB_H__

#include "message_queue.h"
#include "db_struct.h"

#if defined(__cplusplus)
extern "C" {
#endif

/*#define DB_DOUBLE  1
#define DB_INT 2
#define DB_CHAR 3
#define DB_ARRAY 4
#define DB_NULL 0


    typedef struct gendb_var {
        int type_in_union;
        int arr_len;
        union {
            double un_double;
            int un_int;
            char un_char;
            char* un_array;
        } g_un;
    } gendb_v;

   
    typedef struct gendb gendb_h;

    typedef struct {
        //char name[128];
        char* name;
        gendb_v value;
    } gendb_parm;*/

    int gendb_get_results(const char *user, int user_len,
                          const char *pass, int p_len,
                          const char *server, int s_len,
                          queue_t results, const char *sql_query);


    gendb_h *create_dbhandle(const char *user,
                             int user_len,
                             const char *pass,
                             int p_len, const char *server, int s_len);
    
    gendb_h* create_dbhandle1(  const char* login, int login_len,
                                const char* password, int password_len,
                                const char* database, int database_len,
                                const char *dsnname, int dsnname_len,
                                const char *dbport, int dbport_len,
                                char *err, int err_len);

    int destroy_dbhandle(gendb_h * db_h);

    //void gendb_init(void(* handler)(const char* errText))
    void gendb_init();
    //void gendb_destroy();
    //void gendb_destroy_v(gendb_v* g);
    int gendb_exec(gendb_h * db_h,
                   const char *sql_query,
                   gendb_parm ** parms,
                   int parm_len, queue_t results, char *err, int err_len, int* bad_db);
/*
    int gendb_exec_s(const gendb_h * db_h,
                     const char *sql_query,
                     gendb_sparm * parms, int parm_len, queue_t results);

    gendb_h *create_dbhandle_a(const char *user,
                               int user_len,
                               const char *pass,
                               int p_len, const char *server, int s_len);

    int destroy_dbhandle_a(gendb_h * db_h);

    int gendb_exec_a(const gendb_h * db_h,
                     const char *sql_query,
                     gendb_parm * parms, int parm_len);
*/
#if defined(__cplusplus)
}
#endif
#endif                          //_GENERIC_DB_H__
