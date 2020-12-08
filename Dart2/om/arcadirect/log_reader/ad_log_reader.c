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
    char* new_ord_out = calloc(900, 1);
    char* off = cur_off;
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    len = ntohs(len);
    if(len > bytes_left) {
        return 0;
    }
    off += 6;
    unsigned int clordid = 0;
    memcpy(&clordid, off, 4);
    clordid = ntohl(clordid);
    off += 8;
    int qty = 0;
    memcpy(&qty, off, 4);
    qty = ntohl(qty);
    off += 4;
    int price = 0;
    memcpy(&price, off, 4);
    price = ntohl(price);
    off += 4;
    short ex_dest = 0;
    memcpy(&ex_dest, off, 2);
    ex_dest = ntohs(ex_dest);
    off += 3;
    char* sym = off;
    off += 8;
    off += 15;
    char ex_inst = off[0];
    off += 1;
    char side = off[0];
    off += 1;
    char type = off[0];
    off +=1;
    char tif = off [0];
    int s_len = sprintf(new_ord_out, "New Order: len %d, ordid: %u, qty %d, price %d, ex_dest %d, sym %s,ex %c, side %c, type %c, tif %c \n", 
                                      len, clordid, qty, price, ex_dest, sym, ex_inst, side, type, tif); 
    write_to_out(new_ord_out, s_len, out_file);
    return len;
}

static ssize_t parse_cancel(const char* buff, ssize_t len_left, int out_file)
{
    char* off = (char*)buff;
    char* cancel_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    len = ntohs(len);
    if(len > len_left) {
        return 0;
    }
    off += 14;
    unsigned int clordid = 0;
    memcpy(&clordid, off, 4);
    clordid = ntohl(clordid);
    int s_len = sprintf(cancel_out, "Cancel: len %d, %u \n", 
                                      len, clordid); 
    write_to_out(cancel_out, s_len, out_file);
    return len;
}
static ssize_t parse_open(const char* buff, ssize_t len_left, int out_file)
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
    off += 22;
    unsigned int clordid = 0;
    memcpy(&clordid, off, 4);
    clordid = ntohl(clordid);
    off += 4;
    unsigned long ex_tag = 0;
    memcpy(&ex_tag, off, 8);
    ex_tag = ntohll(ex_tag); 
    int s_len = sprintf(open_out, "Open: len %d,clordid: %u exchange id: %lu \n", 
                                      len, clordid, ex_tag); 
    write_to_out(open_out, s_len, out_file);
    return len;
}
static ssize_t parse_pending(const char* buff, ssize_t len_left, int out_file)
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
    off += 22;
    unsigned int clordid = 0;
    memcpy(&clordid, off, 4);
    clordid = ntohl(clordid);
    off += 4;
    unsigned long ex_tag = 0;
    memcpy(&ex_tag, off, 8);
    ex_tag = ntohll(ex_tag); 
    int s_len = sprintf(open_out, "Pending: len %d, clordid: %u, exchange id: %lu \n", 
                                      len, clordid, ex_tag); 
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
    off += 22;
    unsigned int clordid = 0;
    memcpy(&clordid, off, 4);
    clordid = ntohl(clordid);
    off += 4;
    unsigned long ex_tag = 0;
    memcpy(&ex_tag, off, 8);
    ex_tag = ntohll(ex_tag); 
    off += 8;
    unsigned long exec_tag = 0;
    memcpy(&exec_tag, off, 8);
    exec_tag = ntohll(exec_tag);
    off += 8;
    char* arca_ex_id_off = off;
    off += 20;
    unsigned int shares = 0;
    memcpy(&shares, off, 4);
    shares = ntohl(shares);
    off += 4;
    unsigned int price = 0;
    memcpy(&price, off, 4);
    price = ntohl(price);
    off += 5;
    char liq = off[0];
    char side = off[1];
    char* last = off + 2;  
    int s_len = sprintf(open_out, "Fill: len %d,clordid: %u, exchange id: %lu, execution tag: %lu, ArcaExId: %s, shares: %u, price: %u, liq: %c, side: %c, last: %s \n", 
                                      len, clordid, ex_tag, exec_tag, arca_ex_id_off, shares, price, liq, side, last); 
    write_to_out(open_out, s_len, out_file);
    return len;
}
static ssize_t parse_replace(const char* buff, ssize_t len_left, int out_file)
{
    char* off = (char*)buff;
    char* replace_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    len = ntohs(len);
    if(len > len_left) {
        return 0;
    }
    off += 14;
    unsigned int clordid = 0;
    memcpy(&clordid, off, 4);
    clordid = ntohl(clordid);
    off += 4;
    unsigned int orig_id = 0;
    memcpy(&orig_id, off, 4);
    orig_id = ntohl(orig_id);
    off += 4;
    unsigned int qty = 0;
    memcpy(&qty, off, 4);
    qty = ntohl(qty);
    off += 8;
    unsigned int price = 0;
    memcpy(&price, off, 4);
    price = ntohl(price);
    off += 12;
    char symbol[10];
    memset(symbol, '\0', 10);
    memcpy(symbol, off, 8);
    off += 16;
    char ex_inst = off[0];
    char side = off[1];
    char type = off[2];
    char tiff = off[3];
    char rule80A = off[4];
    int s_len = sprintf(replace_out, "Replace: len %d, clordid: %u, orig id: %u, qty: %u, price: %u, symbol: %s, execution instructions: %c, side: %c, type: %c, tiff: %c, rule80A: %c \n", 
                                      len, clordid, orig_id, qty, price, symbol, ex_inst, side, type, tiff, rule80A); 
    write_to_out(replace_out, s_len, out_file);
    return len;
}

