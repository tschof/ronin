/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>

#include "boe_master.h"


#define BOE_LOGOUT 0x08
void boe_direct_send(boem* up, void* data, int len)
{
    int bytes_sent = 0;
    if(pthread_spin_lock(&up->send_mutex) == 0) {
        time(&up->last_sent);
        int disconnect = 0;
        bytes_sent = gen_send(up->rc, data, len,
                &disconnect);
        if(bytes_sent < len && !disconnect) {
            databuf_memcpy(up->mess_to_send, data + bytes_sent,
                    (len - bytes_sent));
            add_sender(up->rc->p, up->rc);
        } else if(disconnect) {
            up->rc->disconnect = disconnect;
            up->is_logged_on = 0;
        }
        pthread_spin_unlock(&up->send_mutex);
    } 
}

static int should_send_hb(boem* ci)
{
    time_t now = 0;
    time(&now);
    return (now - ci->last_sent >= 1);
}
static void send_boe_hb(void* pc)
{
    boem *ob = (boem *) pc;
    char buff[256];
    read(ob->hb_timer->sock,buff, 256); 
    if(should_send_hb(ob)) {
        databuf_t *buff = get_databuf(ob->gk);
        memset(buff->buffer, '\0', 10);
        *buff->wr_ptr++ = 0xBA;
        *buff->wr_ptr++ = 0xBA;
        short len = 8;
        databuf_memcpy(buff, &len, 2);
        *buff->wr_ptr++ = 0x03;
        *buff->wr_ptr++ = 0x0;
        buff->wr_ptr += 4;
        boe_direct_send(ob, buff->rd_ptr, databuf_unread(buff));
        destroy_gk_db(ob->gk, buff);
    }
    add_back_rep_timer(ob->rc->p,ob->hb_timer);
}

void send_boe_logout(boem* ob)
{
    databuf_t *buff = get_databuf(ob->gk);
    memset(buff->buffer, '\0', 10);
    *buff->wr_ptr++ = 0xBA;
    *buff->wr_ptr++ = 0xBA;
    short len = 8;
    databuf_memcpy(buff, &len, 2);
    *buff->wr_ptr++ = 0x02;
    *buff->wr_ptr++ = 0x0;
    buff->wr_ptr += 4;
    boe_direct_send(ob, buff->rd_ptr, databuf_unread(buff));
    destroy_gk_db(ob->gk, buff);
}
static void start_boe_hb(boem* pc)
{
    pc->hb_timer =  add_timer(pc->rc->p, 1, 0, 1,
                             0, pc,
                             send_boe_hb);
}
void parse_boe(int *cut_con, rex_con *pc)
{
    boem* bm = (boem*)pc->parser;
    char* d_off = pc->con->rd_ptr;
    long len = (pc->con->wr_ptr - pc->con->rd_ptr);
    unsigned long bytes_used = 0;
    unsigned short mess_len = 0;
    unsigned char mtype = 0x0;
    do {
        memcpy(&mtype, d_off + 4, 1);
        memcpy(&mess_len, d_off + 2, 2);
        mess_len += 2;
        if(bytes_used + mess_len > len || mess_len == 2) {
            break;
        } 
        if(mtype == 0x08) {
            *cut_con = 1;
            send_debug_message(bm->o,"End of session signaled by boe.\n");
        } else if(mtype == 0x24) {
            mtype = d_off[10];
            if(mtype == 0x41) {
                start_boe_hb(bm);
                bm->ci->can_send_orders = 1;
                pc->connection_notice(bm, 1, NULL);
                send_debug_message(bm->o,"Logon Accepted mess_len: %d, len: %d .\n", mess_len, len);
            } else {
                switch(mtype) {
                    case 'N':
                        send_debug_message(bm->o,
                                "Logon Rejected: %c = Not Authorized, invalid username and/or password \n",
                                d_off[10]);
                        break;
                    case 'D':
                        send_debug_message(bm->o,"Logon Rejected: %c = Session not available. \n", d_off[10]);
                        break;
                    case 'B':
                        send_debug_message(bm->o,"Logon Rejected: %c = Session in use. \n", d_off[10]);
                        break;
                    case 'S':
                        send_debug_message(bm->o,"Logon Rejected: %c = Session invalid. \n", d_off[10]);
                        break;
                    case 'Q':
                        send_debug_message(bm->o,
                                "Logon Rejected: %c = Sequence ahead in Login message. \n",
                                d_off[10]);
                        break;
                    default:
                        send_debug_message(bm->o,"Logon Rejected: %s. \n", d_off + 10);
                        break;
                };
            }
        } else if(mtype == 0x09) {
            //printf("Got a Heartbeat!!!!\n");
        } else if(mtype == 0x13) {
            send_debug_message(bm->o,"Replay Complete mess_len: %d, len: %d .\n", mess_len, len);
        } else {
            bm->parse_seq_packet(bm, d_off, mess_len, mtype, len, bytes_used);
        }
        bytes_used += mess_len;
        d_off += mess_len ;
    } while(bytes_used < len);
    pc->con->rd_ptr += bytes_used;
}

