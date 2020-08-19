/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _TRANSACTION_RECORDER_H__
#define _TRANSACTION_RECORDER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <pthread.h>

#include "message_queue.h"
#include "dart_seq_request.h"
#include "dart_resend_request.h"
#include "common_structs.h"

#define FILE_BUFFER_SIZE 8192
#define FILE_READ_CHUNK 8191
#define MAX_SEQ_RESET_NUM 1000000

    /**
     * Definition of the function pointer that must be passed in to
     * the start recorder function in the rec_init structure.  Users
     * of the library will substitue an appropriate function that can
     * parse the types of messages they are sending.
     */
    typedef unsigned long (*rec_fn_parse) (queue_t q, char *data,
                                           unsigned long size,
                                           unsigned long bytes_used,
                                           unsigned long *seq_num);

    /**
     * Passed to the get_sequence function, these values will be
     * filled in with the actual values after the get_sequence
     * function returns.
     */
    typedef struct seq_offset {
        unsigned long seq;
        unsigned long beg_off;
        unsigned long end_off;
    } seq_offset;

    struct incoming_message *create_incoming_message(const char *name,
                                                     size_t name_len,
                                                     size_t num);

    /**
     * Use this creation function when, instead of incrementing the
     * sequence number by a certain amount, the inbound sequence
     * number needs to be set to a given number.
     */
    struct incoming_message *create_incoming_message_set_value(const char
                                                               *name,
                                                               size_t
                                                               namelength,
                                                               size_t
                                                               value);


    void set_incoming_message_name(struct incoming_message *im,
                                   const char *name, size_t name_len);

    void destroy_incoming_message(struct incoming_message *im);


    typedef struct rec_init {
        app_end_time *t;
        int create_new;
        size_t timeout_value;
        char *def_name;
        int def_name_len;
    } rec_init;

    typedef struct rolling_log_message {
        char *name;
        size_t name_len;
        char *data;
        unsigned long len;
        char *file_name;
        size_t file_name_len;
    } rolling_log_message;

    /**
     * Takes a messages struct with the name of the connection.  the
     * name of the connection is used to find the correct file to
     * write the message to. The file name will look like this
     * 'name'_incoming.log.
     */
    void record_inbound_message(struct incoming_message *m);

    /**
     * Takes a message struct with the name of the connection.  It
     * uses the name of this connection to write the message to the
     * correct file for the connection, the file name will look
     * something like this: 'name'_outgoing.log.
     */
    void record_outbound_message(struct message *m);

    /**
     * Initializes and starts the recording thread.  This thread
     * manages all the file io for all connections through the
     * server. Do not call this function from outside if you are using
     * the start_dart_services function.
     */
    int start_recorder(struct rec_init *rec);

    /**
     * The user passes in a sequence request structure whose values
     * are then filled in with the outgoing and incoming sequence
     * numbers for their connection.
     */
    void get_sequence_numbers(sequence_req * sr);

    /**
     * Users pass in the resend request structure with a NULL output
     * char.  This value will be filled in by the transaction
     * recorder.  The value of len will be the number of bytes added
     * to output.
     */
    void get_resend_request(resend_req * rr);

    /**
     * This function uses the name in the message to create or or
     * append to a file with the naming convention
     * "nameLogYYYYMMDD.LOG" or the subusequent log names:
     * "nameLogYYYYMMdd.HHMMSS.LOG".
     */
    int dart_rolling_log(struct rolling_log_message *m);

    int day_check(int day);

    void clean_seq(sequence_req * sr);

    ssize_t retrieve_inbound_seqno_from_file(const char *name,
                                             size_t namelength);

#if defined(__cplusplus)
}
#endif
#endif                          //_TRANSACTION_RECORDER_H__
