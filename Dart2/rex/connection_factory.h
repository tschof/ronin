#ifndef CONNECTION_FACTORY_H__
#define CONNECTION_FACTORY_H__

#if defined(__cplusplus)
extern "C" {
#endif


#include "polling_server.h"
#include "log_mnger.h"
#include "fix_parser.h"
#include "dart_handler.h"
#include "dart_order_obj.h"
#include "message_queue.h"

pserver* start_base_services(ex_config* bc);
pserver* start_lib_base_services(ex_config* bc);


login_info* build_logon_info_from_config(ex_config* ex);



#if defined(__cplusplus)
}
#endif      
#endif
