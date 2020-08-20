// IOMApplication.h: interface for the IOMApplication class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_IOMAPPLICATION_H__F980BE72_14A7_42AF_B3B0_9CC151C14665__INCLUDED_)
#define AFX_IOMAPPLICATION_H__F980BE72_14A7_42AF_B3B0_9CC151C14665__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>

class COmSession;

class IOMApplication  
{
public:
	IOMApplication();
	virtual ~IOMApplication();

	virtual void	OnMessage	( std::string & szMsg ) = 0;
	virtual int		Send		( std::string & szMsg ) = 0;

	void			SetSession	( COmSession * pSession ) { m_pSession = pSession; }
	COmSession *	GetSession	() { return m_pSession; }

protected:
	COmSession * m_pSession;
};

#endif // !defined(AFX_IOMAPPLICATION_H__F980BE72_14A7_42AF_B3B0_9CC151C14665__INCLUDED_)
