// CommonDatabase.h: interface for the CCommonDatabase class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_COMMONDATABASE_H__C4477B12_DF72_11D1_8CFF_006008CD4894__INCLUDED_)
#define AFX_COMMONDATABASE_H__C4477B12_DF72_11D1_8CFF_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CCommonDatabase
{
public:
	CDatabase	*Database()	{ return m_pDatabase; }
	
private:
	// database object
		CDatabase	*m_pDatabase;

	// connection parameters
		CString				m_Connect;

public:
	BOOL		Connect();

private:
		CString				m_User;
		CString				m_Password;
		CString				m_Database;
		CString				m_DataSource;
		CString				m_Server;
		CString				m_WorkStation;
		

public:
				CCommonDatabase(const CString& FileName);
	virtual	~CCommonDatabase();

};

#endif // !defined(AFX_COMMONDATABASE_H__C4477B12_DF72_11D1_8CFF_006008CD4894__INCLUDED_)
