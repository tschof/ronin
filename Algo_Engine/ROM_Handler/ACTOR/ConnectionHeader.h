// ConnectionHeader.h: interface for the CConnectionHeader class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONNECTIONHEADER_H__BCF48876_98E5_11D2_9F45_00105AA84069__INCLUDED_)
#define AFX_CONNECTIONHEADER_H__BCF48876_98E5_11D2_9F45_00105AA84069__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

class CConnectionHeader  
{
public:
	CConnectionHeader();
	virtual ~CConnectionHeader();

	CString m_Name;
	CString m_ConnectionTime;
};

#endif // !defined(AFX_CONNECTIONHEADER_H__BCF48876_98E5_11D2_9F45_00105AA84069__INCLUDED_)
