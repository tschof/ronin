// MainFrm.h : interface of the CMainFrame class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_MAINFRM_H__CB666CFA_26ED_11D2_8D11_006008CD4894__INCLUDED_)
#define AFX_MAINFRM_H__CB666CFA_26ED_11D2_8D11_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Placement.h"

class CMainFrame : public CMDIFrameWnd
{
	DECLARE_DYNAMIC(CMainFrame)
public:
	CMainFrame();

private:
	CPlacement	m_Placement;
	CString		m_Title;
	int			m_WindowPos;

// Operations
public:
	const CString& Title()					{ return m_Title;			}
	void	SetTitle(const CString& Title)
		{ 
		m_Title = Title;	
		m_Placement.SetTitle(Title);		
		}

	void	SetWindowPos(int Pos)			
		{ 
		m_WindowPos = Pos;
		m_Placement.SetWindowPos(Pos);
		}

	int	WindowPos()							{ return m_WindowPos;	}

	void	Restore()							{ m_Placement.Restore();}
	void	Save()								{ m_Placement.Save();	}

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMainFrame)
	public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg void OnClose();
	afx_msg void OnDestroy();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINFRM_H__CB666CFA_26ED_11D2_8D11_006008CD4894__INCLUDED_)
