#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif


#include <math.h>
#include <sched.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/syscall.h>
#include <sys/wait.h>

// DART specfic
#include "dart_constants.h"
#include "ex_config_helper.h"
#include "fix_handler.h"
#include "fix_trans.h"
#include "connection_interface.h"

#ifdef SHM
// for shared memory tests
#include <sys/mman.h>
#include <sys/stat.h>		/* For mode constants */
#include <fcntl.h>		/* For O_* constants */
#include "shm.h"
#endif

int g_connected = 0;
int g_run = 1;
int g_num_cbk = 0;		// number of callbacks from library

/*
 * This will be called after you have been connected to the other
 * side of the fix connection.
 */
static void con_call_back(struct async_parse_args * apa, int is_connected, char *log,
			  int len)
{
    printf("connection: %s, is %s connected\n", apa->in_args->name,
	   (is_connected ? "" : "NOT"));
    if (is_connected) {
	g_connected = 1;
    } else {
	//fix_connection_callback(co, is_connected);
	g_connected = 0;
    }
    con_interface *ci = (con_interface *) apa->ds_book;
    if (ci) {
        if (ci->ap == NULL) {
            ci->ap = apa;
        }
        ci->on_connect(ci, is_connected);
    }
}




/*
 * callback function called whenever the underlying library 
 * receives an application message.
 */
static void tm_from_fix_func(dart_order_obj *msg, void *client_obj)
{
    ++g_num_cbk;
    //fprintf(stdout, "\n%s(%d): %.80s\n", __FILE__, __LINE__, msg->data);
}
static ff_desc* create_ice_ff_out_array(int* len, char* dir, int d_len)
{
    char* full_path = calloc(1, d_len + 9);
    ff_desc* ffd = calloc(16, sizeof(struct fix_file_desc));
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "NewOrder", 8);
    ffd[0].filename = full_path;
    full_path = calloc(1, d_len + 7);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Cancel", 6);
    ffd[1].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Replace", 7);
    ffd[2].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "GapFill", 7);
    ffd[3].filename = full_path;
    full_path = calloc(1, d_len + 6);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Logon", 5);
    ffd[4].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Logout", 6);
    ffd[5].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "ResendReq", 9);
    ffd[6].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "SecDefReq", 9);
    ffd[7].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "TestReq", 7);
    ffd[8].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "TraderLogon", 11);
    ffd[9].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Heartbeat", 9);
    ffd[10].filename = full_path;
    *len = 11;
    return ffd;
}

static ff_desc* create_ice_ff_in_array(int* len, char* dir, int d_len)
{
    char* full_path = calloc(1, d_len + 16);
    ff_desc* ffd = calloc(10, sizeof(struct fix_file_desc));
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomExe", 11);
    ffd[0].filename = full_path;
    ffd[0].fix_type = 0x38;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomCnlRej", 14);
    ffd[1].filename = full_path;
    ffd[1].fix_type = 0x39;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomNORej", 13);
    ffd[2].filename = full_path;
    ffd[2].fix_type = 0x44;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomLogon", 13);
    ffd[3].filename = full_path;
    ffd[3].fix_type = 0x41;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomSecDef", 14);
    ffd[4].filename = full_path;
    ffd[4].fix_type = 0x64;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomMail", 12);
    ffd[5].filename = full_path;
    ffd[5].fix_type = 0x42;
    //New
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomBizRej", 14);
    ffd[6].filename = full_path;
    ffd[6].fix_type = 0x6A;
    //
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomSesRej", 14);
    ffd[7].filename = full_path;
    ffd[7].fix_type = 0x33;
    //done
    *len = 8;
    return ffd;
}

static con_interface* set_interface(async_server_con_set* async_server,
        gatekeeper* gk, char* filename, int f_len )
{
    con_interface* con = create_empty_interface();
    ex_config *ex = init_config(filename, f_len);//("ice.dat", 7);
    init_args* fix_args = load_fix_config(filename, f_len);
    reset_con_interface(con, 128,
            (char *) ex->mod_name, ex->mod_name_len,
            fix_args->target_comp, fix_args->tc_len,
            send_fix_logon, 0,
            0, 0,
            0, gk, (char *) ex->dest_id,
            ex->dest_id_len);
    con->ass = async_server;
    con->gk = gk;
    con->client_mess_parse = parse_fix_message;
    con->ci_parse_v = 0;
    con->on_connect = on_connect;
    con->ci_obj_callback= tm_from_fix_func;
    con->connection_conf = ex;
    con->can_send_orders = 1;
    con->session_conf = fix_args;
    con->on_reject = 0;
    con->on_replaced = 0;
    con->on_out = 0;
    con->on_cancel_all = 0;
    con->on_fill = 0;
    con->ap = 0;
    con->recon_wrapper = 0;
    int ret_len = 0;
    int out_len = 0;
    int in_len = 0;
    char* directory = get_val_for_tag(ex->dc, "FIX SERVER", 10, 
                                      "directory", 9, &ret_len);
    ff_desc* out_types = create_ice_ff_out_array(&out_len, directory, ret_len);
    ff_desc* in_types = create_ice_ff_in_array(&in_len, directory, ret_len);
    trans_t* t = create_translator(out_types, out_len, in_types, in_len);
    con->parser = t;
    con->tg = generic_creation_func();
    con->sym_trans = 0;
    return con;
}
static void usage(void)
{
    fprintf(stdout, "\nusage:\n");
    fprintf(stdout, "\tmock_tmlight TargetCompID SenderCompID");
    fprintf(stdout,
	    "[version=FIX.4.2] [server=127.0.0.1] [port=7890]\n\n");
    fflush(stdout);
}

