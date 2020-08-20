#include "RomReject.h"

CRomReject::CRomReject(const CRomCsv& msg, std::string& reason): CRomCsv(msg)
{
	//m_pCsv = new ROMMSG::CRomMessage(msg);
	
	char cmd = cGet(ROMFIELDS::CSV_COMMAND);
	//m_pCsv->SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
	SetAt(ROMFIELDS::CSV_TEXT, reason);

	if (cmd == ROMVALUE::COMMAND_ORDER)
	{
		SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
		SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
	}
	else if (cmd == ROMVALUE::COMMAND_CANCEL ||
			 cmd == ROMVALUE::COMMAND_CANCELS)
	{
		SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
		SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_CANCELREJECTED);
	}
	else if (cmd == ROMVALUE::COMMAND_REPLACE)
	{
		SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
		SetAt(ROMFIELDS::CSV_STATUS, ROMVALUE::STATUS_REPLACEREJECTED);
	}
	else
	{
		SetAt(ROMFIELDS::CSV_COMMAND, ROMVALUE::COMMAND_REJECT);
	}
}

CRomReject::~CRomReject(void)
{
	//if (m_pCsv)
	//{
	//	delete m_pCsv;
	//}
}

//std::string& CRomReject::Message()
//{
//	return m_pCsv->Message(); 
//}
