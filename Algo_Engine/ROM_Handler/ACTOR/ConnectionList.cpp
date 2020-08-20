// ConnectionList.cpp : implementation file
//

#include "stdafx.h"
#include "Connection.h"
#include "ConnectionList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CConnectionList

CConnectionList::CConnectionList()
{
}

CConnectionList::~CConnectionList()
{
}


BEGIN_MESSAGE_MAP(CConnectionList, CListBox)
	//{{AFX_MSG_MAP(CConnectionList)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CConnectionList message handlers

void CConnectionList::DrawItem(LPDRAWITEMSTRUCT lpDraw) 
{
	CDC			DC;    
	CConnection	*pConnection	= NULL;
	int			Index				= lpDraw->itemID;
	CRect			Rect				= lpDraw->rcItem;
	CString		Count;
	
	DC.Attach(lpDraw->hDC);

	switch (lpDraw->itemAction)
		{
		case ODA_DRAWENTIRE:

			if ((pConnection = (CConnection *)GetItemData(Index)))
				{
				if (!(pConnection->m_Connected))
					{
					DC.SetBkColor(RGB(255,0,0));
					DC.SetTextColor(RGB(255,255,255));
					}
				else if (!(pConnection->LoggedOn()))
					{
					DC.SetBkColor(RGB(255,255,0));
					DC.SetTextColor(RGB(0,0,0));
					}
				else
					{
					DC.SetBkColor(RGB(255,255,255));
					DC.SetTextColor(RGB(0,0,0));
					}
			
				// erase the entire area
					DC.ExtTextOut(Rect.left, Rect.top, ETO_OPAQUE, &Rect, "", NULL);

				DC.ExtTextOut(Rect.left, Rect.top, ETO_OPAQUE, &Rect, pConnection->m_Name, NULL);
				Rect.left = Rect.left + 90;

				DC.ExtTextOut(Rect.left, Rect.top, ETO_OPAQUE, &Rect, pConnection->m_ConnectionTime, NULL);
				Rect.left = Rect.left + 120;

				Count.Format("%ld", pConnection->m_Sent);
				DC.ExtTextOut(Rect.left, Rect.top, ETO_OPAQUE, &Rect, Count, NULL);
				Rect.left = Rect.left + 30;

				Count.Format("%ld", pConnection->m_Received);
				DC.ExtTextOut(Rect.left, Rect.top, ETO_OPAQUE, &Rect, Count, NULL);

				}

			break;

		default:
			break;

		}

	DC.Detach();
	
	
}

void CConnectionList::Add(CConnection *pConnection)
{
	int Index;
	
	Index = AddString("");
	SetItemDataPtr(Index, pConnection);
	
}

void CConnectionList::Delete(CConnection *pConnection)
{
	int			Index;

	for (Index = 0; Index < GetCount(); Index++)
		{
		if ((CConnection *)GetItemDataPtr(Index) == pConnection)
			DeleteString(Index);
		}
}