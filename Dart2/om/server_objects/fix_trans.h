/*************************************************************************
 * Copyright (c) 2013 by DART executions, LLC
 *
 * All Rights Reserved
 * ***********************************************************************/

#ifndef _FIX_TRANS_H__
#define _FIX_TRANS_H__

#if defined(__cplusplus)
extern "C" {
#endif
/** 
 * @file fix_trans.h Definition of the Fix translation objects and functions 
 * The objects and functions described here wrap a FIX message and allow
 * users to set integer and char values on these messages.
 * All ofp objects allocated will start off occupying one page of memory.
 * This allows for FIX messages of 3278 bytes all on one page.
 * If you believe that you need more than 3278 bytes only use the 
 * 'safe' adds below.  Other additions do not check space before 
 * performing copies and adds and thus are not safe for any message 
 * over 3278 bytes.
 */
#include "dart_order_obj.h"

/**
 * \struct fix_file_desc
 * \var typedef fix_file_desc ff_desc
 * This structure should point to a proper FIX
 * configuration file for a type of FIX message and
 * then the short definition of that FIX message's 35 
 * value.
 * @see \file dart_constants.h
 */
typedef struct fix_file_desc {
    char* filename;
    int f_len;
    short fix_type;
} ff_desc;
/**
 * \struct fix_translator
 * \var typedef fix_translator trans_t
 * The trans t structure encapsulates the logic for
 * creating and parsing fix messages.  The order objects created
 * and held by this object are transient and should be used
 * with caution.  This structure is created with the 
 * create_translator function and must be passed in to all 
 * functions defined on this interface.
 * @see create_translator()
 */
typedef struct fix_translator trans_t;
/**
 * \struct out_fix_parser
 * \var typedef out_fix_parser ofp
 * This is the forward definition of the out_fix_parser
 * structure.  This is the structure returned by the get_fixy_message
 * function. This structure manages the collection of tags that 
 * will become a FIX message. You can reset and then reuse an 
 * ofp directly or give back the structure to be reset through the 
 * destroy_fixy_message function.
 * @see get_fixy_message()
 * @see destroy_fixy_message()
 */
typedef struct out_fix_parser ofp; 

/**
 * Initializes the translator based on the file names  
 * passed in.  See the fix_file_desc struct above for
 * the values.  It is especially important to pass the
 * correct type for the in_types array.  We use the 
 * type in the file for the out types.
 * @param out_type an array of ff_desc structures.
 * @param out_len the length of the out_type parameter
 * @param in_types an array of ff_desc structures.
 * @param in_len the length of the in_types array.
 * @see \struct fix_file_desc
 */
trans_t* create_translator(ff_desc* out_types, int out_len,
                           ff_desc* in_types, int in_len);

/**
 * Uses the maps created in the trans_t to move the
 * fix values to the appropriate ROM fields.
 * @param t trans_t structure that has been loaded with the proper
 * definitions.
 * @param fix_msg raw FIX message to be parsed.
 * @param len the length of the FIX message.
 * @param doj a dart_order_obj structure that will hold the 
 * values parsed from the FIX message.
 * @param type a short value representing the value in FIX tag 35.
 * @see create_translator()
 * @see \struct dart_order_obj
 * @see \file dart_constants.h
 */
void translate_fix_msg(trans_t* t, char* fix_msg, int len, 
                       dart_order_obj* obj, short type);
/**
 * This returns a dart_order_obj with an iovec that
 * is prefilled with fix tags in the order specified 
 * in your configuration.
 * @param t a trans_t structure that has been loaded with the 
 * appropriate type.
 * @param type a short value representing the value of FIX tag 35
 */
ofp* get_fixy_message(trans_t* t, short type);
/**
 * When you are done with a dart_order_object obtained by using 
 * the get_fixy_message function return it by calling this.
 * Despite the name the ofp objects passed into this function 
 * are not destroyed but are instead "reset" and put back on the 
 * internal free list of the appropriate type.
 * @param t a trans_t structure from which this ofp was received
 * using the get_fixy_message function.
 * @param doj A out_fix_parser structure that you wish to have reset and
 * held by this trans_t object.
 */
void destroy_fixy_message(trans_t* t, ofp* doj);
/**
 * Here the value passed in as val is copied into the 
 * ofp's buffer and added as FIX tag of offset (i.e. if
 * offset is 38 then the string 38= will be added to the buffer.
 * This function handles the atoi of changing int offset to char
 * FIX tag.
 * This function will use the mappings set up for new orders.
 * @param t trans_t structure.
 * @param doj an out_fix_parser structure
 * @param offset should be the numeric value of a fix tag
 * @param val a character buffer that you wish to add to the fix message.
 * @param val_len length of the character buffer.
 */
void set_fix_val(trans_t*t, ofp* doj, int offset, char* val, int val_len);
/**
 * This function is the same as the previous but it tags the fix tag and length
 * of that tag and mearly does a memcpy rather than a itoa conversion.
 * @see set_fix_val()
 * @param t trans_t structure.
 * @param doj an out_fix_parser structure
 * @param fix_tag a char array that is composed of digits that will 
 * be a valid FIX tag ex: "38".
 * @param f_len the length of the fix_tag array.
 * @param val a character buffer that you wish to add to the fix message.
 * @param val_len length of the character buffer.
 */
void set_char_fix_val(trans_t*t, ofp* obj, char* fix_tag, int f_len, char* val, int val_len);
/**
 * This function is the same as the previous but it tags the fix tag and length
 * of that tag and mearly does a memcpy rather than a itoa conversion.
 * @see set_fix_val()
 * @param t trans_t structure.
 * @param doj an out_fix_parser structure
 * @param fix_tag a char array that is composed of digits that will 
 * be a valid FIX tag ex: "38".
 * @param f_len the length of the fix_tag array.
 * @param val a one byte character that you wish to add to the fix message.
 */
void add_single_char_val(trans_t* t, ofp* obj, char* fix_tag, int f_len, char val);
/**
 * This function returns a 1 if it was able to write val at the offset
 * requested, 0 if it could not find the value in your defaults and
 * was forced to write it at the end of the message, and -1 if the val_len 
 * is greater than the length of the default value.  In this last case 
 * nothing will be written into the buffer. Once overwritten the default
 * value will have been changed for this instance of the ofp.  It is up to
 * you to reset the value if that is desired.  The indent value can be used
 * if you only need to overwrite a part of the tag.
 * @param t trans_t structure.
 * @param p an out_fix_parser structure
 * @param offset should be the numeric value of a fix tag
 * @param val a character buffer that you wish to add to the fix message.
 * @param val_len length of the character buffer.
 */
int overwrite_fix_val(trans_t*t, ofp* p, int offset, char* val, int val_len,
                      int indent);
/**
 * This function allows you to reset the ofp structure without releasing it
 * back to the trans_t object. After this call all previous values set
 * in the ofp object will have been removed.
 * @param t trans_t structure.
 * @param doj an out_fix_parser structure
 */
void reset_object(trans_t* t, ofp* doj);

/**
 * This is a function that users of the library should not have to call
 * but it is exposed so that the fix_handler can access.
 * @param t is of type fix_trans
 * @param doj is a fix object which will be checksumed.
 */
void set_check_sum(trans_t* t, ofp* doj);
/**
 * Updates the FIX tag nine of the ofp object passed in. 
 * @param t is of type fix_trans
 * @param obj is a fix object which will be counted and have it's FIX tag nine
 * updated.
 */
void set_tag_nine(trans_t* t, ofp* obj);
/**
 * Handles the atoi conversion for both the fix_tag and value passed in.
 * This function will check to see if it should find the tag in the list of
 * Default fix tags that were defined in the type of this ofp.
 * @param t is of type fix_trans
 * @param doj is an \typedef ofp that will have the FIX tag 'fix_tag' set with
 * the value 'value'.
 * @param fix_tag an integer representing a valid FIX tag.
 * @param value and integer to be converted to a char value and added to the
 * ofp object's message.
 */
void set_num_val(trans_t* t, ofp* doj, int fix_tag, unsigned int value);
/**
 * This function acts much like the above set_num_val except it blindly appends
 * the fix_tag and value to the current end of the ofp message.
 * @see set_num_val()
 * @param t is of type fix_trans
 * @param doj is an \typedef ofp to which you wish to add this FIX tag value
 * combination. 
 * @param fix_tag an integer representing a valid FIX tag.
 * @param value and integer to be converted to a char value and added to the
 * ofp object's message.
 */
void add_num_val(trans_t* t, ofp* doj, int fix_tag, unsigned int value);
/**
 * This function acts much like the above set_num_val except it blindly appends
 * the fix_tag and value to the current end of the ofp message.
 * @see set_num_val()
 * @param t is of type fix_trans
 * @param obj is an \typedef ofp to which you wish to add this FIX tag value
 * combination. 
 * @param fix_tag a char array representing a valid FIX tag.
 * @param len represents the length of the fix tag array.
 * @param value and integer to be converted to a char value and added to the
 * ofp object's message.
 */
void add_value(trans_t* t, ofp* obj, char* fix_tag, int len, unsigned int val);
/**
 * This value takes the timeval struct and attempts to see if the offset is 
 * included in the list of default FIX tags defined for this ofp. If found
 * the time tag will be inserted at that position otherwise it will be appended
 * to the current end of the ofp's buffer.
 * @param t is of type fix_trans
 * @param obj is an \typedef ofp to which you wish to add this time value
 * @param timeval struct tm representing the time you wish to place in the FIX message.
 * @param offset the integer representation of the FIX tag you wish to associate with this
 * timestamp.
 */
void set_time_tag(trans_t* t, ofp* obj, 
                       struct tm *timeval, int offset);
/**
 * This function makes no attempt to find the offset value in it's default FIX tag map
 * and immediatly appends the values to the end of it's message string.
 * @param t is of type fix_trans
 * @param obj is an \typedef ofp to which you wish to add this time value
 * @param timeval struct tm representing the time you wish to place in the FIX message.
 * @param offset the integer representation of the FIX tag you wish to associate with this
 */
void add_time_tag(trans_t* t, ofp* obj, 
                       struct tm *timeval, int offset);

/**
 * This function will assure that space is reallocated if needed.
 * If you believe that you are creating messages that will be larger
 * than 3278 bytes then you should only use the following two safe 
 * functions.
 * @param t is of type fix_trans
 * @param doj is an \typedef ofp to which you wish to add this FIX tag value
 * combination. 
 * @param fix_tag an integer representing a valid FIX tag.
 * @param value and integer to be converted to a char value and added to the
 * ofp object's message.
 */
void safe_num_val(trans_t* t, ofp* obj, int fix_tag, unsigned int value);
/**
 * This 'safe' function will assure that space is reallocated if needed in
 * for messages larger than 3278 bytes.
 * @param t is of type fix_trans
 * @param doj is an \typedef ofp to which you wish to add this FIX tag value
 * combination. 
 * @param fix_tag const char* representation of a FIX tag
 * @param flen the length of the FIX tag
 * @param val a char* value to be placed in the FIX message.
 * @param len the length of val.
 */
void safe_val_add(trans_t* t, ofp* obj, const char* fix_tag, int flen,
                  char* val, int len);
/**
 * This function returns a pointer to the databuf_t structure where all FIX tag value pairs
 * are written two. In practice this should not be called by most client programs but it 
 * is used to pass the message to the send functions.
 * @param o an ofp structure who's FIX message is desired.
 * @return a pointer to the databuf_t structure that is controlled by the ofp.
 */
databuf_t* get_message(ofp* o);
void set_cannonical_str(trans_t* t, ofp* message, databuf_t* cmessage);
#if defined(__cplusplus)
} 
#endif
#endif  //FIX_TRANS
