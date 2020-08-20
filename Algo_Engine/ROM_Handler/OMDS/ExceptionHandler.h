// ExceptionHandler.h: interface for the ExceptionHandler class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EXCEPTIONHANDLER_H__FE57E40C_0BE0_4885_97AF_1B0780B13482__INCLUDED_)
#define AFX_EXCEPTIONHANDLER_H__FE57E40C_0BE0_4885_97AF_1B0780B13482__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef LONG ( __stdcall *PFNCHFILTFN ) ( EXCEPTION_POINTERS * pExPtrs ) ;

#include <Dbghelp.h>
#include <PSAPI.h>

class ExceptionHandler  
{
public:
	ExceptionHandler();
	virtual ~ExceptionHandler();
	static LONG __stdcall CrashHandlerExceptionFilter( EXCEPTION_POINTERS * pExPtrs );

private:
	LPCTSTR DecodeExceptionRecord( EXCEPTION_RECORD * pE );
	LPCSTR DumpLoadedModules(void);
	LPCTSTR GetFaultReason( EXCEPTION_POINTERS * pExPtrs );
	LPCTSTR ConvertSimpleException ( DWORD dwExcept );
	BOOL InternalSymGetLineFromAddr ( HANDLE hProcess,
									  DWORD  dwAddr, 
									  PDWORD pdwDisplacement ,
									  PIMAGEHLP_LINE  Line );
	LPCTSTR InternalGetStackTraceString( DWORD dwOpts, EXCEPTION_POINTERS * pExPtrs );
	LPCTSTR GetFirstStackTraceString( DWORD dwOpts, EXCEPTION_POINTERS * pExPtrs );
	LPCTSTR GetNextStackTraceString ( DWORD dwOpts, EXCEPTION_POINTERS * pExPtrs );

	void InitSymbolEngine();
	void CleanupSymbolEngine();
	static ExceptionHandler	* m_pThis;
};

#endif // !defined(AFX_EXCEPTIONHANDLER_H__FE57E40C_0BE0_4885_97AF_1B0780B13482__INCLUDED_)
