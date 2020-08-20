// OatsHeader.cpp: implementation of the COatsHeader class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OatsHeader.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

COatsHeader::COatsHeader(const CString& File, const int FOREId)	:
	COatsRecord(OATS_HEADER_TYPE)
{
	char	workString[128];

	GetPrivateProfileString("Oats Header Information", "Version",	"",	workString,	128, File);
	m_Fields.SetAt(Version, workString);

	CTime	Current = CTime::GetCurrentTime();
	m_Fields.SetAt(GenerationDate, Current.Format("%Y%m%d"));

	m_Fields.SetAt(ForeId, FOREId);
	m_Fields.SetAt(ReportingOSOId, "");
	
	GetPrivateProfileString("Oats Header Information", "UserId",	"",	workString,	128, File);
	m_Fields.SetAt(UserId, workString);

	GetPrivateProfileString("Oats Header Information", "Password",	"",	workString,	128, File);
	m_Fields.SetAt(Password, workString);

	GetPrivateProfileString("Oats Header Information", "MPID",	"",	workString,	128, File);
	m_Fields.SetAt(MPID, workString);
}

COatsHeader::~COatsHeader()
{
}
