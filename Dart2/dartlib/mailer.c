/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include <netdb.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/tcp.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include "mailer.h"
#include "message_queue.h"

#define SMTP_RESPONSE_BUFF_SIZE 1024
#define SMTP_HEADER_SIZE 400
#define SMTP_MESS_PARTS_LEN 200

static pthread_mutex_t mailer_init_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_mutex_t mailer_queue_mutex = PTHREAD_MUTEX_INITIALIZER;
static sem_t mail_queue_sem;
static queue_t mail_queue;
static char *biz_alert_group;
static int biz_a_len;
static char *tech_alert_group;
static int tech_a_len;
static int mail_init = 0;

struct dart_mailer_args {
    char *mail_server;
    int m_len;
    char *biz_group;
    int b_len;
    char *tech_group;
    int t_len;
    int end_hour;
    int end_min;
    int end_day;
};

struct dart_mail_message {
    char *to;
    int to_len;
    char *message;
    int m_len;
    char *title;
    int t_len;
};

static int dart_smtp_connect(char *host, int port)
{
    int sock;
    int errcode;
    char portstr[3] = { '2', '5', '\0' };
    struct addrinfo hints, *res;
    int con;
    int err;
    struct timeval tv;
    sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    tv.tv_sec = 3;
    tv.tv_usec = 0;
    if (setsockopt
        (sock, SOL_SOCKET, SO_RCVTIMEO, (char *) &tv, sizeof(tv))) {
        printf("Failed to set recv timeout!\n");
    }
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    errcode = getaddrinfo(host, portstr, &hints, &res);
    if (errcode != 0) {

        printf("get addrinfo failed!! %s, %s \n", gai_strerror(errcode), host);
        return -1;
    }
    con = connect(sock, res->ai_addr, res->ai_addrlen);
    err = errno;
    if (con == 0 || err == EALREADY || err == EINPROGRESS || err == EAGAIN
        || err == EINTR) {
        freeaddrinfo(res);
        return sock;
    } else {
        printf
            ("Fatal socket error: %d, killing connection after attempt!\n",
             err);
        freeaddrinfo(res);
        return -1;
    }
    freeaddrinfo(res);
    return -1;
}

static int recv_response(int sock)
{
    char resp_buf[SMTP_RESPONSE_BUFF_SIZE];
    int num_bytes = 0;
    int ret_code = 0;
    int err = 0;
    memset(resp_buf, '\0', SMTP_RESPONSE_BUFF_SIZE);
    switch ((num_bytes =
             recv(sock, resp_buf, SMTP_RESPONSE_BUFF_SIZE - 1, 0))) {
    case -1:
        err = errno;
        if (err == EAGAIN || err == EINTR || err == 0) {
            printf("Timed out waiting for smtp response. \n");
        } else {
            printf("Negative one read shutting down.\n");
            ret_code = -1;
        }
        break;
    case 0:
        printf("Zero Read in Mailer shutting down.\n");
        ret_code = -1;
        break;
    default:
        ret_code = num_bytes;
        printf("Response From Mailer: {%s} \n", resp_buf);
        break;
    }
    return ret_code;
}

static int send_smtp(int sock, char *mess, int mess_len)
{
    int return_code = 0;
    return_code = send(sock, mess, mess_len, 0);
    if (return_code < 0) {
        return_code = -1;
    }
    return return_code;
}

