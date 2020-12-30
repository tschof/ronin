#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "nbcon.h"
#include "utp.h"



static int is_mconnected = 0;
static int keep_going = 1;
static int ack = 0;
static void t_on_rej(utprej* doj)
{
}
static void t_on_out(utpout* doj)
{
    printf("OUT: account:%s, orderid: %s \n", doj->account, doj->orig_clordid);
}
static void t_on_fill(utpfill* doj)
{
    printf("FILLED: account:%s, orderid: %s \n", doj->account, doj->clordid);
    keep_going = 0;
}
static void t_on_vfill(utpvfill* doj)
{
}
static void t_on_ack(utpack* doj) 
{
    ack = 1;
    printf("ACKED: account:%s, orderid: %s \n", doj->account, doj->clordid);
}
static void t_on_cnl_ack(utpcnlack* doj)
{
    printf("CNL_REJ: account:%s, orderid: %s \n", doj->account, doj->clordid);
}

static void t_on_connect(uint8_t is_connected)
{
    is_mconnected = is_connected;
    printf("Are we connected: %d, \n", is_connected);
}
int main(int argc, char *argv[])
{
    int ok = 1;
    int i = 0; 
    int j = 0;
    int ord_num = 1; 
    dnbcon* dnb = start_dart("MM8", 3,
        "MM8", 3,
        "glob.dat", 8,t_on_rej,
        t_on_out, t_on_fill, t_on_vfill,
        t_on_ack, t_on_cnl_ack, t_on_connect);
    sleep(30);
    if(is_mconnected) {
        utpno* uno = calloc(1, sizeof(utpno));
        utpcnl* unc = calloc(1,sizeof(utpcnl));
        for(; i < 100;++i) {
            uno->type = 0x0041;
            uno->type = htons(uno->type);
            uno->len = sizeof(utpno);
            uno->len = htons(uno->len);
            uno->seq = ord_num;
            uno->qty = 100;
            uno->qty = htonl(uno->qty);
            uno->max_floor = 0;
            uno->price = 190000;
            uno->price = htonl(uno->price);
            uno->scale ='4';
            memcpy(uno->sym, "CRG", 3);    
            uno->side = ord_num % 2 == 0 ? '1' : '5';
            uno->mtype = '2';
            uno->tif = '0';
            uno->cap = 'A';
            sprintf(uno->clordid, "IOI %04d/20170501", ord_num);
            //memcpy(uno->clordid, "TGH 0008/20170501", 17);
            uint8_t worked = send_new_order(dnb, uno);
            printf("Did it work? %d \n", worked);
            sleep(2);
            j = 0;
            for(;j < 100; ++j) {
                run_non_block_recv_loop(dnb->dnbp);
            }
            unc->type = 0x0061;
            unc->type = htons(unc->type);
            unc->len = sizeof(utpcnl);
            unc->len = htons(unc->len);
            sprintf(unc->clordid, "POI %04d/20170501", ord_num);
            memcpy(unc->orig_clordid, uno->clordid, 17);
            worked = send_cancel(dnb, unc);
            printf("Did cancel work? %d \n", worked);
            sleep(2);
            j = 0;
            for(;j < 100; ++j) {
                run_non_block_recv_loop(dnb->dnbp);
            }
            ord_num++;
        }
    }
    i = 0;
    for(;i < 800; ++i) {
        run_non_block_recv_loop(dnb->dnbp);
    } //(ack == 0);
    /*utpcnl* unc = calloc(1,sizeof(utpcnl));
    unc->type = 0x0061;
    unc->type = htons(unc->type);
    unc->len = sizeof(utpcnl);
    unc->len = htons(unc->len);
    memcpy(unc->orig_clordid, uno->clordid, 17);
    sprintf(unc->clordid, "HGH %04d/20170501", ord_num);
    ord_num++;
    worked = send_cancel(dnb, unc);
    do {
    run_non_block_recv_loop(dnb->dnbp);
    }while(keep_going);*/
    //run_spin_recv_loop(dnb->dnbp);
    sleep(30);
    shutdown_dart(dnb);
    return ok;
}

