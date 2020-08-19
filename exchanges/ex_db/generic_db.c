/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/

#define	EX_CTLIB_VERSION	CS_VERSION_125

#define MAXSTRING   40

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctpublic.h>
#include <pthread.h>

#include "cstypes.h"
#include "generic_db.h"
#include "hashmap.h"
#include "version.h"

static pthread_mutex_t client_callback_lock = PTHREAD_MUTEX_INITIALIZER;
/*
** Callback routines for library errors and server messages.
*/
CS_RETCODE CS_PUBLIC csmsg_callback2(CS_CONTEXT * context,
                                     CS_CLIENTMSG * msg);

CS_RETCODE CS_PUBLIC client_callback(CS_CONTEXT * context,
                                     CS_CONNECTION * connection,
                                     CS_CLIENTMSG * msg);

CS_RETCODE CS_PUBLIC server_callback(CS_CONTEXT * context,
                                     CS_CONNECTION * connection,
                                     CS_SERVERMSG * msg);

/* struct gendb {
    CS_CONTEXT *context;
    CS_CONNECTION *connection;
    char errtext[1024];
    int msgcode;
};*/

typedef struct gendb {
    CS_CONTEXT *context;
    CS_CONNECTION *connection;
    char errtext[1024];
    int msgcode;
} tdsgendb;

//void(* g_handler)(const char* text) = NULL;
hashmap_t g_gendbmap = NULL;

typedef struct _dgdb_data {
    CS_INT indicator;
    CS_CHAR *value;
    CS_INT valuelen;
} DGDB_DATA;

static void get_threadid_str(char *buffer, int size)
{
    pthread_t id = pthread_self();
    char idstr[30];
    sprintf(idstr, "%lu", id);
    strncpy(buffer, idstr, size - 1);
    buffer[size - 1] = '\0';
}


/*void gendb_destroy_v(gendb_v* g)
{
    if(g->type_in_union == DB_ARRAY &&
            g->arr_len > 0) {
        free(g->g_un.un_array);
    }
    free(g);
}*/

static int dgdb_binder(DGDB_DATA * data, CS_DATAFMT * column, gendb_v * g)
{

    switch ((int) column->datatype) {
    case CS_SMALLINT_TYPE:
    case CS_TINYINT_TYPE:
    case CS_INT_TYPE:
        {
            if (data->valuelen > 0) {
                int type = 0;
                g->type_in_union = DB_INT;
                memcpy(&type, data->value, data->valuelen);
                g->g_un.un_int = type;
            } else {
                g->type_in_union = DB_NULL;
                g->g_un.un_int = 0;
            }
            g->arr_len = 0;

        }
        break;
    case CS_FLOAT_TYPE:
        {
            if (data->valuelen > 0) {
                double t = 0;
                g->type_in_union = DB_DOUBLE;
                memcpy(&t, data->value, sizeof(t));
                g->g_un.un_double = t;
            } else {
                g->type_in_union = DB_NULL;
                g->g_un.un_double = 0;
            }
            g->arr_len = 0;
        }
        break;
    default:
        {
            if (data->value != NULL && data->valuelen > 0) {
                g->type_in_union = DB_ARRAY;
                g->g_un.un_array = malloc(data->valuelen);
                memcpy(g->g_un.un_array, data->value, data->valuelen);
                g->arr_len = data->valuelen;
            } else {
                g->type_in_union = DB_NULL;
                g->arr_len = 0;
            }
        }
        break;
    }
    return 0;
}