/* system signal handler */
void catch_int(int sig_num)
{
    fprintf(stdout, "<Ctrl-C>\n\n");
    fflush(stdout);
    g_run = 0;
}
/*
 * construct and send a FIX order to the DART library
 */
static inline void send_order(ofp *fix_obj, const unsigned int seqnum,
                              con_interface* m, trans_t* t, struct tm* tmval)
{
    add_value(t, fix_obj, "11", 2, seqnum);
    add_value(t, fix_obj, "58", 2, seqnum);
    //set_fix_val(t, fix_obj, 58, i_and_a, len);
    set_char_fix_val(t, fix_obj, "1", 1, "RONIN", 5); 
    set_char_fix_val(t, fix_obj, "44", 2, "3.1415926", 9); 
    set_char_fix_val(t, fix_obj, "40", 2, "2", 1); 
    set_char_fix_val(t, fix_obj, "54", 2, "2", 1); 
    set_char_fix_val(t, fix_obj, "59", 2, "0", 1); 
    char symbol[3][5] = { "ESZ3", "ESH4", "ESM4" };
    set_char_fix_val(t, fix_obj, "107", 3, symbol[seqnum % 3], 4);
    set_char_fix_val(t, fix_obj, "55", 2, "ES", 2);
    set_char_fix_val(t, fix_obj, "167", 3, "FUT", 3);
    add_time_tag(t, fix_obj, tmval, 60);
    add_value(t, fix_obj, "38", 2, (1+seqnum%10));
    dart_send_fix_message(fix_obj, m);
    reset_object(t, fix_obj);
}
/*
 * hot thread that spins reading shared memory and when the data is
 * ready, sends a FIX order
 */
static void run_thread(con_interface* m, async_server_con_set* ass)
{
    cpu_set_t cpuset;
	if (!g_connected) {
        g_connected = async_connection(ass,
                         m->connection_conf->mod_name,
                         m->connection_conf->mod_name_len,
                         m->connection_conf->fix_port,
                         m->connection_conf->fix_ip,
                         m->connection_conf->fix_ip_len,
                         m->connection_conf->end_hour,
                         m->connection_conf->end_min,
                         m->connection_conf->end_day,
                         m->connection_conf->heart_beat_int,
                         m,con_call_back,
                         m->client_mess_parse,
                         m->session_conf,
                         m->connect,
                         m,
                         ci_async_write_callback,
                         m->ci_parse_v,
                         m->ci_obj_callback);
	}

    /* hard coded to run on CPU 4 */
    pthread_t thread = pthread_self();
    CPU_ZERO(&cpuset);
    CPU_SET(4, &cpuset);
    pthread_setaffinity_np(thread, sizeof(cpu_set_t), &cpuset);


    trans_t* t = (trans_t*)m->parser;
    ofp* fix_obj = get_fixy_message(t, 0x44);
	static time_t last = 0;
	time_t now;
    struct tm tmval;
#ifdef SHM
    /* construct order from shared mememory (SHM) */
    porder_data_t porder = (porder_data_t) init_shm(sizeof(order_data_t));
    printf("\nstart run thread: order: %p\n", porder);
    while (g_run) {
	if (porder->dirty) {
	    if (g_connected) {
            time(&now);
            gmtime_r(&now, &tmval);
            send_order(fix_obj, porder->seqno, m, t, &tmval);
	    }
	    //printf( "ORDER: seqno=%d\n", porder->seqno);
	    porder->dirty = 0;
	}
    }
#else
    {
    	while ( g_run ) {
	    time(&now);
	    if ( difftime(now, last) > 1 && g_connected) {
            gmtime_r(&now, &tmval);
            send_order(fix_obj, now, m, t, &tmval);
            last = now;
	    }
    	}
    }
#endif
}

int main(int argc, char **argv)
{
    if (argc < 1) {
	usage();
	return 0;
    }
    signal(SIGINT, catch_int);

    char *filename = argc > 1 ? argv[1] : "cme.dat";
    size_t f_len = strlen(filename);
    int* cpus = calloc(2, sizeof(int));
    cpus[0] = 2;
    cpus[1]  = 3;
    rec_args* ra = init_recorder(2, 2, ".", 1);
    gatekeeper* gk = create_empty_gatekeeper(2, "ChiefKeef",9, NULL,
                                       1);
    async_server_con_set* async_server = start_async_server(gk, ra,
                                                            2, 0, cpus, 2);
    con_interface *fix_if = set_interface(async_server,gk,filename, f_len);
    run_thread(fix_if, async_server);
	printf("num messages recevive: %d\n", g_num_cbk);
    exit(EXIT_SUCCESS);
}
