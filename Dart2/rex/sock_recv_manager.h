
#ifndef SOCK_RECV_MANAGER_H__
#define SOCK_RECV_MANAGER_H__

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct sock_recv_manager {
    char* buffer;
    char* wr_ptr;
    char* rd_ptr;
    char* end;
    int sock;
} sock_mnger;


sock_mnger* create_sock_recv_manager();

void destroy_sock_recv_manager(sock_mnger* srm);

void manage_socket(void* rc, int* cut_connection);


#if defined(__cplusplus)
}
#endif
#endif