static int set_params(CS_COMMAND * cmd, gendb_parm ** parms, int parm_len)
{
    CS_RETCODE retCode = CS_SUCCEED;
    CS_DATAFMT dataFmt;
    gendb_parm *pPar;
    int i = 0;

    for (i = 0; i < parm_len; i++) {
        pPar = parms[i];
        memset((void *) &dataFmt, '\0', sizeof(CS_DATAFMT));
        strcpy(dataFmt.name, pPar->name);
        dataFmt.namelen = strlen(dataFmt.name);
        dataFmt.status = CS_INPUTVALUE;
        switch (pPar->value.type_in_union) {
        case DB_INT:
            dataFmt.datatype = CS_INT_TYPE;
            retCode = ct_param(cmd,
                               &dataFmt,
                               (CS_VOID *) & pPar->value.g_un.un_int,
                               CS_UNUSED, 0);

            if (retCode != CS_SUCCEED) {
            }
            break;

        case DB_DOUBLE:
            dataFmt.datatype = CS_FLOAT_TYPE;
            retCode = ct_param(cmd,
                               &dataFmt,
                               (CS_VOID *) & pPar->value.g_un.un_double,
                               CS_UNUSED, 0);

            if (retCode != CS_SUCCEED) {
            }
            break;
        case DB_ARRAY:
            dataFmt.datatype = CS_CHAR_TYPE;
            dataFmt.format = CS_FMT_NULLTERM;
            retCode = ct_param(cmd,
                               &dataFmt,
                               (CS_VOID *) pPar->value.g_un.un_array,
                               strlen(pPar->value.g_un.un_array),
                               //UNION_ARRAY_LEN,
                               0);
            if (retCode != CS_SUCCEED) {
            }
            break;
        default:
            retCode = -1;
        }
    }

    return retCode;
}


static int fetchstatus(CS_COMMAND * cmd)
{
    int retCode = 1;
    CS_INT num_cols = 0;
    CS_DATAFMT datafmt;
    CS_RETCODE statuscode = 0;
    CS_INT rowsRead;
    //CS_CHAR* value = NULL;

    retCode = ct_res_info(cmd, CS_NUMDATA, &num_cols, CS_UNUSED, NULL);

    if (retCode != CS_SUCCEED) {
        printf("Failed to get Column data. \n");
    } else {
        //  only 1 'row' and 'col' expected
        if (num_cols > 0) {
            //CS_CHAR* value = NULL;

            retCode = ct_describe(cmd, 1, &datafmt);
            if (retCode != CS_SUCCEED) {
                printf("Failed to describe col: %d\n", 0);
            }

            retCode = ct_bind(cmd, 1, &datafmt, &statuscode, NULL, NULL);

            //retCode = ct_bind(cmd, 1, &datafmt,
            //                  &statuscode, NULL, NULL);

            if (CS_SUCCEED != retCode) {
                printf("ct_bind failed\n");
                retCode = 0;
            } else {
                retCode = ct_fetch(cmd,
                                   CS_UNUSED,
                                   CS_UNUSED, CS_UNUSED, &rowsRead);

                if (CS_SUCCEED != retCode) {
                    printf("ct_fetch failed\n");
                    retCode = 0;
                } else {
                    //memcpy(&retCode, value, sizeof(int));
                    //if (0 != statuscode)
                    //{
                    //  retCode = 0;
                    //}

                    //need to exhaust the fetch
                    while (CS_SUCCEED == ct_fetch(cmd,
                                                  CS_UNUSED,
                                                  CS_UNUSED,
                                                  CS_UNUSED, &rowsRead)) {

                    }

                }
            }
        }
    }

    return retCode;
}

