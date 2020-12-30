#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <arpa/inet.h>

#define ntohll(x) ( ( (uint64_t)(ntohl( (uint32_t)((x << 32) >> 32) )) << 32) | ntohl( ((uint32_t)(x >> 32)) ) )
#define htonll(x) ntohll(x)
#define FILE_READ_CHUNK 65536



static ssize_t write_to_out(char *ptr,ssize_t len,
                           int fd)
{
    char *w_off =  ptr;
    ssize_t len_left = len;
    ssize_t written = 0;
    long b_w = 0;

    while (written < len && b_w >= 0) {
        b_w = write(fd, w_off, len_left);
        if(b_w >= 0 ) {
            written += b_w;
            w_off += b_w;
            len_left -= b_w;
        } else {
            printf("Error on stream in transwriter : %d. %d \n", errno,
                   fd);
        }
    }
    if(written > len) {
        printf("Error on stream in transwriter : %ld. %ld \n", written,
               len);
    }
    return written;
}
static ssize_t parse_new_ord(char* cur_off, ssize_t bytes_left, int out_file)
{
    if(bytes_left < 84) {
        return 0;
    }
    char* new_ord_out = calloc(900, 1);
    char* off = cur_off;
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    len = ntohs(len);
    if(len  != 84) {
        len = 84;
    }
    off += 62;
    char clordid[20];
    memset(clordid, '\0', 20);
    memcpy(clordid, off, 17);
    off = cur_off + 8;
    int qty = 0;
    memcpy(&qty, off, 4);
    qty = ntohl(qty);
    off += 8;
    int price = 0;
    memcpy(&price, off, 4);
    price = ntohl(price);
    off += 5;
    char sym[12];
    memset(sym, '\0', 12);
    memcpy(sym, off, 11);
    off += 11;
    char ex_inst = off[0];
    off += 1;
    char side = off[0];
    off += 1;
    char type = off[0];
    off +=1;
    char tif = off [0];
    off += 1;
    char rule80A = off[0];
    char route_inst = off[1];
    int s_len = sprintf(new_ord_out, "New Order: len %d, ordid: %s, qty %d, price %d, sym %s,ex %c, side %c, type %c, tif %c, rule80A %c, Route %c \n", 
                                      len, clordid, qty, price, sym, ex_inst, side, type, tif, rule80A, route_inst); 
    write_to_out(new_ord_out, s_len, out_file);
    return len;
}