static ssize_t parse_pending_replaced(const char* buff, ssize_t len_left, int out_file)
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
    off += 22;
    unsigned int clordid = 0;
    memcpy(&clordid, off, 4);
    clordid = ntohl(clordid);
    off += 4;
    unsigned long ex_tag = 0;
    memcpy(&ex_tag, off, 8);
    ex_tag = ntohll(ex_tag); 
    int s_len = sprintf(open_out, "Pending Replace: len %d,clordid: %u, exchange id: %lu \n", 
                                      len, clordid, ex_tag); 
    write_to_out(open_out, s_len, out_file);
    return len;
}
static ssize_t parse_replaced(const char* buff, ssize_t len_left, int out_file)
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
    off += 22;
    unsigned int clordid = 0;
    memcpy(&clordid, off, 4);
    clordid = ntohl(clordid);
    off += 4;
    unsigned long ex_tag = 0;
    memcpy(&ex_tag, off, 8);
    ex_tag = ntohll(ex_tag); 
    int s_len = sprintf(open_out, "Replaced: len %d,clordid: %u, exchange id: %lu \n", 
                                      len, clordid, ex_tag); 
    write_to_out(open_out, s_len, out_file);
    return len;
}
static ssize_t parse_out(const char* buff, ssize_t len_left, int out_file)
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
    off += 22;
    unsigned int clordid = 0;
    memcpy(&clordid, off, 4);
    clordid = ntohl(clordid);
    off += 4;
    unsigned long ex_tag = 0;
    memcpy(&ex_tag, off, 8);
    ex_tag = ntohll(ex_tag); 
    int s_len = sprintf(open_out, "Out: len %d,clordid: %u, exchange id: %lu \n", 
                                      len, clordid, ex_tag); 
    write_to_out(open_out, s_len, out_file);
    return len;
}

static ssize_t parse_unknown(const char* buff, ssize_t len_left, int out_file)
{
    char* off = (char*)buff;
    char* unknown_out = calloc(900, 1);
    off += 2;
    short len = 0;
    memcpy(&len, off, 2);
    len = ntohs(len);
    if(len > len_left) {
        return 0;
    }
    int s_len = sprintf(unknown_out, "Unknown: len %d, %c \n", 
                                      len, buff[0]); 
    write_to_out(unknown_out, s_len, out_file);
    return len;
}

static ssize_t parse_n_write(const char* buff, ssize_t bytes_read, int out_file) 
{
    ssize_t parsed_bytes = 0;
    char* cur_off = (char*)buff;
    while(bytes_read > 25 && parsed_bytes < bytes_read) {
        write_to_out(cur_off, 23, out_file);
        cur_off += 23;
        parsed_bytes += 23;
        char type = cur_off[0];
        ssize_t mess_bytes = 0;
        switch(type) {
            case 'D':
                mess_bytes = parse_new_ord(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 'F':
                mess_bytes = parse_cancel(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 'G':
                mess_bytes = parse_replace(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 'a':
                mess_bytes = parse_open(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case '6':
                mess_bytes = parse_pending(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case '4':
                mess_bytes = parse_out(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case '5':
                mess_bytes = parse_replaced(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case '2':
                mess_bytes = parse_fill_mess(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            case 'E':
                mess_bytes = parse_pending_replaced(cur_off, bytes_read - parsed_bytes, out_file);
                break;
            default:
                mess_bytes = parse_unknown(cur_off, bytes_read - parsed_bytes, out_file);
                break;
        }        
        if(mess_bytes == 0) {
            parsed_bytes -= 23;
            return parsed_bytes;
        }
        parsed_bytes += mess_bytes;
        cur_off += mess_bytes;
        if(cur_off[0] == '\n') {
            parsed_bytes += 1;
            cur_off += 1;
        } else {
            printf("Bad read: %c\n", type);
            while(cur_off[0] != '\n') {
                ++cur_off;
                ++parsed_bytes;
            }
            if(cur_off[1] == '2') {
                ++cur_off;
                ++parsed_bytes;
            } else {
                cur_off += 2;
                parsed_bytes += 2;
            }
        }
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
             bytes_read += (rd_ptr - buff);
             bytes_parsed = parse_n_write(buff, bytes_read, out_file);
            if(bytes_parsed < bytes_read) {
                char* cur_off = buff;
                cur_off += bytes_parsed;
                ssize_t len = bytes_read - bytes_parsed;
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
