// Placement.cpp: implementation of the CPlacement class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Placement.h"
#include "ChildFrm.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPlacement::CPlacement()
{
}

CPlacement::~CPlacement()
{
}

void CPlacement::Save()
{
	CWinApp				*pApp = AfxGetApp();
	WINDOWPLACEMENT	Place;
	CString Title;
	
	Title.Format("%s-%d", m_Title, m_WindowPos);

	m_pWnd->GetWindowPlacement(&Place);

	pApp->WriteProfileInt(Title, "MinX", Place.ptMinPosition.x);
	pApp->WriteProfileInt(Title, "MinY", Place.ptMinPosition.y);
	pApp->WriteProfileInt(Title, "MaxX", Place.ptMaxPosition.x);
	pApp->WriteProfileInt(Title, "MaxY", Place.ptMaxPosition.y);
	
	pApp->WriteProfileInt(Title, "left", Place.rcNormalPosition.left);
	pApp->WriteProfileInt(Title, "top", Place.rcNormalPosition.top);
	pApp->WriteProfileInt(Title, "right", Place.rcNormalPosition.right);
	pApp->WriteProfileInt(Title, "bottom", Place.rcNormalPosition.bottom);
	
	pApp->WriteProfileInt(Title, "flags", Place.flags);	
	pApp->WriteProfileInt(Title, "length", Place.length);	
	pApp->WriteProfileInt(Title, "showCmd", Place.showCmd);	

}

void CPlacement::Restore()
{
	CWinApp				*pApp = AfxGetApp();
	WINDOWPLACEMENT	Place;
	CString Title;
	
	Title.Format("%s-%d", m_Title, m_WindowPos);

	Place.ptMinPosition.x = pApp->GetProfileInt(Title, "MinX", 0);
	Place.ptMinPosition.y = pApp->GetProfileInt(Title, "MinY", 0);
	Place.ptMaxPosition.x = pApp->GetProfileInt(Title, "MaxX", 0);
	Place.ptMaxPosition.y = pApp->GetProfileInt(Title, "MaxY", 0);
	
	Place.rcNormalPosition.left	= pApp->GetProfileInt(Title, "left", 0);
	Place.rcNormalPosition.top		= pApp->GetProfileInt(Title, "top", 0);
	Place.rcNormalPosition.right	= pApp->GetProfileInt(Title, "right", 200);
	Place.rcNormalPosition.bottom = pApp->GetProfileInt(Title, "bottom", 200);
	
	Place.flags		= pApp->GetProfileInt(Title, "flags", 0);	
	Place.length	= pApp->GetProfileInt(Title, "length", 0);	
	Place.showCmd	= pApp->GetProfileInt(Title, "showCmd", SW_SHOWNORMAL);	

	m_pWnd->SetWindowPlacement(&Place);
}