static ssize_t parse_cancel(const char* buff, ssize_t len_left, int out_file)
{
    if(len_left < 92) {
        return 0;
    }
    char* off = (char*)buff;
    char* cancel_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    len = ntohs(len);
    if(len != 92) {
        len = 92;
    }
    off += 54;
    char clordid[20];
    memset(clordid, '\0', 20);
    memcpy(clordid, off, 17);
    off += 17;
    char orig[20];
    memset(orig, '\0', 20);
    memcpy(orig, off, 17);
    int s_len = sprintf(cancel_out, "Cancel: len %d,Clordid %s, orig id: %s \n", 
                                      len, clordid, orig); 
    write_to_out(cancel_out, s_len, out_file);
    return len;
}
static ssize_t parse_open(const char* buff, ssize_t len_left, int out_file)
{
    if(len_left < 56) {
        return 0;
    }
    char* off = (char*)buff;
    char* open_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    if(len  != 56) {
        len = 56;
    }
    off += 34;
    char clordid[20];
    memset(clordid, '\0', 20);
    memcpy(clordid, off, 17);
    off = (char*)buff;
    int s_len = sprintf(open_out, "Open: len %d,clordid: %s \n", 
                                      len, clordid); 
    write_to_out(open_out, s_len, out_file);
    return len;
}
static ssize_t parse_pending(const char* buff, ssize_t len_left, int out_file)
{
    if(len_left < 56) {
        return 0;
    }
    char* off = (char*)buff;
    char* open_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    if(len != 56) {
        len = 56;
    }
    off += 34;
    char clordid[20];
    memset(clordid, '\0', 20);
    memcpy(clordid, off, 17);
    int s_len = sprintf(open_out, "Pending: len %d, clordid: %s, \n", 
                                      len, clordid); 
    write_to_out(open_out, s_len, out_file);
    return len;
}
static ssize_t parse_fill_mess(const char* buff, ssize_t len_left, int out_file)
{
    char* off = (char*)buff;
    char* open_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    len = ntohs(len);
    if(len > len_left) {
        return 0;
    }
    off += 2;
    int seq = 0;
    memcpy(&seq, off, 4);
    seq = ntohl(seq);
    off += 4;
    memcpy(&seq, off, 4);
    seq = ntohl(seq);
    off += 4;
    memcpy(&seq, off, 4);
    seq = ntohl(seq);
    off += 4;
    unsigned int leaves = 0;
    memcpy(&leaves, off, 4);
    leaves = ntohl(leaves);
    off += 4;
    unsigned int shares = 0;
    memcpy(&shares, off, 4);
    shares = ntohl(shares);
    off += 4;
    unsigned int price = 0;
    memcpy(&price, off, 4);
    price = ntohl(price);
    off += 5;
    char liq = off[1];
    char side = off[0];
    char last = off[2];
    off += 34;
    char bill_rate[8];
    memset(bill_rate, '\0', 8);
    memcpy(bill_rate, off, 6);
    off += 6;
    char exec_tag[12];
    memset(exec_tag, '\0', 12);
    memcpy(exec_tag, off, 10);
    off = (char*)buff;  
    off += 100;//99;
    char clordid[20];
    memset(clordid, '\0', 20);
    memcpy(clordid, off, 17);
    int s_len = sprintf(open_out, "Fill: len %d,clordid: %s, execution tag: %s, shares: %u, leaves: %u, price: %u, liq: %c, side: %c, last: %c, Billing Rate: %s \n", 
                                      len, clordid, exec_tag, shares, leaves, price, liq, side, last, bill_rate); 
    write_to_out(open_out, s_len, out_file);
    return len;
}
static ssize_t parse_replace(const char* buff, ssize_t len_left, int out_file)
{
    if(len_left < 104) {
        return 0;
    }
    char* off = (char*)buff;
    char* replace_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    len = ntohs(len);
    if(len  != 104) {
        len = 104;
    }
    off += 10;
    unsigned int qty = 0;
    memcpy(&qty, off, 4);
    qty = ntohl(qty);
    off += 8;
    unsigned int price = 0;
    memcpy(&price, off, 4);
    price = ntohl(price);
    off += 5;
    char symbol[12];
    memset(symbol, '\0', 12);
    memcpy(symbol, off, 11);
    off += 11;
    char ex_inst = off[0];
    off += 1;
    char route_inst = off[0];
    char side = off[1];
    char type = off[2];
    char tiff = off[3];
    char rule80A = off[4];
    off += 31;
    char clordid[20];
    memset(clordid, '\0', 20);
    memcpy(clordid, off, 17);
    off += 17;
    char orig[20];
    memset(orig, '\0', 20);
    memcpy(orig, off, 17);
    int s_len = sprintf(replace_out, "Replace: len %d, clordid: %s, orig id: %s, qty: %u, price: %u, symbol: %s, execution instructions: %c, side: %c, type: %c, tiff: %c, rule80A: %c, Route: %c \n", 
                                      len, clordid, orig, qty, price, symbol, ex_inst, side, type, tiff, rule80A, route_inst); 
    write_to_out(replace_out, s_len, out_file);
    return len;
}

static ssize_t parse_pending_replaced(const char* buff, ssize_t len_left, int out_file)
{
    if(len_left < 56) {
        return 0;
    }
    char* off = (char*)buff;
    char* open_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    if(len  != 56) {
        len = 56;
    }
    off += 34;
    char clordid[20];
    memset(clordid, '\0', 20);
    memcpy(clordid, off, 17);
    int s_len = sprintf(open_out, "Pending Replace: len %d,clordid: %s \n", 
                                      len, clordid); 
    write_to_out(open_out, s_len, out_file);
    return len;
}
static ssize_t parse_replaced(const char* buff, ssize_t len_left, int out_file)
{
    if(len_left < 60) {
        return 0;
    }
    char* off = (char*)buff;
    char* open_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    if(len != 60) {
        len = 60;
    }
    off += 39;
    char clordid[20];
    memset(clordid, '\0', 20);
    memcpy(clordid, off, 17);
    off = (char*)buff;
    off += 16;
    unsigned int leaves = 0;
    memcpy(&leaves, off, 4);
    leaves = ntohl(leaves);
    int s_len = sprintf(open_out, "Replaced: len %d,clordid: %s , leaves: %u \n", 
                                      len, clordid, leaves); 
    write_to_out(open_out, s_len, out_file);
    return len;
}
static ssize_t parse_out(const char* buff, ssize_t len_left, int out_file)
{
    if(len_left < 56) {
        return 0;
    }
    char* off = (char*)buff;
    char* open_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    len = ntohs(len);
    if(len != 56) {
        len = 56;
    }
    off += 35;
    char clordid[20];
    memset(clordid, '\0', 20);
    memcpy(clordid, off, 17);
    int s_len = sprintf(open_out, "Out: len %d,clordid: %s  \n", 
                                      len, clordid); 
    write_to_out(open_out, s_len, out_file);
    return len;
}

