/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/


#define MAXSTRING   40

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>

#include "libpq-fe.h"
#include "postgres_fe.h"
#include "server/catalog/pg_type.h"
#include "generic_db.h"
#include "hashmap.h"
void gendb_destroy(gendb_v* g)
{
    if(g->type_in_union == DB_ARRAY &&
            g->arr_len > 0) {
        free(g->g_un.un_array);
    }
    free(g);
}
/*****************************
 *
 * PSQL Functions
 *
 */


static int data_binder(gendb_v * g, Oid datatype, char* data, int datalen)
{
    int rc = 1;
    g->arr_len = 0;
    
    switch(datatype)
    {
        //currently we only use 4 bytes integer in our tables!!!
        case INT8OID:
            g->type_in_union = DB_LONG;
            g->g_un.un_long = atol(data);
            break;
        case INT2OID:  
        case INT4OID:
            g->type_in_union = DB_INT;
            g->g_un.un_int = atoi(data);
            break;
        case BPCHAROID:
            g->type_in_union = DB_CHAR;
            g->g_un.un_char = *data;
            break;
        //currently we only use the 8 bytes double in our tables!!!
        //case FLOAT4OID:
        case FLOAT8OID:
            g->type_in_union = DB_DOUBLE;
            g->g_un.un_double = atof(data);
            break;
        case VARCHAROID:
        case CHAROID:
        default:
            //g->type_in_union = DB_ARRAY;
            if (data != NULL && datalen > 0) 
            {
                g->type_in_union = DB_ARRAY;

                //trim trailing spaces
                int len = datalen;
                char* p = data;
                while (len > 0 && isspace(*p))
                {
                    p++;
                    len--;
                }
                while (len > 0 && isspace(data[len - 1]))
                {
                    len--;
                }
                if (len)
                {
                    g->g_un.un_array = malloc(len);
                    memcpy(g->g_un.un_array, p, len);
                    g->arr_len = len;
                }
                else
                {
                    g->type_in_union = DB_NULL;
                    g->arr_len = 0;
                }
            } 
            else 
            {
                g->type_in_union = DB_NULL;
                g->arr_len = 0;
            }
            break;
    }
     
    return rc;
}

int gendb_exec(gendb_h* db_h,
               const char *sql_stmt,
               gendb_parm ** parms,
               int parm_len,
               queue_t results,
               char *err,
               int err_len,
               int* bad_db)
{
    int rc = 0;
    char* errStr = NULL;
    
    PGresult *res = PQexec((PGconn*)db_h, sql_stmt);
   
    ExecStatusType status = PQresultStatus(res);//postgres doc suggests calling PQresultStatus
                                                //even PQexec returns null!!!

    if (status != PGRES_FATAL_ERROR)
    {
        if (status == PGRES_TUPLES_OK)
        {
            int nFields = PQnfields(res);
            int nRows = PQntuples(res);

            for (int i = 0; i < nRows; i++)
            {
                queue_t q = create_queue();
                enqueue(results, q);

                for (int j = 0; j < nFields; j++)
                {
                    gendb_v *v = calloc(1, sizeof(gendb_v));

                    if (PQgetisnull(res, i, j))
                    {
                         v->arr_len = 0;
                         v->type_in_union = DB_NULL;
                    }
                    else
                    {
                        Oid type = PQftype(res, j);
                        char* data = PQgetvalue(res, i, j);
                        
                        data_binder(v, type, data, strlen(data));
                    }

                    enqueue(q, v);
                }
            }
        }
        else if (status != PGRES_COMMAND_OK) //SQL failed
        {
            errStr = PQresultErrorMessage(res);
            rc = -1;
        }
    }
    else //out-of-memory or connection is bad!!!
    {
        errStr = PQerrorMessage((PGconn*)db_h);
        //*bad_db = 1;
        rc = -1;
    }
    
    PQclear(res);
    
    if (-1 == rc)
    {
      int len = strlen(errStr);
      int cp_len = len >= err_len ? (err_len -1): len;
      memcpy(err, errStr, cp_len);
      rc = cp_len;
    }
    
    return rc;
}

void destroy_dbhandle(gendb_h* db_h)
{
    PQfinish((PGconn*)db_h);
}

gendb_h* create_dbhandle(  const char* login, int login_len,
                            const char* password, int password_len,
                            const char* database, int database_len,
                            const char *dsnname, int dsnname_len,
                            char *err, int err_len)
{
    char connectStr[1024];
     
    char* p = connectStr;
    
    if (dsnname != NULL && dsnname_len > 0)
    {
        memcpy(p, "host=", 5);
        p += 5;
        memcpy(p, dsnname, dsnname_len);
        p += dsnname_len;
        memcpy(p, " ", 1);
        p += 1;
    }
    
    memcpy(p, "port=5432 ", 10);
    p += 10;
    
    memcpy(p, "dbname=", 7);
    p += 7;
    memcpy(p, database, database_len);
    p += database_len;
    memcpy(p, " ", 1);
    p += 1;
    
    memcpy(p, "user=", 5);
    p += 5;
    memcpy(p, login, login_len);
    p += login_len;
    memcpy(p, " ", 1);
    p += 1;
    
    memcpy(p, "password=", 9);
    p += 9;
    memcpy(p, password, password_len);
    p += password_len;
    memcpy(p, " ", 1);
    p += 1;
   
    *p = '\0';
    
    PGconn *conn = PQconnectdb(connectStr);
        
    if (PQstatus(conn) != CONNECTION_OK)
    {
        char* errStr = PQerrorMessage(conn);
        if (strlen(errStr) < err_len)
        {
            strcpy(err, errStr);
        }
        else
        {
            err[err_len - 1] = '\0';
            strncpy(err, errStr, err_len - 1);
        }
        PQfinish(conn);
        conn = NULL;
    }
   
    return (gendb_h*)conn;
}

int psql_gendb_get_results(const char* login, int login_len,
                      const char* password, int password_len,
                      const char* database, int database_len,
                      const char *dsnname, int dsnname_len,
                      queue_t results, const char *sql_query,
                      char *err, int err_len)
{
    int rc = 1;

    gendb_h* hd = create_dbhandle(  login, login_len, password, password_len,
                                    database, database_len, dsnname, dsnname_len,
                                    err, err_len);
    if (hd)
    {
        int bd = 0;
        rc = gendb_exec(hd, sql_query, NULL, 0, results, err, err_len, &bd);
        destroy_dbhandle((PGconn*)hd);
    }
    else
    {
        rc = -1;
    }

    return rc;
}
