#include "RomCancel.h"

//CRomCancel::CRomCancel(const ROMMSG::CRomMessage& cancel, const CRomOrder& order)
CRomCancel::CRomCancel(CRomOrder& order) : CRomCsv(order.Message())
{
    SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_CANCEL);
    //CRomOrder::tChildrenIT it = (order.Children()).begin();
    //CRomCsv* pCsv = NULL;
    /*while (it != (order.Children()).end())
    {
        pCsv = new CRomCsv(*((*it)->csv()));
        pCsv->SetAt(ROMFIELDS::CSV_SHARES, (*it)->OriginalShares());//Ratio
        GetChildren().push_back(pCsv);
        it++;
    }*/
}

CRomCancel::~CRomCancel(void)
{
	
}



