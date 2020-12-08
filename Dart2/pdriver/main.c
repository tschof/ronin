#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "pilldirect.h"
#include "pillar.h"




static int is_mconnected = 0;
static int keep_going = 1;
static int ack = 0;
static void t_on_rej(pillcnlreprej* doj)
{
    printf("REJ: rejtype:%d, orderid: %lu \n", doj->rejtype, doj->clordid);
}
static void t_on_out(pillmodcnlack* doj)
{
    printf("OUT: acktype:%d, orderid: %lu \n", doj->acktype, doj->orig_clordid);
}
static void t_on_fill(pillexerpt* doj)
{
    printf("FILLED: shares:%d, orderid: %lu \n", doj->lastqty, doj->clordid);
    keep_going = 0;
}
static void t_on_ack(pillordack* doj) 
{
    ack = 1;
    printf("ACKED: account:%s, orderid: %lu \n", doj->mpid, doj->clordid);
}

static void t_on_connect(uint8_t is_connected)
{
    is_mconnected = is_connected;
    printf("Are we connected: %d, \n", is_connected);
}

/**
 *
typedef struct pillar_new_order_request {
    pmh header;
    uint32_t symbol;
    char mpid[4];
    uint32_t mmid;
    char mpsubid;
    uint64_t clordid;
    uint64_t orig_ord_key;
    uint64_t bitfield_instr;
    uint64_t price;
    uint32_t qty;
    uint32_t min_qty;
    char user_data[8];
    uint32_t optional;
} pillneword;

typedef struct pillar_cancel_request {
    pmh header;
    uint32_t symbol;
    char mpid[4];
    uint64_t clordid;
    uint64_t orig_ord_key;
} pillcancel;
 *
 */
int main(int argc, char *argv[])
{
    int ok = 1;
    int iterations = 4284;
    uint64_t i = 4184; 
    int j = 0;
    int ord_num = 1; 
    pillcon* dnb = start_pillar("MM8A", 4,
        "MM8A", 4,
        "glob.dat", 8,t_on_rej,
        t_on_out, t_on_fill, 
        t_on_ack, t_on_connect);
    sleep(20);
            for(;j < 1000; ++j) {
                run_pill_non_block_recv_loop(dnb->dnbp);
            }
            j = 0;
    if(is_mconnected) {
        pillneword* uno = calloc(1, sizeof(pillneword));
        pillcancel* unc = calloc(1,sizeof(pillcancel));
        pillequityadd* pilleadd = malloc(sizeof(pillequityadd));
        memset(pilleadd, '\0', sizeof(pillequityadd));
        pilleadd->header.length = 41;
        pilleadd->header.type = 0x0241;
        pilleadd->maxfloor = 100;
        //uno->symbol = 9914;
        uno->symbol = 10066;
        unc->symbol = 10066;
        //unc->symbol = 9914;
        memcpy(uno->user_data, "CRON026", 7);
        for(; i < iterations;++i) {
            uno->header.type = 0x0240;
            uno->header.length = 65;
            uno->clordid = i;
            uno->qty = 200;
            uno->price = (int64_t) 5693000000;
           // uno->price = (int64_t) 9293000000;
            memcpy(uno->mpid, "DCHF", 4);
            uno->bitfield_instr = 1297036971322900480;
            int8_t worked = send_pill_new_order(dnb, uno, pilleadd, 41);
            printf("Did it work? %d \n", worked);
            sleep(2);
            j = 0;
            for(;j < 100; ++j) {
                run_pill_non_block_recv_loop(dnb->dnbp);
            }
            if(worked >= 0) {
                unc->header.type = 0x0280;
                unc->header.length = 28;
                unc->clordid = i + iterations;
                unc->orig_ord_key = i;
                memcpy(unc->mpid, "DCHF", 4);
                worked = send_pill_cancel(dnb, unc);
                printf("Did cancel work? %d \n", worked);
                sleep(2);
                j = 0;
                for(;j < 100; ++j) {
                    run_pill_non_block_recv_loop(dnb->dnbp);
                }
            }
            ord_num++;
        }
    }
    i = 0;
    for(;i < 100800; ++i) {
        run_pill_non_block_recv_loop(dnb->dnbp);
    } //(ack == 0);
    sleep(30);
    shutdown_pillar(dnb->dnbp);
    return ok;
}