static ssize_t parse_rej(const char* buff, ssize_t len_left, int out_file)
{
    if(len_left < 116) {
        return 0;
    }
    char* off = (char*)buff;
    char* open_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    if(len != 116) {
        len = 116;
    }
    off += 35;
    char clordid[20];
    memset(clordid, '\0', 20);
    memcpy(clordid, off, 17);
    int s_len = sprintf(open_out, "Rej: len %d,clordid: %s {%c} \n", 
                                      len, clordid, buff[19]); 
    write_to_out(open_out, s_len, out_file);
    return len;
}

static ssize_t parse_unknown(const char* buff, ssize_t len_left, int out_file, short type)
{
    char* off = (char*)buff;
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    len = ntohs(len);
    if(len > len_left) {
        return 0;
    } else {
        char* open_out = calloc(900, 1);
        int s_len = sprintf(open_out, "Unknown: type: %#010x, len %d \n", 
                                         type, len); 
        write_to_out(open_out, s_len, out_file);
        return len;
    }
}

static ssize_t parse_n_write(const char* buff, ssize_t bytes_read, int out_file) 
{
    ssize_t parsed_bytes = 0;
    char* cur_off = (char*)buff;
    while(bytes_read > 25 && parsed_bytes < bytes_read) {
        write_to_out(cur_off, 23, out_file);
        cur_off += 23;
        parsed_bytes += 23;
        short type = 0;
        memcpy(&type, cur_off, 2);//cur_off[0];
        type = ntohs(type);
        ssize_t mess_bytes = 0;
        switch(type) {
            case 0x41:
                mess_bytes = parse_new_ord(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 0x61:
                mess_bytes = parse_cancel(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 0x71:
                mess_bytes = parse_replace(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 0x91:
                mess_bytes = parse_open(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 0xA1:
                mess_bytes = parse_pending(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 0xD1:
                mess_bytes = parse_out(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 0xE1:
                mess_bytes = parse_replaced(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 0x81:
                mess_bytes = parse_fill_mess(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 0xB1:
                mess_bytes = parse_pending_replaced(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 0xF1:
                mess_bytes = parse_rej(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            default:
                mess_bytes = parse_unknown(cur_off, bytes_read - parsed_bytes, out_file, type);
                break;
        }        
        if(mess_bytes == 0) {
            return parsed_bytes;
        }
        if(mess_bytes < 0 || mess_bytes > 300) {
            mess_bytes = 0;
        } else {
            cur_off += mess_bytes;
        }
        while(cur_off[0] != '\n') {
           ++cur_off;
          ++mess_bytes;
        }
        if(cur_off[0] == '\n') {
            ++cur_off;
            ++mess_bytes; 
        }
        parsed_bytes += mess_bytes;
    }
    return parsed_bytes;
}

static int translate_file(const char* in_name, const char* out_name)
{
    int in_file = open(in_name, O_RDONLY);
    int out_file = open(out_name, O_TRUNC | O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    char* buff = calloc(FILE_READ_CHUNK, 1);
    ssize_t size_left = FILE_READ_CHUNK;
    ssize_t bytes_read = 0;
    ssize_t bytes_parsed = 0;
    char* rd_ptr = buff;
    if(in_file > -1 && out_file > -1) {
        while ((bytes_read = read(in_file, rd_ptr, size_left)) > 0) {
             bytes_parsed = parse_n_write(buff, bytes_read, out_file);
            if(bytes_parsed < bytes_read) {
                char* cur_off = buff;
                cur_off += bytes_parsed;
                ssize_t len = bytes_read - bytes_read;
                memcpy(buff, cur_off, len);
                rd_ptr = buff;
                rd_ptr += len;
                size_left = FILE_READ_CHUNK - len;
            }
        }
    }
    return 0;
}




int main(int argc, char *argv[])
{
    int bad_args = -1;
    if (argc < 2) {
        fprintf(stderr,
                "usage: parse_raw in_file output_file \n");
        return bad_args;
    }
    
    return translate_file(argv[1], argv[2]);;
}