int gendb_get_results(const char *user, int user_len,
                      const char *pass, int p_len,
                      const char *server, int s_len,
                      queue_t results, const char *sql_query)
{
    int num_cols = 0;
    CS_CONTEXT *context;
    CS_CONNECTION *connection;
    CS_COMMAND *cmd = 0;
    CS_DATAFMT *datafmt = 0;
    DGDB_DATA *coldata = 0;
    CS_INT count;
    CS_RETCODE ret;
    CS_INT result_type;
    CS_RETCODE results_ret;
    int i = 0;

    context = (CS_CONTEXT *) NULL;
    ret = cs_ctx_alloc(EX_CTLIB_VERSION, &context);
    if (ret != CS_SUCCEED) {
        printf("cs_ctx_alloc failed!!\n");
        return -1;
    }

    ret = ct_init(context, EX_CTLIB_VERSION);
    if (ret != CS_SUCCEED) {
        printf("cs_init failed!!\n");
        return -1;
    }

    ret = ct_callback(context, NULL, CS_SET, CS_CLIENTMSG_CB,
                      client_callback);

    if (ret != CS_SUCCEED) {
        printf("cs_callback(client_callback) failed!!\n");
        return 0;
    }

    ret = ct_callback(context, NULL, CS_SET, CS_SERVERMSG_CB,
                      server_callback);

    if (ret != CS_SUCCEED) {
        printf("cs_callback(server_callback) failed!!\n");
        return 0;
    }

    ret = cs_config(context, CS_SET, CS_MESSAGE_CB,
                    csmsg_callback2, CS_UNUSED, NULL);
    if (ret != CS_SUCCEED) {
        printf("cs_config failed!!\n");
    }
    ret = ct_con_alloc(context, &connection);
    if (ret != CS_SUCCEED) {
        printf("ct_con_alloc failed!!\n");
    }
    ret = ct_con_props(connection, CS_SET, CS_USERNAME,
                       (char *) user, CS_NULLTERM, NULL);
    if (ret != CS_SUCCEED) {
        printf("could not set username failed!!\n");
    }
    ret = ct_con_props(connection, CS_SET, CS_PASSWORD,
                       (char *) pass, CS_NULLTERM, NULL);
    if (ret != CS_SUCCEED) {
        printf("failed to set password!!\n");
    }
    ret = ct_connect(connection, (char *) server, s_len);
    if (ret != CS_SUCCEED) {
        printf("Failed to connect!!\n");
    }
    ret = ct_cmd_alloc(connection, &cmd);
    if (ret != CS_SUCCEED) {
        printf("cmd_alloc failed!!\n");
    }
    ret =
        ct_command(cmd, CS_LANG_CMD, (char *) sql_query, CS_NULLTERM,
                   CS_UNUSED);
    if (ret != CS_SUCCEED) {
        printf("ct_command failed!!\n");
    }
    ret = ct_send(cmd);
    if (ret != CS_SUCCEED) {
        printf("FAILED  ct send: \n");
    }
    while ((results_ret = ct_results(cmd, &result_type)) == CS_SUCCEED) {
        switch ((int) result_type) {
        case CS_ROWFMT_RESULT:
            {
                printf("Got a Rowformat result \n");
            }
            break;
        case CS_ROW_RESULT:
            {
                ret = ct_res_info(cmd, CS_NUMDATA, &num_cols,
                                  CS_UNUSED, NULL);
                if (ret != CS_SUCCEED) {
                    printf("Failed to get Column data. \n");
                }
                if (num_cols > 0) {
                    datafmt = calloc(1,num_cols * sizeof(CS_DATAFMT));
                    coldata = calloc(1,num_cols * sizeof(DGDB_DATA));
                    printf("Got this many columns: %d\n", num_cols);
                    for (; i < num_cols; ++i) {
                        ret = ct_describe(cmd, (i + 1), &datafmt[i]);
                        if (ret != CS_SUCCEED) {
                            printf("Failed to describe col: %d\n", i);
                        }
                        coldata[i].value = malloc(datafmt[i].maxlength + 1);
                        ret = ct_bind(cmd, (i + 1), &datafmt[i],
                                      coldata[i].value,
                                      &coldata[i].valuelen,
                                      (CS_SMALLINT *) &
                                      coldata[i].indicator);
                        if (ret != CS_SUCCEED) {
                            printf("Failed to bind col: %d\n", i);
                        }
                    }
                }
                while (((ret = ct_fetch(cmd, CS_UNUSED, CS_UNUSED,
                                        CS_UNUSED, &count)) == CS_SUCCEED)
                       || (ret == CS_ROW_FAIL)) {
                    if (ret == CS_ROW_FAIL) {
                        //printf("ERROR on ROW %ld\n", (count + 1));
                    } else {
                        queue_t q = create_sized_queue(60);
                        enqueue(results, q);
                        for (i = 0; i < num_cols; ++i) {
                            gendb_v *v = malloc(sizeof(gendb_v));
                            dgdb_binder(&coldata[i], &datafmt[i], v);
                            enqueue(q, v);
                            //memset(&coldata[i].value, 0, coldata[i].valuelen);
                        }
                    }
                }

                for (i = 0; i < num_cols; i++) {
                    free(coldata[i].value);
                }

                if (datafmt) {
                    free(datafmt);
                    datafmt = 0;
                }
                if (coldata) {
                    free(coldata);
                    coldata = 0;
                }
            }
            break;
        case CS_CMD_SUCCEED:
            printf("No rows returned.\n");
            break;
        case CS_CMD_FAIL:
            printf("Got a CMD FAIL.\n");
            break;
        case CS_CMD_DONE:
            //printf("Got a CMD Done.\n");
            break;
        case CS_STATUS_RESULT:
            break;
        default:
            printf("ct_results returned unexpected result type.\n");
            break;
        }
    }

    switch ((int) results_ret) {
    case CS_END_RESULTS:
        //printf("ct_results as expected!\n");
        break;
    case CS_FAIL:
        printf("ct_results returned a fail.\n");
        break;
    default:
        printf("ct_results returned unexpected return code\n");
        break;
    }

    if (cmd) {
        ct_cancel(NULL, cmd, CS_CANCEL_ALL);
        ret = ct_cmd_drop(cmd);
    }

    if (ret != CS_SUCCEED) {
        printf("ct_cmd_drop failed!!\n");
    }
    ret = ct_close(connection, CS_UNUSED);
    if (ret != CS_SUCCEED) {
        printf("ct_close failed!!\n");
    }
    ret = ct_con_drop(connection);
    if (ret != CS_SUCCEED) {
        printf("ct_con_drop failed!!\n");
    }
    ret = ct_exit(context, CS_UNUSED);
    if (ret != CS_SUCCEED) {
        printf("ct_exit failed!!\n");
    }
    ret = cs_ctx_drop(context);
    if (ret != CS_SUCCEED) {
        printf("ct_ctx_drop failed!!\n");
    }

    return 0;
}

