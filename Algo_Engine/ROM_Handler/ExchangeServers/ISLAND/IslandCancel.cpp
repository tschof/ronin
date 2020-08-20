// IslandCancel.cpp: implementation of the CIslandCancel class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "IslandCancel.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CIslandCancel::CIslandCancel(const CCancel& Cancel)
{

	m_Fields.AddField(9,		0);
	m_Fields.AddField(10,	Cancel.OrderTag());

    CString lcsTradeFor = Cancel.Owner();
    if(lcsTradeFor.GetLength() > 4) {
        lcsTradeFor = Cancel.Owner().Left(4);
    }
	m_Fields.AddField(4,lcsTradeFor);

	//m_Fields.AddField(4,	Cancel.Owner()	);
	m_Fields.AddField('X');

}

