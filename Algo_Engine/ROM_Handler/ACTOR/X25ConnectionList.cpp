// ConnectionList.cpp : implementation file
//

#include "stdafx.h"
#include "X25Connection.h"
#include "X25ConnectionList.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// X25ConnectionList

CX25ConnectionList::CX25ConnectionList()
{
}

CX25ConnectionList::~CX25ConnectionList()
{
}


BEGIN_MESSAGE_MAP(CX25ConnectionList, CListBox)
	//{{AFX_MSG_MAP(X25ConnectionList)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// X25ConnectionList message handlers

void CX25ConnectionList::DrawItem(LPDRAWITEMSTRUCT lpDraw) 
{
	CDC			DC;    
	CX25Connection	*pConnection	= NULL;
	int			Index				= lpDraw->itemID;
	CRect			Rect				= lpDraw->rcItem;
	CString		Count;
	
	DC.Attach(lpDraw->hDC);

	switch (lpDraw->itemAction)
		{
		case ODA_DRAWENTIRE:

			if ((pConnection = (CX25Connection *)GetItemData(Index)))
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

void CX25ConnectionList::Add(CX25Connection *pConnection)
{
	int Index;
	
	Index = AddString("");
	SetItemDataPtr(Index, pConnection);
	
}

void CX25ConnectionList::Delete(CX25Connection *pConnection)
{
	int			Index;

	for (Index = 0; Index < GetCount(); Index++)
		{
		if ((CX25Connection *)GetItemDataPtr(Index) == pConnection)
			DeleteString(Index);
		}
}