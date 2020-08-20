// Runable.h: interface for the CRunable class.
//
//////////////////////////////////////////////////////////////////////
 
#if !defined(AFX_RUNABLE_H__013BD83A_ABB9_4EDF_A788_8F0FBC516670__INCLUDED_)
#define AFX_RUNABLE_H__013BD83A_ABB9_4EDF_A788_8F0FBC516670__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <windows.h>

class CRunable  
{
public:
	virtual void onShutdown(HANDLE thead);
	DWORD getThreadId();
	bool			isCorrectThread();
	virtual void	run();
	bool			isRunning();
	void			shutDown();
	HANDLE			getThread();
	HANDLE			start();

	CRunable();
	virtual ~CRunable();

protected:
	//static unsigned WINAPI Runit(CRunable * pRunable);
	static unsigned WINAPI CRunable::ThreadProc( LPVOID a_pParameter );
	HANDLE		m_hThread;
	DWORD		m_threadId;
	HANDLE		m_hKill;
	HANDLE		m_hKilled;
};

#endif // !defined(AFX_RUNABLE_H__013BD83A_ABB9_4EDF_A788_8F0FBC516670__INCLUDED_)