static int format_date(char *d_off)
{
    int ret_len = 0;
    int s_len = 0;
    char *ls_time = calloc(50, 1);
    char *ls_off = ls_time;
    time_t rawtime;
    struct tm *ti;
    int year = 0;
    int month = 0;
    int day_of_week = 0;
    time(&rawtime);
    struct tm lm;
    ti = gmtime_r(&rawtime, &lm);
    ti = &lm;
    year = 1900 + ti->tm_year;
    month = 1 + ti->tm_mon;
    day_of_week = ti->tm_wday;
    switch (day_of_week) {
    case 0:
        {
            memcpy(ls_off, "Sun, ", 5);
            ls_off += 5;
            ret_len += 5;
            break;
        }
    case 1:
        {
            memcpy(ls_off, "Mon, ", 5);
            ls_off += 5;
            ret_len += 5;
            break;
        }
    case 2:
        {
            memcpy(ls_off, "Tues, ", 6);
            ls_off += 6;
            ret_len += 6;
            break;
        }
    case 3:
        {
            memcpy(ls_off, "Wed, ", 5);
            ls_off += 5;
            ret_len += 5;
            break;
        }
    case 4:
        {
            memcpy(ls_off, "Thurs, ", 7);
            ls_off += 7;
            ret_len += 7;
            break;
        }
    case 5:
        {
            memcpy(ls_off, "Fri, ", 5);
            ls_off += 5;
            ret_len += 5;
            break;
        }
    case 6:
        {
            memcpy(ls_off, "Sat, ", 5);
            ls_off += 5;
            ret_len += 5;
            break;
        }
    }

    s_len = sprintf(ls_off, "%02u", ti->tm_mday);
    ret_len += s_len;
    ls_off += s_len;

    switch (month) {
    case 1:
        memcpy(ls_off, "Jan ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 2:
        memcpy(ls_off, "Feb ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 3:
        memcpy(ls_off, "Mar ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 4:
        memcpy(ls_off, "Apr ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 5:
        memcpy(ls_off, "May ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 6:
        memcpy(ls_off, "Jun ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 7:
        memcpy(ls_off, "Jul ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 8:
        memcpy(ls_off, "Aug ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 9:
        memcpy(ls_off, "Sep ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 10:
        memcpy(ls_off, "Oct ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 11:
        memcpy(ls_off, "Nov ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    case 12:
        memcpy(ls_off, "Dec ", 4);
        ls_off += 4;
        ret_len += 4;
        break;
    }

    s_len = sprintf(ls_off, "%04d %02d:%02d:%02d GMT\r\n", year,
                    ti->tm_hour, ti->tm_min, ti->tm_sec);
    ls_off += s_len;
    ret_len += s_len;

    memcpy(d_off, ls_time, ret_len);
    free(ls_time);
    return ret_len;
}

static int prepare_header(char *header, char *from, char *to,
                          char *subject)
{
    char *h_off = header;
    int s_len = 0;
    int ret_len = 0;
    s_len = sprintf(h_off, "From: %s\r\nTo: <%s>\r\nDate: ", from, to);
    h_off += s_len;
    ret_len += s_len;
    s_len = format_date(h_off);
    h_off += s_len;
    ret_len += s_len;
    s_len =
        sprintf(h_off, "Subject: %s\r\nX-Mailer: Send RV Mail\r\n\r\n",
                subject);
    ret_len += s_len;
    return ret_len;
}

static int cook_body(char *orig_message, int len, char *send_message)
{
    int ret_len = 0;
    char *bad_offset = send_message;
    char *kill_me = NULL;
    char *end = "\r\n";
    int endl = strlen(end);
    char sz_bad[10] =
        { '\\', 'r', '\\', 'n', '.', '\\', 'r', '\\', 'n', '\0' };
    kill_me = strstr(orig_message, sz_bad);
    if (kill_me != NULL) {
        return -1;
    }
    memcpy(bad_offset, orig_message, len);
    bad_offset += len;
    ret_len += len;
    memcpy(bad_offset, end, endl);
    ret_len += endl;
    return ret_len;
}

static int transit_message(struct dart_mail_message *m,
                           struct dart_mailer_args *dma)
{
    char from_str[SMTP_MESS_PARTS_LEN];
    char to_str[SMTP_MESS_PARTS_LEN];
    char data[SMTP_MESS_PARTS_LEN];
    char *ls_hello = calloc(100, 1);
    char *ls_quit = "QUIT\r\n";
    char *ls_end = "\r\n.\r\n";
    int s_len = 0;
    int sock = 0;
    int ret_code = 0;
    //char* local_host = "pflynn@sumocap.com";
    int local_host_len = 0;
    char *lh_off = 0;
    char local_host[100];
    char *header = NULL;
    char *body = NULL;

    sock = dart_smtp_connect(dma->mail_server, 25);
    if (sock <= 0) {
        free(ls_hello);
	printf("SMTP Failed to connect\n");
        return -1;
    }
    recv_response(sock);
    memset(local_host, '\0', 100);
    gethostname(local_host, 100);
    local_host_len = strlen(local_host);
    lh_off = local_host;
    lh_off += local_host_len;
    s_len = sprintf(ls_hello, "HELO %s.sumocap.com\r\n", local_host);
    //s_len = sprintf(ls_hello, "HELO %s\r\n", local_host);
    printf("About to send: %s \n", ls_hello);
    ret_code = send_smtp(sock, ls_hello, s_len);
    if (ret_code < 0) {
        close(sock);
        free(ls_hello);
	printf("SMTP Failed to send hello\n");
        return -1;
    }
    recv_response(sock);
    s_len = sprintf(from_str, "MAIL From: <%s>\r\n", "pflynn@sumocap.com");
    ret_code = send_smtp(sock, from_str, s_len);
    if (ret_code < 0) {
        close(sock);
        free(ls_hello);
	printf("SMTP Failed to send From\n");
        return -1;
    }
    recv_response(sock);
    s_len = sprintf(to_str, "RCPT TO: <%s>\r\n", m->to);
    ret_code = send_smtp(sock, to_str, s_len);
    if (ret_code < 0) {
        close(sock);
        free(ls_hello);
	printf("SMTP Failed to send TO\n");
        return -1;
    }
    recv_response(sock);
    s_len = sprintf(data, "DATA\r\n");
    ret_code = send_smtp(sock, data, s_len);
    if (ret_code < 0) {
        close(sock);
        free(ls_hello);
	printf("SMTP Failed to send DATA\n");
        return -1;
    }
    recv_response(sock);
    header = calloc(SMTP_HEADER_SIZE, 1);
    s_len = prepare_header(header, local_host, m->to, m->title);
    ret_code = send_smtp(sock, header, s_len);
    if (ret_code < 0) {
        close(sock);
        free(ls_hello);
        free(header);
	printf("SMTP Failed to send HEADER\n");
        return -1;
    }
    recv_response(sock);
    body = calloc((m->m_len + 10), 1);
    s_len = cook_body(m->message, m->m_len, body);
    if (s_len < 0) {
        close(sock);
        free(ls_hello);
        free(body);
        free(header);
        printf("Body had an illegal sequence: %s\n", m->message);
        return -1;
    }
    ret_code = send_smtp(sock, body, s_len);
    if (ret_code < 0) {
        close(sock);
        free(ls_hello);
        free(body);
        free(header);
	printf("SMTP Failed to send BODY\n");
        return -1;
    }
    send_smtp(sock, ls_end, strlen(ls_end));
    recv_response(sock);

    ret_code = send_smtp(sock, ls_quit, 6);
    recv_response(sock);
    close(sock);
    free(body);
    free(header);
    free(ls_hello);
    return ret_code;
}

static void send_mail_message(struct dart_mailer_args *dma)
{
    element_t e, temp;
    pthread_mutex_lock(&mailer_queue_mutex);
    e = dequeue(mail_queue);
    pthread_mutex_unlock(&mailer_queue_mutex);
    while (e != NULL) {
        struct dart_mail_message *m;
        m = (struct dart_mail_message *) e->d;
        transit_message(m, dma);
        temp = e;
        e = temp->next;
        free(m->to);
        free(m->message);
        free(m->title);
        free(m);
        m = NULL;
        temp->d = NULL;
        pthread_mutex_lock(&mailer_queue_mutex);
        free_element(mail_queue, temp);
        pthread_mutex_unlock(&mailer_queue_mutex);
        temp = NULL;
    }
}

static void *run_mail_service(void *args)
{
    struct dart_mailer_args *dma = (struct dart_mailer_args *) args;
    time_t long_time;
    struct timespec timeout;
    struct tm *newtime;
    int keep_running = 1;
    int status = 0;
    int l_is_empty = 1;
    while (keep_running) {
        timeout.tv_sec = time(NULL) + 30;
        timeout.tv_nsec = 0;
        status = sem_timedwait(&mail_queue_sem, &timeout);
        if (status == 0) {
        } else if (status == ETIMEDOUT || status == EINVAL) {
            printf
                ("Could not get the semaphore before timeout or bad sem: %d.\n",
                 status);
        } else if (status == EDEADLK) {
            printf("Deadlock Detected in nyse_seq_generator.\n");
        }
        pthread_mutex_lock(&mailer_queue_mutex);
        l_is_empty = is_empty(mail_queue);
        pthread_mutex_unlock(&mailer_queue_mutex);
        if (!l_is_empty) {
            send_mail_message(dma);
        }
        time(&long_time);
        struct tm lm;
        newtime = localtime_r(&long_time, &lm);
        if (lm.tm_hour == dma->end_hour &&
            lm.tm_min >= dma->end_min
            && ((newtime->tm_wday == dma->end_day)
                || (dma->end_day == 7))) {
            keep_running = 0;
        }
    }
    return NULL;
}

int start_dart_mail_service(const char *mail_server, int m_len,
                            const char *biz_group, int b_len,
                            const char *tech_group, int t_len,
                            int end_hour, int end_min, int end_day)
{
    pthread_mutex_lock(&mailer_init_mutex);
    if (mail_init) {
        // do nothing
    } else {
        //mail_init = 1;
        int status;
        pthread_attr_t thread_attr;
        pthread_t fix_admin_thread;
        struct dart_mailer_args *dma;

        mail_init = 1;
        dma = malloc(sizeof(struct dart_mailer_args));
        dma->mail_server = calloc(m_len + 1, 1);
        memcpy(dma->mail_server, mail_server, m_len);
        dma->m_len = m_len;
        dma->biz_group = calloc(b_len + 1, 1);
        biz_alert_group = calloc(b_len + 1, 1);
        memcpy(dma->biz_group, biz_group, b_len);
        memcpy(biz_alert_group, biz_group, b_len);
        dma->b_len = b_len;
        biz_a_len = b_len;
        dma->tech_group = calloc(t_len + 1, 1);
        tech_alert_group = calloc(t_len + 1, 1);
        memcpy(tech_alert_group, tech_group, t_len);
        memcpy(dma->tech_group, tech_group, t_len);
        dma->t_len = t_len;
        tech_a_len = t_len;
        dma->end_hour = end_hour;
        dma->end_min = end_min;
        dma->end_day = end_day;

        mail_queue = create_queue();
        sem_init(&mail_queue_sem, 0, 0);

        status = pthread_attr_init(&thread_attr);
        if (status != 0) {
            printf("Pthread attr init failed!\n");
        }
        status = pthread_attr_setdetachstate(&thread_attr,
                                             PTHREAD_CREATE_DETACHED);
        if (status != 0) {
            printf("Thread set detached failed!\n");
        }

        pthread_create(&fix_admin_thread, &thread_attr, run_mail_service,
                       dma);
    }
    pthread_mutex_unlock(&mailer_init_mutex);
    return 0;
}

static int queue_message(const char *message, int len, const char *title,
                         int t_len, char *to, int to_len)
{
    if (mail_queue) {
        struct dart_mail_message *dmm =
            malloc(sizeof(struct dart_mail_message));
        dmm->message = calloc(len + 1, 1);
        memcpy(dmm->message, message, len);
        dmm->m_len = len;
        dmm->title = calloc(t_len + 1, 1);
        memcpy(dmm->title, title, t_len);
        dmm->t_len = t_len;
        dmm->to = calloc(to_len + 1, 1);
        dmm->to_len = to_len;
        memcpy(dmm->to, to, to_len);
        pthread_mutex_lock(&mailer_queue_mutex);
        enqueue(mail_queue, dmm);
        pthread_mutex_unlock(&mailer_queue_mutex);
        sem_post(&mail_queue_sem);
        return 1;
    } else {
        return 0;
    }
}

int send_biz_message(const char *message, int len, const char *title,
                     int t_len)
{
    return queue_message(message, len, title, t_len, biz_alert_group,
                         biz_a_len);
}

int send_tech_message(const char *message, int len, const char *title,
                      int t_len)
{
    return queue_message(message, len, title, t_len, tech_alert_group,
                         tech_a_len);
}
