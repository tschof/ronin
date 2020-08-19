#ifndef __DB_STRUCT_H__
#define __DB_STRUCT_H__

#if defined(__cplusplus)
extern "C" {
#endif

#define DB_DOUBLE  1
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

typedef struct {
        //char name[128];
        char* name;
        gendb_v value;
} gendb_parm;
    
typedef void gendb_h;

typedef struct {
    char* login;
    char* password;                            
    char* database;                          
    char* dbserver; 
    int login_len;
    int password_len;
    int database_len;
    int dbserver_len;
} db_login;

void gendb_destroy_v(gendb_v* g);
int stringcopy(char* buf, int buf_len, const char* source);

#if defined(__cplusplus)
}
#endif
#endif
