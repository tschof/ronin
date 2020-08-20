// X25ClientConnection.h: interface for the CX25ClientConnection class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_X25CLIENTCONNECTION_H__BCF48873_98E5_11D2_9F45_00105AA84069__INCLUDED_)
#define AFX_X25CLIENTCONNECTION_H__BCF48873_98E5_11D2_9F45_00105AA84069__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "X25Connection.h"

class CX25ClientConnection : public CX25Connection  
{
public:
	CX25ClientConnection();
	virtual ~CX25ClientConnection();

private:
	void ProcessMessage(CString& Message);

public:
	void ProcessStatus(const CString& Message);
	void ProcessError(const CString& Error);
	void ProcessLogEntry(const CString& Entry);

public:
	void Close(/*int ErrorCode*/);
	void Connect(int nErrorCode);

};

#endif // !defined(AFX_X25CLIENTCONNECTION_H__BCF48873_98E5_11D2_9F45_00105AA84069__INCLUDED_)