CS_RETCODE CS_PUBLIC csmsg_callback2(CS_CONTEXT * cp, CS_CLIENTMSG * emsgp)
{
    printf("csmsg_callback2 called!!");
    return (CS_SUCCEED);
}


CS_RETCODE CS_PUBLIC client_callback(CS_CONTEXT * cp,
                                     CS_CONNECTION * connection,
                                     CS_CLIENTMSG * msg)
{
    char idstr[30];
    tdsgendb *pDbh = NULL;

    get_threadid_str(idstr, sizeof(idstr));

    pthread_mutex_lock(&client_callback_lock);

    if (get_map_entry(g_gendbmap, idstr, strlen(idstr), (void **) (&pDbh))) {
        int destsize = sizeof(pDbh->errtext);
        memset(pDbh->errtext, '\0', destsize);
        strncpy(pDbh->errtext, msg->msgstring, destsize - 1);
        pDbh->msgcode = msg->msgnumber;

        /*if (msg->msgnumber != 0)
        {
            if (g_handler)
            {
                char msg[1200];
                sprintf(msg, "client_callback:code=%d, text=%s\n", pDbh->msgcode, pDbh->errtext);
                g_handler(msg);
            }
        }
        */
    }

    pthread_mutex_unlock(&client_callback_lock);

    return (CS_SUCCEED);
}

