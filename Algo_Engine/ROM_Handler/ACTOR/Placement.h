// Placement.h: interface for the CPlacement class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLACEMENT_H__023E06E4_14CF_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_PLACEMENT_H__023E06E4_14CF_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CChildFrame;

class CPlacement  
{
public:
	void Save();
	void Restore();
	void SetWnd(CFrameWnd *pWnd)				{ m_pWnd = pWnd;				}
	void SetTitle(const CString& Title)		{ m_Title = Title;			}
	void SetWindowPos(const int WindowPos)	{ m_WindowPos = WindowPos; }

	const CString& Title()				const	{ return m_Title;				}
	const int		WindowPos()			const	{ return m_WindowPos;		}

private:
	CFrameWnd	*m_pWnd;
	CString		m_Title;
	int			m_WindowPos;

public:
	CPlacement();
	virtual ~CPlacement();

};

#endif // !defined(AFX_PLACEMENT_H__023E06E4_14CF_11D2_8D11_006008CD4894__INCLUDED_)
