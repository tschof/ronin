// CBOEHandler.cpp: implementation of the CCBOEHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "CMSHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CCMSHandler::CCMSHandler()
{
	m_pTags = new COptionMemTagSet();
	m_pTags->InitMep();
}