CS_RETCODE CS_PUBLIC server_callback(CS_CONTEXT * cp,
                                     CS_CONNECTION * connection,
                                     CS_SERVERMSG * msg)
{
    char idstr[30];
    tdsgendb *pDbh = NULL;

    if (msg->msgnumber != 0 &&
        msg->msgnumber != 5701 &&
        msg->msgnumber != 5703 &&
        msg->msgnumber != 5704)
    {
        get_threadid_str(idstr, sizeof(idstr));

        pthread_mutex_lock(&client_callback_lock);

        if (get_map_entry(g_gendbmap, idstr, strlen(idstr), (void **) (&pDbh))) {
            int destsize = sizeof(pDbh->errtext);
            memset(pDbh->errtext, '\0', destsize);
            pDbh->msgcode = msg->msgnumber;
            char* p = pDbh->errtext;
            sprintf(p, "errCode=%d: ", pDbh->msgcode);
            strncpy(p + strlen(p), msg->text, destsize - strlen(p) - 1);


            /*
            if (msg->msgnumber != 0)
            {
                if (g_handler)
                {
                    char msg[1200];
                    sprintf(msg, "server_callback:code=%d, text=%s\n", pDbh->msgcode, pDbh->errtext);
                    g_handler(msg);
                }
            }
            */
        }

        pthread_mutex_unlock(&client_callback_lock);
    }

    return (CS_SUCCEED);
}

//void gendb_init(void(* handler)(const char* errText))
//{
//    g_gendbmap = create_map(100);
//    g_handler = handler;
//}

void gendb_init()
{
    g_gendbmap = create_map(100);
}

/*void gendb_destroy()
{
    delete_map(g_gendbmap);
    pthread_mutex_destroy(&client_callback_lock);
}*/

gendb_h *create_dbhandle(const char *user,
                         int user_len,
                         const char *pass,
                         int p_len, const char *server, int s_len)
{
    char idstr[30];
    tdsgendb *pdb = (tdsgendb *) calloc(1, sizeof(tdsgendb));
    CS_RETCODE ret = CS_SUCCEED;
    pdb->context = (CS_CONTEXT *) NULL;
    ret = cs_ctx_alloc(EX_CTLIB_VERSION, &pdb->context);

    if (ret != CS_SUCCEED) {
        printf("cs_ctx_alloc failed!!\n");
        return NULL;
    }

    ret = ct_init(pdb->context, EX_CTLIB_VERSION);
    if (ret != CS_SUCCEED) {
        printf("cs_init failed!!\n");
        return NULL;
    }

    ret = ct_callback(pdb->context, NULL, CS_SET, CS_CLIENTMSG_CB,
                      client_callback);

    if (ret != CS_SUCCEED) {
        printf("cs_callback(client_callback) failed!!\n");
        return NULL;
    }

    ret = ct_callback(pdb->context, NULL, CS_SET, CS_SERVERMSG_CB,
                      server_callback);

    if (ret != CS_SUCCEED) {
        printf("cs_callback(server_callback) failed!!\n");
        return NULL;
    }

    ret = ct_con_alloc(pdb->context, &pdb->connection);
    if (ret != CS_SUCCEED) {
        printf("ct_con_alloc failed!!\n");
        return NULL;
    }
    ret = ct_con_props(pdb->connection, CS_SET, CS_USERNAME,
                       (char *) user, CS_NULLTERM, NULL);
    if (ret != CS_SUCCEED) {
        printf("could not set username failed!!\n");
        return NULL;
    }
    ret = ct_con_props(pdb->connection, CS_SET, CS_PASSWORD,
                       (char *) pass, CS_NULLTERM, NULL);
    if (ret != CS_SUCCEED) {
        printf("failed to set password!!\n");
        return NULL;
    }

    ret = ct_connect(pdb->connection, (char *) server, s_len);
    if (ret != CS_SUCCEED) {
        printf("Failed to connect!!\n");
        return NULL;
    }

    get_threadid_str(idstr, sizeof(idstr));

    pthread_mutex_lock(&client_callback_lock);
    no_copy_insert(g_gendbmap, idstr, strlen(idstr), pdb);
    pthread_mutex_unlock(&client_callback_lock);

    return pdb;
}

