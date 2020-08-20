// SeqDlg.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "SeqDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSeqDlg dialog


CSeqDlg::CSeqDlg(BOOL bOutgoing, CWnd* pParent /*=NULL*/)
	: CDialog(CSeqDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSeqDlg)
	m_SeqNum = 0;
	m_bOutgoing = bOutgoing;
	//}}AFX_DATA_INIT
}


void CSeqDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSeqDlg)
	DDX_Text(pDX, IDC_SEQNUM, m_SeqNum);
	DDV_MinMaxLong(pDX, m_SeqNum, 1, 99999);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSeqDlg, CDialog)
	//{{AFX_MSG_MAP(CSeqDlg)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CSeqDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	if (m_bOutgoing == TRUE)
	{
		SetWindowText("Reset Order ID");
	}
	else
	{
		SetWindowText("Reset incoming sequence number");
	}

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
// CSeqDlg message handlers
