// ActDialog.cpp : implementation file
//

#include "stdafx.h"
#include "actor.h"
#include "ActDialog.h"
#include "MessageApp.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CActDialog dialog


CActDialog::CActDialog(CWnd* pParent /*=NULL*/)
	: CDialog(CActDialog::IDD, pParent)
{
	//{{AFX_DATA_INIT(CActDialog)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CActDialog::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CActDialog)
		// NOTE: the ClassWizard will add DDX and DDV calls here
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CActDialog, CDialog)
	//{{AFX_MSG_MAP(CActDialog)
	ON_CBN_SELCHANGE(IDC_COMBO_FUNCTION, OnSelchangeComboFunction)
	ON_CBN_SELCHANGE(IDC_COMBO_ASOF, OnSelchangeComboAsof)
	ON_CBN_SELCHANGE(IDC_COMBO_SECCLS, OnSelchangeComboSeccls)
	ON_CBN_SELCHANGE(IDC_COMBO_BSX, OnSelchangeComboBsx)
	ON_EN_KILLFOCUS(IDC_EDIT_REFNUM, OnKillfocusEditRefnum)
	ON_EN_KILLFOCUS(IDC_EDIT_VOLUME, OnKillfocusEditVolume)
	ON_EN_KILLFOCUS(IDC_EDIT_PRICEDOLLAR, OnKillfocusEditPricedollar)
	//ON_CBN_SELCHANGE(IDC_COMBO_SYMBOL, OnSelchangeComboSymbol)
	ON_EN_KILLFOCUS(IDC_EDIT_PRICENUM, OnKillfocusEditPricenum)
	ON_CBN_SELCHANGE(IDC_COMBO_DEN, OnSelchangeComboDen)
	ON_CBN_SELCHANGE(IDC_COMBO_TRADEMOD, OnSelchangeComboTrademod)
	ON_BN_CLICKED(IDC_PRICEOVERIDE, OnPriceoveride)
	ON_EN_KILLFOCUS(IDC_EDIT_OEID, OnKillfocusEditOeid)
	ON_EN_KILLFOCUS(IDC_EDIT_OEGU, OnKillfocusEditOegu)
	ON_EN_KILLFOCUS(IDC_EDIT_OECLEARNUM, OnKillfocusEditOeclearnum)
	ON_EN_KILLFOCUS(IDC_EDIT_MMID, OnKillfocusEditMmid)
	ON_EN_KILLFOCUS(IDC_EDIT_MMGU, OnKillfocusEditMmgu)
	ON_EN_KILLFOCUS(IDC_EDIT_MMCLEARNUM, OnKillfocusEditMmclearnum)
	ON_CBN_SELCHANGE(IDC_COMBO_MMPA, OnSelchangeComboMmpa)
	ON_CBN_SELCHANGE(IDC_COMBO_SPECIALINDICATOR, OnSelchangeComboSpecialindicator)
	ON_BN_CLICKED(IDC_REPORTFLAG, OnReportflag)
	ON_EN_KILLFOCUS(IDC_EDIT_MEMO, OnKillfocusEditMemo)
	ON_EN_KILLFOCUS(IDC_EDIT_TRADEPRICEEXP, OnKillfocusEditTradepriceexp)
	ON_EN_KILLFOCUS(IDC_EDIT_TRADEPRICEEXP2, OnKillfocusEditTradepriceexp2)
	ON_CBN_SELCHANGE(IDC_COMBO_CLEARFLAG, OnSelchangeComboClearflag)
	ON_BN_CLICKED(IDC_SENDACT, OnSendact)
	ON_WM_NCLBUTTONDBLCLK()
	ON_EN_CHANGE(IDC_SYMBOL_EDIT, OnChangeSymbolEdit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CActDialog message handlers

BOOL CActDialog::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetWindowText(m_ActMessage.Message());
	GetWindowRect(m_Size);
	SetWindowPos(NULL, 0, 0, m_Size.Width(), 25, SWP_NOMOVE);

	CEdit *pEdit = (CEdit*) GetDlgItem(IDC_EDIT_NUMSEND);
	pEdit->SetWindowText("1");

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

void CActDialog::OnSelchangeComboFunction() 
{
	CComboBox *control = (CComboBox*) GetDlgItem(IDC_COMBO_FUNCTION);
	CString value;
	
	switch(control->GetCurSel())
	{
	case 0:
		value = "U";
		break;
	case 1:
		value = "E";
		break;
	case 2:
		value = "C";
		break;
	case 3:
		value = "N";
		break;
	case 4:
		value = "A";
		break;
	case 5:
		value = "D";
		break;
	case  6:
		value = "O";
		break;
	case 7:
		value = "B";
		break;
	case 8:
		value = "R";
		break;
	}

	m_ActMessage.SetAt(ACT_FUNCTION, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnSelchangeComboAsof() 
{
	CComboBox *control = (CComboBox*) GetDlgItem(IDC_COMBO_ASOF);
	CString value;

	switch(control->GetCurSel())
	{
	case 0:
		value = " ";
		break;
	case 1:
		value = "Y";
		break;
	case 2:
		value = "T";
		break;
	case 3:
		value = "R";
		break;
	}

	m_ActMessage.SetAt(ACT_ASOF, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnSelchangeComboSeccls() 
{
	CComboBox *control = (CComboBox*) GetDlgItem(IDC_COMBO_SECCLS);
	CString value;

	switch(control->GetCurSel())
	{
	case 0:
		value = "R";
		break;
	case 1:
		value = "N";
		break;
	case 2:
		value = "C";
		break;
	case 3:
		value = "P";
		break;
	case 4:
		value = "T";
		break;
	case 5:
		value = "X";
		break;
	case 6:
		value = "Y";
		break;
	case 7:
		value = "Z";
		break;
	}

	m_ActMessage.SetAt(ACT_SECURITYCLASS, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnSelchangeComboBsx() 
{
	CComboBox *control = (CComboBox*) GetDlgItem(IDC_COMBO_BSX);
	CString value;

	switch(control->GetCurSel())
	{
	case 0:
		value = "B";
		break;
	case 1:
		value = "S";
		break;
	case 2:
		value = "X";
		break;
	case 3:
		value = "Z";
		break;
	case 4:
		value = "E";
		break;
	case 5:
		value = "C";
		break;
	case 6:
		value = "K";
		break;
	case 7:
		value = "P";
		break;
	case 8:
		value = "A";
		break;
	}

	m_ActMessage.SetAt(ACT_BSX, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnKillfocusEditRefnum() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_REFNUM);
	CString value;

	control->GetWindowText(value);
	unsigned char length = value.GetLength();
	if(length > 6)
		value = value.Left(6);
	else
		value = value + CString(' ', 6 - length);

	SetWindowText(value);
	m_ActMessage.SetAt(ACT_REFERENCENUMBER, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnKillfocusEditVolume() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_VOLUME);
	CString value;

	control->GetWindowText(value);
	unsigned char length = value.GetLength();
	if(length > 8)
		value = value.Left(8);
	else
		value = CString('0', 8 - length) + value;
	control->SetWindowText(value);

	m_ActMessage.SetAt(ACT_VOLUME, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnSelchangeComboSymbol() 
{
	/*CComboBox *control = (CComboBox*) GetDlgItem(IDC_COMBO_SYMBOL);
	CString value;

	control->GetLBText(control->GetCurSel(), value);
	value = value.Right(5);

	control->SetWindowText(value);
	m_ActMessage.SetAt(ACT_SYMBOL, value);
	SetWindowText(m_ActMessage.Message());*/
}

void CActDialog::OnKillfocusEditPricedollar() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_PRICEDOLLAR);
	CString value;

	control->GetWindowText(value);
	unsigned char length = value.GetLength();
	if(length < 4)
		value = CString('0', 4 - length) + value;
	else if(length > 5)
		value = value.Left(4);

	control->SetWindowText(value);
	m_ActMessage.SetAt(ACT_PRICEDOLLAR, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnKillfocusEditPricenum() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_PRICENUM);
	CString value;

	control->GetWindowText(value);
	long num = strtol(value, NULL, 10);
	unsigned char length = value.GetLength();

	if(num > 255 || num < 0)
		value = "000";
	if(length < 3)
		value = CString('0', 3 - length) + value;

	control->SetWindowText(value);
	m_ActMessage.SetAt(ACT_PRICENUMERATOR, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnSelchangeComboDen() 
{
	CComboBox *control = (CComboBox*) GetDlgItem(IDC_COMBO_DEN);
	CString value;

	switch(control->GetCurSel())
	{
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
	case 7:
	case 8:
		control->GetLBText(control->GetCurSel(), value);
		//value = CString((char)control->GetCurSel()+1+0x30, 1);
		break;
	case 9:
		value = "A";
		break;
	case 10:
		value = "B";
	}

	m_ActMessage.SetAt(ACT_PRICETRADEDIGIT, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnSelchangeComboTrademod() 
{
	CComboBox *control = (CComboBox*) GetDlgItem(IDC_COMBO_TRADEMOD);
	CString value;

	unsigned char length = value.GetLength();

	switch(control->GetCurSel())
	{
	case 0:
		value = "SLD";
		break;
	case 1:
		value = "  B";
		break;
	case 2:
		value = " SB";
		break;
	case 3:
		value = "SNN";
		break;
	case 4:
		value = "  C";
		break;
	case 5:
		value = " ND";
		break;
	case 6:
		value = "  T";
		break;
	case 7:
		value = "  W";
		break;
	}	

	m_ActMessage.SetAt(ACT_TRADEMODIFIER, value);
	SetWindowText(m_ActMessage.Message());
}


void CActDialog::OnSelchangeComboClearflag() 
{
	CComboBox *control = (CComboBox*) GetDlgItem(IDC_COMBO_CLEARFLAG);
	CString value;

	unsigned char length = value.GetLength();

	switch(control->GetCurSel())
	{
	case 0:
		value = " ";
		break;
	case 1:
		value = "G";
		break;
	case 2:
		value = "N";
		break;
	case 3:
		value = "L";
		break;
	case 4:
		value = "Q";
		break;
	case 5:
		value = "Z";
	}	

	m_ActMessage.SetAt(ACT_CLEARINGFLAG, value);
	SetWindowText(m_ActMessage.Message());
}


void CActDialog::OnPriceoveride() 
{
	CButton *control = (CButton*) GetDlgItem(IDC_PRICEOVERIDE);
	CString value;

	if(control->GetCheck())
		value = "O";
	else
		value = " ";

	m_ActMessage.SetAt(ACT_PRICEOVERIDE, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnReportflag() 
{
	CButton *control = (CButton*) GetDlgItem(IDC_REPORTFLAG);
	CString value;

	if(control->GetCheck())
		value = "N";
	else
		value = " ";

	m_ActMessage.SetAt(ACT_TRADEREPORTFLAG, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnKillfocusEditOeid() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_OEID);
	CString value;

	control->GetWindowText(value);
	unsigned char length = value.GetLength();
	if(length < 4)
		value = "    ";
	else if(length > 4)
		value = value.Left(4);

	control->SetWindowText(value);
	m_ActMessage.SetAt(ACT_OEID, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnKillfocusEditOegu() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_OEGU);
	CString value;

	control->GetWindowText(value);
	unsigned char length = value.GetLength();
	if(length < 4)
		value = "    ";
	else if(length > 4)
		value = value.Left(4);

	control->SetWindowText(value);
	m_ActMessage.SetAt(ACT_OEGU, value);
	SetWindowText(m_ActMessage.Message());
}



void CActDialog::OnKillfocusEditOeclearnum() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_OECLEARNUM);
	CString value;

	control->GetWindowText(value);
	unsigned char length = value.GetLength();
	if(length < 4)
		value = CString('0', 4 - length) + value;
	else if(length > 4)
		value = value.Left(4);

	control->SetWindowText(value);
	m_ActMessage.SetAt(ACT_OECLEARNUMBER, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnKillfocusEditMmid() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_MMID);
	CString value;

	control->GetWindowText(value);
	unsigned char length = value.GetLength();
	if(length < 4)
		value = "    ";
	else if(length > 4)
		value = value.Left(4);

	control->SetWindowText(value);
	m_ActMessage.SetAt(ACT_MMID, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnKillfocusEditMmgu() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_MMGU);
	CString value;

	control->GetWindowText(value);
	unsigned char length = value.GetLength();
	if(length < 4)
		value = "    ";
	else if(length > 4)
		value = value.Left(4);

	control->SetWindowText(value);
	m_ActMessage.SetAt(ACT_MMGU, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnKillfocusEditMmclearnum() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_MMCLEARNUM);
	CString value;

	control->GetWindowText(value);
	unsigned char length = value.GetLength();
	if(length < 4)
		value = CString('0', 4 - length) + value;
	else if(length > 4)
		value = value.Left(4);

	control->SetWindowText(value);
	m_ActMessage.SetAt(ACT_MMCLEARNUMBER, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnSelchangeComboMmpa() 
{
	CComboBox *control = (CComboBox*) GetDlgItem(IDC_COMBO_MMPA);
	CString value;

	unsigned char length = value.GetLength();

	switch(control->GetCurSel())
	{
	case 0:
		value = "P";
		break;
	case 1:
		value = "R";
		break;
	case 2:
		value = "A";
	}	

	m_ActMessage.SetAt(ACT_MMPAINDICATOR, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnSelchangeComboSpecialindicator() 
{
	CComboBox *control = (CComboBox*) GetDlgItem(IDC_COMBO_SPECIALINDICATOR);
	CString value;

	unsigned char length = value.GetLength();

	switch(control->GetCurSel())
	{
	case 0:
		value = " ";
		break;
	case 1:
		value = "Y";
		break;
	case 2:
		value = "S";
		break;
	case 3:
		value = "X";
	}	

	m_ActMessage.SetAt(ACT_SPECIALTRADEINDICATOR, value);
	SetWindowText(m_ActMessage.Message());	
}


void CActDialog::OnKillfocusEditMemo() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_MEMO);
	CString value;

	control->GetWindowText(value);
	unsigned char length = value.GetLength();
	if(length < 10)
		value = value + CString(' ', 10 - length);
	else if(length > 10)
		value = value.Left(10);

	control->SetWindowText(value);
	m_ActMessage.SetAt(ACT_MEMO, value);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnKillfocusEditTradepriceexp() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_TRADEPRICEEXP);
	CEdit *control2 = (CEdit*) GetDlgItem(IDC_EDIT_TRADEPRICEEXP2);
	CString value, value2;

	control->GetWindowText(value);
	control2->GetWindowText(value2);
	unsigned char length = value.GetLength();
	if(length < 6)
		value = CString('0', 6 - length) + value;
	else if(length > 6)
		value = value.Left(6);

	control->SetWindowText(value);

	if(value2.GetLength())
		m_ActMessage.SetAt(ACT_TRADEDECIMALPRICE, value + value2);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnKillfocusEditTradepriceexp2() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_EDIT_TRADEPRICEEXP);
	CEdit *control2 = (CEdit*) GetDlgItem(IDC_EDIT_TRADEPRICEEXP2);
	CString value, value2;

	control->GetWindowText(value);
	control2->GetWindowText(value2);
	unsigned char length = value2.GetLength();
	if(length < 6)
		value2 = value2 + CString('0', 6 - length);
	else if(length > 6)
		value2 = value2.Left(6);

	control2->SetWindowText(value2);

	if(value.GetLength())
		m_ActMessage.SetAt(ACT_TRADEDECIMALPRICE, value + value2);
	SetWindowText(m_ActMessage.Message());
}

void CActDialog::OnSendact() 
{
	CMessageApp *pApp = (CMessageApp*)AfxGetApp();

	CEdit *pEdit = (CEdit*) GetDlgItem(IDC_EDIT_NUMSEND);
	CString sLoop;
	pEdit->GetWindowText(sLoop);
	int iLoop = atoi(sLoop);

	for(int i = 0; i < iLoop; i++)
	{
		CString ActMessage = m_ActMessage.Message();
		pApp->Exchange()->ProcessMessage(ActMessage);
		SetWindowText(ActMessage);
	}
}

void CActDialog::OnNcLButtonDblClk( UINT nHitTest, CPoint point )
{
	static BOOL show = FALSE;
	
	if(show)
		SetWindowPos(NULL, 0, 0, m_Size.Width(), 25, SWP_NOMOVE);
	else
		SetWindowPos(NULL, 0, 0, m_Size.Width(), m_Size.Height(), SWP_NOMOVE);

	show = !show;
}

void CActDialog::OnChangeSymbolEdit() 
{
	CEdit *control = (CEdit*) GetDlgItem(IDC_SYMBOL_EDIT);
	CString value;

	control->GetWindowText(value);
	value = value.Right(5);

	if(value.GetLength())
		m_ActMessage.SetAt(ACT_SYMBOL, value);
	SetWindowText(m_ActMessage.Message());	
}