gendb_h* create_dbhandle1(  const char* login, int login_len,
                                const char* password, int password_len,
                                const char* database, int database_len,
                                const char *dsnname, int dsnname_len,
                                const char *dbport, int dbport_len, 
                                char *err, int err_len)
{
    gendb_h* pdb = create_dbhandle(login, login_len,
                                   password, password_len,
                                   dsnname, dsnname_len);
    
    if (pdb)
    {
        char usedbCmd[128];
        queue_t results = create_queue();
        sprintf(usedbCmd, "use %s", database);
        int bad_db = 0;

        gendb_h* rc = (gendb_h*)pdb;
        
        if  (gendb_exec(rc,
                        usedbCmd,
                        NULL,
                        0,
                        results,
                        err,
                        err_len,
                        &bad_db) == -1)
        {
            stringcopy(err, err_len, "'use dbname' failed\n");
            destroy_dbhandle(pdb);
            pdb = NULL;
        }

        destroy_queue(results);
    }

    return pdb;
}

int destroy_dbhandle(gendb_h * db_h)
{
    tdsgendb* db_hh = (tdsgendb*)db_h;

    CS_RETCODE ret = ct_close(db_hh->connection, CS_UNUSED);
    if (ret != CS_SUCCEED) {
        printf("ct_close failed!!\n");
    }
    ret = ct_con_drop(db_hh->connection);
    if (ret != CS_SUCCEED) {
        printf("ct_con_drop failed!!\n");
    }
    ret = ct_exit(db_hh->context, CS_UNUSED);
    if (ret != CS_SUCCEED) {
        printf("ct_exit failed!!\n");
    }
    ret = cs_ctx_drop(db_hh->context);
    if (ret != CS_SUCCEED) {
        printf("ct_ctx_drop failed!!\n");
    }
    //get_threadid_str(idstr, sizeof(idstr));
    //remove_from_map(g_gendbmap, idstr, strlen(idstr));
    free(db_hh);
    return ret;
}


