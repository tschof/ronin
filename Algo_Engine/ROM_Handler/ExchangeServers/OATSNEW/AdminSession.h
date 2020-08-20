// AdminSession.h: interface for the CAdminSession class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ADMINSESSION_H__96E115BD_BC68_473E_B8BA_E79591622A21__INCLUDED_)
#define AFX_ADMINSESSION_H__96E115BD_BC68_473E_B8BA_E79591622A21__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SessionBase.h"
#include <set>

class COatsCallback;

class CAdminSession : public CSessionBase  
{
public:
	typedef std::set<CString> tStrSet;
	typedef tStrSet::iterator tStrSetIterator;
	typedef struct
	{
		bool excluded;
		tStrSet OwnerList;
	} OwnerFilter;

	CAdminSession( COatsCallback * pDoc );
	virtual ~CAdminSession();

	virtual void OnMessage( CString & szMsg );
	virtual void OnClose( int nErrorCode ); 
	virtual int	 Send( const CString & szMsg );

	bool OwnerIncluded(const CString& owner);

private:
	void SetExecludedOwners(CString& owners);

	OwnerFilter m_OwnerFilter;

	int m_Mode;
	bool m_LoggedOn;
	bool m_NewClient;

};

#endif // !defined(AFX_ADMINSESSION_H__96E115BD_BC68_473E_B8BA_E79591622A21__INCLUDED_)
