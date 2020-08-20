// UserData.cpp : implementation file
#include "stdafx.h"
#include "UserData.h"
#include <vector>
#include "Main.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CUserData

IMPLEMENT_DYNAMIC(CUserData, CRecordset)

CUserData::CUserData(CDatabase* pdb)
	: CRecordset(pdb)
{
	//{{AFX_FIELD_INIT(CUserData)
	m_UserName = _T("");
	m_Password = _T("");
	m_Token = _T("");
	m_UserParam = _T("");
	m_TokenParam = _T("");
	m_nFields = 3;
	m_nParams = 2;
	//}}AFX_FIELD_INIT
	m_strFilter = "username = ? AND token = ?";
	try
	{
		if( !Open() )
			throw exception("fail to open recordset");
	}
	catch(...)
	{
		throw exception("open recordset exception");
	}
}

CString CUserData::GetDefaultConnect()
{
	return "";
}

CString CUserData::GetDefaultSQL()
{
	return "rom.dbo.OMuser";
}

void CUserData::DoFieldExchange(CFieldExchange* pFX)
{
	//{{AFX_FIELD_MAP(CUserData)
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("[username]"), m_UserName);
	RFX_Text(pFX, _T("[password]"), m_Password);
	RFX_Text(pFX, _T("[token]"), m_Token);

	pFX->SetFieldType( CFieldExchange::param );
	RFX_Text(pFX, _T("[username]"), m_UserParam);
	RFX_Text(pFX, _T("[token]"), m_TokenParam);
	//}}AFX_FIELD_MAP
}

bool CUserData::IsValidLogin( const std::string & szUser, const std::string & szToken )
{
	if(	!CRecordset::IsOpen() )
	{
		if( !Open() )
			CMain::getInstance()->AddError("CRecordset::Open() failure");
		return false;
	}

	if(	!CRecordset::CanRestart() )
	{
		CMain::getInstance()->AddError("CRecordset::CanRestart() failure");
		return false;
	}

	m_UserParam = szUser.c_str();
	m_TokenParam = szToken.c_str();
	if( !CRecordset::Requery() )
	{
		CMain::getInstance()->AddError("CRecordset::Requery failure");
		return false;
	}
	while( !IsEOF() )
	{
		if( m_TokenParam == m_Token )
			return true;
		MoveNext();
	}
	CString szError;
	szError.Format("Invalid User Login: %s; %s", m_UserParam, m_TokenParam );
	CMain::getInstance()->m_log.write( LOG_DEBUG, (LPCSTR)szError );
	return false;
}


/////////////////////////////////////////////////////////////////////////////
// CUserData diagnostics
#ifdef _DEBUG
void CUserData::AssertValid() const
{
	CRecordset::AssertValid();
}

void CUserData::Dump(CDumpContext& dc) const
{
	CRecordset::Dump(dc);
}
#endif //_DEBUG