int gendb_exec(gendb_h * db_h,
               const char *sql_query,
               gendb_parm ** parms,
               int parm_len,
               queue_t results,
               char *err,
               int err_len,
               int* bad_db)
{
    tdsgendb* db_hh = (tdsgendb*)db_h;

    CS_COMMAND *cmd = 0;
    CS_INT num_cols = 0;
    int cmd_failed = 0;
    int i = 0;
    CS_INT count;
    CS_DATAFMT *datafmt = 0;
    DGDB_DATA *coldata = 0;
    CS_INT result_type;
    CS_RETCODE results_ret;

    *bad_db = 0;

    CS_RETCODE retCode = ct_cmd_alloc(db_hh->connection, &cmd);
    if (retCode != CS_SUCCEED) {
        printf("cmd_alloc failed!!\n");
        *bad_db = 1;
        return -1;
    }
    //clear the error code
    db_hh->msgcode = 0;

    if (0 < parm_len) {
        retCode = ct_command(cmd, CS_RPC_CMD,
                             (char *) sql_query, CS_NULLTERM,
                             CS_NO_RECOMPILE);

        if (retCode == CS_SUCCEED) {
            set_params(cmd, (gendb_parm **) parms, parm_len);
        }
        else
        {
            *bad_db = 1;
        }
    } else {
        retCode = ct_command(cmd, CS_LANG_CMD,
                             (char *) sql_query, CS_NULLTERM, CS_UNUSED);
    }

    if (retCode != CS_SUCCEED) {
        printf("ct_command failed!!\n");
        *bad_db = 1;
        return -1;
    }
    retCode = ct_send(cmd);
    if (retCode != CS_SUCCEED) {
        printf("FAILED  ct send: \n");
        *bad_db = 1;
        return -1;
    }

    if (NULL == results) {
        return retCode;
    }

    while ((results_ret = ct_results(cmd, &result_type)) == CS_SUCCEED) {
        switch ((int) result_type) {
        case CS_ROWFMT_RESULT:
            {
                printf("Got a Rowformat result \n");
            }
            break;
        case CS_ROW_RESULT:
            {
                retCode = ct_res_info(cmd, CS_NUMDATA, &num_cols,
                                      CS_UNUSED, NULL);
                if (retCode != CS_SUCCEED) {
                    printf("Failed to get Column data. \n");
                }
                if (num_cols > 0) {
                    datafmt = calloc(1,num_cols * sizeof(CS_DATAFMT));
                    coldata = calloc(1,num_cols * sizeof(DGDB_DATA));
                    printf("Got this many columns: %d\n", (int) num_cols);
                    for (i = 0; i < num_cols; ++i) {
                        retCode = ct_describe(cmd, (i + 1), &datafmt[i]);
                        if (retCode != CS_SUCCEED) {
                            printf("Failed to describe col: %d\n", i);
                        }
                        coldata[i].value = malloc(datafmt[i].maxlength + 1);
                        retCode = ct_bind(cmd, (i + 1), &datafmt[i],
                                          coldata[i].value,
                                          &coldata[i].valuelen,
                                          (CS_SMALLINT *) &
                                          coldata[i].indicator);
                        if (retCode != CS_SUCCEED) {
                            printf("Failed to bind col: %d\n", i);
                        }
                    }
                }
                while (((retCode = ct_fetch(cmd, CS_UNUSED, CS_UNUSED,
                                            CS_UNUSED,
                                            &count)) == CS_SUCCEED)
                       || (retCode == CS_ROW_FAIL)) {
                    if (retCode == CS_ROW_FAIL) {
                        //printf("ERROR on ROW %ld\n", (count + 1));
                    } else {
                        queue_t q = create_queue();
                        enqueue(results, q);
                        for (i = 0; i < num_cols; ++i) {
                            gendb_v *v = malloc(sizeof(gendb_v));
                            dgdb_binder(&coldata[i], &datafmt[i], v);
                            enqueue(q, v);
                            //memset(&coldata[i].value, 0, coldata[i].valuelen);
                        }
                    }
                }

                for (i = 0; i < num_cols; i++) {
                    free(coldata[i].value);
                }

                if (datafmt) {
                    free(datafmt);
                    datafmt = 0;
                }
                if (coldata) {
                    free(coldata);
                    coldata = 0;
                }
            }
            break;
        case CS_CMD_SUCCEED:
            //printf("No rows returned.\n");
            break;
        case CS_CMD_FAIL:
            printf("Got a CMD FAIL.\n");
            break;
        case CS_CMD_DONE:
            //printf("Got a CMD Done.\n");
            break;
        case CS_STATUS_RESULT:

            if (fetchstatus(cmd) == 0 || db_hh->msgcode != 0) {
                strncpy(err, db_hh->errtext, err_len);
                cmd_failed = 1;
            }

            break;
        default:
            //if (0 == skip)
            //{
            printf("ct_results returned unexpected result type.\n");
            //}
            break;
        }
    }


    if (db_hh->msgcode != 0)
    {
        strncpy(err, db_hh->errtext, err_len);
        if (db_hh->msgcode == 1205) //dead lock condition on db server.
        {
            cmd_failed = -1;
            retCode = 0;
        }
        else
        {
            cmd_failed = 1;
            retCode = -1;
        }
        db_hh->msgcode = 0;
        retCode = -1;
    }

    switch ((int) results_ret) {
    case CS_END_RESULTS:
        if (cmd_failed == 1)
        {
            retCode = -1;
        }
        else if (cmd_failed == -1) //recoverable condition
        {
            retCode = 0;
        }
        break;
    case CS_FAIL:
        printf("ct_results returned a fail.\n");
        retCode = -1;
        break;
    default:
        printf("ct_results returned unexpected return code\n");
        retCode = -1;
        break;
    }

    if (cmd) {
        ct_cancel(NULL, cmd, CS_CANCEL_ALL);
        ct_cmd_drop(cmd);
    }

    return retCode;
}

