// BloombergFixSession1.h: interface for the CBloombergFixSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BLOOMBERGFIXSESSION1_H__70DE6243_58F2_11D5_A61F_00500401F87A__INCLUDED_)
#define AFX_BLOOMBERGFIXSESSION1_H__70DE6243_58F2_11D5_A61F_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CBloombergFixSession : public CFixSession  
{
public:
	CBloombergFixSession();
	virtual ~CBloombergFixSession();

	long ProcessEntry(COrder& a_Order);
	BOOL ProcessReplace(CReplace& a_Replace);

};

#endif // !defined(AFX_BLOOMBERGFIXSESSION1_H__70DE6243_58F2_11D5_A61F_00500401F87A__INCLUDED_)
