// FromIslandCancelReject.cpp: implementation of the CFromIslandCancelReject class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Island.h"
#include "FromIslandCancelReject.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CFromIslandCancelReject::CFromIslandCancelReject(const CString& Message)
{
	CString	workString;
	char Type = Message[1];

	if (Type == 'K')
	{
		m_SourceId	= CIslandMessage::Get(Message, 2 , 4);
		m_OrderTag	= CIslandMessage::Get(Message, 6 , 10);
		m_Reason	= CIslandMessage::Get(Message, 16 , 8);
	}

}

