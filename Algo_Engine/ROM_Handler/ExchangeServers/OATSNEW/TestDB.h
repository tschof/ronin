// TestDB.h: interface for the CTestDB class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_TESTDB_H__60CE0DC6_D557_4BA5_91A6_D8B16FFB5222__INCLUDED_)
#define AFX_TESTDB_H__60CE0DC6_D557_4BA5_91A6_D8B16FFB5222__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CQueueRequest;
class CTestDB  
{
public:
	CTestDB( CString szMsgFile );
	virtual ~CTestDB();
	
	bool ProcessMessage( CQueueRequest *pMessage );

private:
	CStdioFile			MsgFile;
};

#endif // !defined(AFX_TESTDB_H__60CE0DC6_D557_4BA5_91A6_D8B16FFB5222__INCLUDED_)
