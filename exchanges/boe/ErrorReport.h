#ifndef __ERROR_REPORT_H__
#define __ERROR_REPORT_H__

/* The ErrorReport can be used to report error information back to the
 * caller.  It simply formalizes the reporting of error messages and
 * the cleanup required on them.  For instance, sometimes strerror may
 * be used to report system errors.  In other cases a custom buffer
 * may be created.  Either way the free flag indicates whether the
 * message needs to be released with a call to free(). */
struct ErrorReport {
    char *message;
    int free;
};

#define declare_new_error_report(name)          \
    struct ErrorReport name = {0,0}
#define destroy_new_error_report(name)                  \
    do{if(name->free!=0){free(name->message);}while(0)
#define report_errno_error(name)                                    \
    do{name->free=0;name->message=strerror_r(errno,0,0);}while(0)
#define report_constant_error_message(name,usermessage) \
    do{name->free=0;name->message=usermessage;}while(0)
#define report_error_message_and_return(name,usermessage)   \
    do{name->free=0;name->message=usermessage;return -1;}while(0)
#define cond_error_report(cond, err, msg)    \
    if(!(cond)){(err)->free=0;(err)->message=(msg);return -1;}

#endif
