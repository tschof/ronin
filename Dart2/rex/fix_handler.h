/**************************************************************************
 * Copyright (c) 2008 by Ronin Capital, LLC
 *
 * All Rights Reserved
 *************************************************************************/
#ifndef _FIX_HANDLER_H__
#define _FIX_HANDLER_H__

#if defined(__cplusplus)
extern "C" {
#endif

#include <pthread.h>
#include <time.h>

#include "polling_server.h"
#include "message_queue.h"




/** @file fix_handler.h Definition of the Fix handling functions.*/
/**
 * This function is used to count the number of FIX messages a connection
 * has received during the course of a session.  It only returns that seq
 * by filling in the seq_num param. It does not capture 
 * account for reset sequence pads.
 * @param data an array of char data that should be FIX messages.
 * @param size the length of the data array.
 * @param byte_offset the offset into the array that the function should start
 * parsing.
 * @param seq_num the current sequence number parsed.
 */
unsigned long build_fix_offsets(char *data,
                                unsigned long size,
                                unsigned long byte_offset,
                                unsigned long *seq_num, void* b);
/**
 * A function for parsing fix messages.
 * @param cut_con, a pointer to an integer. If set to one the
 * connection will be dropped.
 * @param pc an async_parse_args object that defines a fix session.
 * @see \rex_con
 */
void parse_fix_message(int *cut_con, rex_con *pc);
/**
 * Sends a FIX logon message.
 * @param pa an async_parse_args structure that helps to
 * define a FIX session.
 */
void send_fix_logon(void *pa);
/**
 * This function is used to produce 'default' FIX messages to 
 * account for reset sequence pads.
 * @param seq the seq to which you will pad.
 * @param fix_version a char array that represents the FIX version.
 * @param len the length of the fix_version array.
 */
char *create_fix_resend_pad(unsigned long seq,
                            char *fix_version, size_t * len);
/**
 * This function is used internally in a timer, users can ignore.
 * @param this is a void but should be a pointer to a rex_con object.
 */
void* send_heart_beat(void * fco);
/**
 * Sends a test request message on the FIX connection defined by fco.
 * @param fco a rex_con that defines a FIX connection.
 */
void send_test_req(rex_con * fco);
/**
 * This function parses a FIX messages sent into it in the data param
 * and then based on the beg_seq and end_seq params updates the beg_off
 * and end_off pointers so that they point to the begining and end of 
 * the segment in question.
 * @param data the raw char array to be parsed.
 * @param seq the current sequence this will be updated as complete
 * FIX messages are parsed out of the data stream.
 * @param size the length of data
 * @param byte_offset an offset into the data array, can be 0.
 * @param beg_seq if it is greater than 0 then it is the seq number the parser
 * will be looking for to begin.
 * @param end_seq if zero the file will be parsed to the end otherwise parsing will
 * stop once the message with this sequence is reached.
 * @param beg_off a pointer to the start of the messages that fit the criteria of being
 * after the beg_seq and before the end_seq.
 * @param end_off a pointer to the end of the message that fit the criteria of being after
 * the beg_seq and before the end_seq.
 */
size_t find_fix_offsets(char *data, size_t * seq, size_t size,
                        size_t byte_offset, size_t beg_seq,
                        size_t end_seq, size_t * beg_off,
                        size_t * end_off);

#if defined(__cplusplus)
}
#endif
#endif
