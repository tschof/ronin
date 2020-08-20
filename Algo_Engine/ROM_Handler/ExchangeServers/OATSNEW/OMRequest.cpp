// OMRequest.cpp: implementation of the COMRequest class.
//
//////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include "oats.h"
#include "OMRequest.h"
#include "csv.h"
#include "OrderMsg.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COMRequest::COMRequest()
{
}

COMRequest::~COMRequest()
{
}


bool COMRequest::Process()
{
	CCsv csv;
	csv.Bust( CString(m_szMsg) );
	COrderMsg msg( &csv );
	msg.Process();
	return true;
}
