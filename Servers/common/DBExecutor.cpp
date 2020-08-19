#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "DBExecutor.h"
#include "dlfcn.h"

CDBExecutor::CDBExecutor(const tDBLogin& db)
{
    pthread_mutex_init(&m_Lock, NULL);
    m_dblogin = db;
    m_dbCon = NULL;
    
    if (0 == db.dbtype)
    {
        m_so_handle = dlopen("libdartdb_obj.so", RTLD_NOW);
    }
    else
    {
        m_so_handle = dlopen("libdartpsqldb_obj.so", RTLD_NOW);
    }
        
    void *tmpfuncptr = NULL;
    
    if (m_so_handle)
    {
        if (0 == db.dbtype)
        {
            tmpfuncptr = dlsym(m_so_handle, "create_dbhandle1");
        }
        else 
        {
            tmpfuncptr = dlsym(m_so_handle, "create_dbhandle");
        }
        memcpy(&create_dbhandle, &tmpfuncptr, sizeof tmpfuncptr);
        tmpfuncptr = dlsym(m_so_handle, "destroy_dbhandle");
        memcpy(&destroy_dbhandle, &tmpfuncptr, sizeof tmpfuncptr);
        tmpfuncptr = dlsym(m_so_handle, "gendb_exec");
        memcpy(&gendb_exec, &tmpfuncptr, sizeof tmpfuncptr);
        
        ConnectDB();
    }
    else
    {
        fprintf(stderr, "%s\n", dlerror());
        exit(EXIT_FAILURE);
    }
}

CDBExecutor::~CDBExecutor()
{
    pthread_mutex_destroy(&m_Lock);

    if (m_dbCon)
    {
        destroy_dbhandle(m_dbCon);
        m_dbCon = NULL;
    }
}

void CDBExecutor::ConnectDB()
{
    if (!m_dbCon)
    {
        char err[256];
                
        if ((m_dbCon = create_dbhandle(m_dblogin.user.c_str(), 
                                       m_dblogin.user.length(), 
                                       m_dblogin.password.c_str(), 
                                       m_dblogin.password.length(), 
                                       m_dblogin.database.c_str(),
                                       m_dblogin.database.length(),
                                       m_dblogin.dbserver.c_str(), 
                                       m_dblogin.dbserver.length(),
                                       m_dblogin.dbserver_port.c_str(),
                                       m_dblogin.dbserver_port.length(),
                                       err, sizeof(err))) == NULL)
        {
            //log db connect failure
            printf("create_dbhandle failed:%s\n", err);
        }
        /*else
        {
            char usedbCmd[128], errStr[1024];
            queue_t results = create_queue();
            sprintf(usedbCmd, "use %s", m_dblogin.database.c_str());
            int bad_db = 0;

            if  (gendb_exec(m_dbCon,
                            usedbCmd,
                            NULL,
                            0,
                            results,
                            errStr,
                            sizeof(errStr),
                            &bad_db) == -1)
            {
                printf("'use dbname' failed\n");
            }

            destroy_queue(results);
        }*/
    }
}

int CDBExecutor::Execute(const std::string& sqlstr, gendb_parm ** parms, int parm_len, queue_t result, std::string& errStr)
{
    int rc = EXEC_SUCCEED;

    pthread_mutex_lock(&m_Lock);

    if (!m_dbCon)
    {
        ConnectDB();
    }

    if (m_dbCon)
    {
        char errstr[128];
        int bad_db = 0;

        if  (gendb_exec(m_dbCon,
                        sqlstr.c_str(),
                        parms,
                        parm_len,
                        result,
                        errstr,
                        sizeof(errstr),
                        &bad_db) == -1)
        {
            errStr = errstr;
            printf("gendb_exec failed\n");
            if (bad_db)
            {
                destroy_dbhandle(m_dbCon);
                m_dbCon = NULL;
                rc = DB_FAILED;
            }
            else
            {
                rc = EXEC_FAILED;
            }
        }
    }
    else
    {
        rc = DB_FAILED;
    }

    pthread_mutex_unlock(&m_Lock);

    return rc;
}

bool CDBExecutor::DbUp()
{
    bool rc = false;

    pthread_mutex_lock(&m_Lock);
    if (m_dbCon)
    {
        rc = true;
    }
    pthread_mutex_unlock(&m_Lock);

    return rc;
}