#ifndef USER_DEFINED_FIELDS
#define USER_DEFINED_FIELDS

#include "Field.h"

namespace FIX
{
	USER_DEFINE_INT(OMExchange, 10001);
	USER_DEFINE_STRING(SSFirm, 5700);
    USER_DEFINE_STRING(RoutingInst, 9303);
    USER_DEFINE_STRING(PfdMktMkr, 9307);
    USER_DEFINE_STRING(NYSEDirectplus, 5194);
    USER_DEFINE_STRING(SMRouteFlag, 9375);
    USER_DEFINE_STRING(SMExecAlgorFlag, 9376);
    USER_DEFINE_STRING(LiqProvFlg, 9730);
}
  


#endif
