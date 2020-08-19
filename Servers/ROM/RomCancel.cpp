#include "RomCancel.h"

CRomCancel::CRomCancel(CRomOrderNew& order) : CCsv(order.Message())
{
    SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
}

CRomCancel::CRomCancel(const std::string& destID)
{
  SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCELS);
  SetAt(ROMFIELDS::CSV_ORDERDEST, destID);
}

CRomCancel::~CRomCancel(void)
{

}



