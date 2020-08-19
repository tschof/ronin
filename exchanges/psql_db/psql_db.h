/**************************************************************************
 * Copyright (c) 2016 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#ifndef _PSQL_DB_H__
#define _PSQL_DB_H__

#include "message_queue.h"
#include "db_struct.h"

#if defined(__cplusplus)
extern "C" {
#endif

    gendb_h* create_dbhandle(  const char* login, int login_len,
                                const char* password, int password_len,
                                const char* database, int database_len,
                                const char *dsnname, int dsnname_len,
                                const char *dbport, int dbport_len,
                                char *err, int err_len);

    void destroy_dbhandle(gendb_h* db_h);

    int gendb_exec(gendb_h* db_h,
                   const char *sql_stmt,
                   gendb_parm ** parms,
                   int parm_len, queue_t results, char *err, int err_len, int* bad_db);
    
    int gendb_exec1(gendb_h* db_h,
                   const char *sql_stmt,
                   gendb_parm ** parms,
                   int parm_len, 
                   void (*consumer) (queue_t results, void* owner), 
                   void* consumer_owner,
                   char *err, 
                   int err_len, 
                   int* bad_db);
    
    int gendb_nonquery(gendb_h* db_h,
                       const char *sql_stmt,
                       char *err, int err_len, int* bad_db);
    
    int gendb_get_results(const char *dsnname, int dsnname_len,
                          const char *dbport, int dbport_len,
                          const char* login, int login_len,
                          const char* password, int password_len,
                          const char* database, int database_len,
                          queue_t results, const char *sql_query,
                          char *err, int err_len);
    
    //int lets_hack(gendb_h* db_h,
     //             const char *sql_stmt,
    //              char *err, int err_len, int* bad_db);
    
#if defined(__cplusplus)
}
#endif
#endif                          //_PSQL_DB_H__
