#ifndef DBEXECUTOR_H
#define	DBEXECUTOR_H

#include <string>
#include <vector>
//#include "generic_db.h"
//#include "psql_db.h"
#include "DBParm.h"
#include "ConfigBase.h"
#include "message_queue.h"

class CDBExecutor
{
public:

    enum RETURN_CODE
    {
        DB_FAILED = -1,
        EXEC_FAILED,
        EXEC_SUCCEED,
    };

    CDBExecutor(const tDBLogin& db);
    ~CDBExecutor();

    int Execute(const std::string& sqlstr, gendb_parm ** parms, int parm_len, queue_t result, std::string& errStr);
    bool DbUp();

private:
    gendb_h* (*create_dbhandle)(  const char* login, int login_len,
                                const char* password, int password_len,
                                const char* database, int database_len,
                                const char *dsnname, int dsnname_len,
                                const char *dbport, int dbport_len,
                                char *err, int err_len);

    void (*destroy_dbhandle)(gendb_h* db_h);
    int (*gendb_exec)(gendb_h* db_h,
                   const char *sql_stmt,
                   gendb_parm ** parms,
                   int parm_len, queue_t results, char *err, int err_len, int* bad_db);
    
    void ConnectDB();

    pthread_mutex_t m_Lock;
    tDBLogin m_dblogin;
    gendb_h* m_dbCon;
    void* m_so_handle;
};

#endif	/* DBEXECUTOR_H */

