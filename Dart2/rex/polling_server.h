


#ifndef POLLING_SERVER_H__
#define POLLING_SERVER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include "call_back_wrapper.h"
#include "message_queue.h"
#include "ex_config_helper.h"
#include "sock_recv_manager.h"
#include "log_mnger.h"
#include "hashmap.h"
#include "dart_order_obj.h"

typedef struct polling_server pserver;

#pragma pack(push, 1)
typedef struct user_logon_info {
    char name[32];
    char password[32];
    time_t last_logon;
    int nlen;
    int plen;
    int reset_hour;
    int reset_min;
    int reset_day;
    int create_new;
    int new_incoming;
    int new_outgoing;
    int do_balance;
    int cancel_on_dis;
} login_info;
#pragma pack(pop)

typedef struct rex_connection {
    ex_config* conf;
    pserver* p;
    otl* o;
    call_back_wrapper* read_wrapper;
    call_back_wrapper* write_wrapper;
    call_back_wrapper* connect_wrapper;
    sock_mnger* con;
    login_info* logon;
    void* con_pass_back;
    void (*write_callback)(void* pass_back);
    void (*connect)(void* ct);
    void (*connection_notice)(void* rc,
            int is_connected, dart_order_obj* logon);
    void (*parse)(int* cut_con, struct rex_connection* rc);
    login_info* (*is_valid)(void* rc, dart_order_obj* logon);
    void* parser;
    time_t stop_time;
    int disconnect;
    int removed;
    int is_server;
    int sock;
} rex_con;

void add_signal_listener(pserver* p, call_back_wrapper* cbw);

call_back_wrapper* add_timer(pserver* p, int repeat_int_sec,
        int repeat_int_nano, int init_int_sec, int init_int_nano,
        void* cb_obj, void (*t_cb_func)(void* cb));

void add_recon_timer(pserver* p, call_back_wrapper* cbw);

void disconnect_by_name(pserver* p, const char* name,
        int nlen);

void add_sender(pserver* p, rex_con* pb);

int check_is_connected(pserver* p, char* name, int nlen);

int remove_connection(pserver* p, rex_con* rc);

int add_connection(pserver* p, rex_con* rc);

void reconnect(void* rc);

pserver* start_lib_server(int num_threads,int to, int* cpus,
        int depth, otl* o);
void start_job(pserver* p, void(*user_job)(void* d), void* uj);
pserver* start_server(int num_threads, int time_out,
                             int* cpus, int depth, otl* o);

rex_con* add_listener(pserver* p, struct ex_con* ex,
                             void* parse_pass_back,void* con_pass_back,
                             void (*write_callback) (void* pass_back),
                             void (*connection_notice)(void* rc,
                                 int is_connected, dart_order_obj* logon),
                             void (*parse)(int* cut_con, rex_con* rc),
                             login_info* (*is_valid)(void* rc, dart_order_obj* logon));

rex_con* create_rex_con(pserver* p, struct ex_con* ex,
                             void* parse_pass_back,void* con_pass_back,
                             login_info* linfo,
                             void (*write_callback) (void* pass_back),
                             void (*connection_notice)(void* rc,
                                 int is_connected, dart_order_obj* logon),
                             void (*parse)(int* cut_con, rex_con* rc),
                             login_info* (*is_valid)(void* rc, dart_order_obj* logon));

void rex_connect(pserver* p, rex_con* rc);
void rex_direct_connect(pserver* p, rex_con* rc);

void join_n_clean(pserver* p);

void stop_system(pserver* p);

void add_back_rep_timer(pserver* p, call_back_wrapper* cbw);

void add_back_rep_timer_adjust(pserver* p, call_back_wrapper* cbw, int new_sec);
otl* get_logger(pserver* p);

int gen_send(rex_con* rc, char* data, int len, int* disconnect);
void create_new_conversation(rex_con* rc);
#if defined(__cplusplus)
}
#endif      
#endif
