// ExceptionHandler.cpp: implementation of the ExceptionHandler class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "ExceptionHandler.h"
#include <string>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define GSTSO_PARAMS    0x01
#define GSTSO_MODULE    0x02
#define GSTSO_SYMBOL    0x04
#define GSTSO_SRCLINE   0x08

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#define BUFF_SIZE 2048
static TCHAR g_szBuff [ BUFF_SIZE ] ;

// The static symbol lookup buffer.  This gets casted to make it work.
#define SYM_BUFF_SIZE 512
static BYTE g_stSymbol [ SYM_BUFF_SIZE ] ;

// The static source and line structure.
static IMAGEHLP_LINE g_stLine ;

// The stack frame used in walking the stack.
static STACKFRAME g_stFrame ;

ExceptionHandler * ExceptionHandler::m_pThis = NULL;

ExceptionHandler::ExceptionHandler()
{
	m_pThis = this;
	InitSymbolEngine();
	SetUnhandledExceptionFilter( CrashHandlerExceptionFilter );
}

ExceptionHandler::~ExceptionHandler()
{
	CleanupSymbolEngine();
	m_pThis = NULL;
}

LONG __stdcall ExceptionHandler::CrashHandlerExceptionFilter( EXCEPTION_POINTERS * pExPtrs )
{
	//#define EXCEPTION_EXECUTE_HANDLER       1			// crash dialog will not pop up
	//#define EXCEPTION_CONTINUE_SEARCH       0			// crash dialog will show up
	//#define EXCEPTION_CONTINUE_EXECUTION    -1		// program may hang
    LONG lRet = EXCEPTION_EXECUTE_HANDLER ; 

    __try
    {
		TCHAR szName[256];
		if( !::GetModuleFileName( NULL, szName, sizeof(szName) ) )
			return -1;
		TCHAR * szExt = strchr( szName, '.' );
		if( szExt )
			strcpy( szExt, ".log" );
		FILE * file = fopen( szName, "w" );
		LPCSTR reason = m_pThis->GetFaultReason( pExPtrs );
		fprintf( file, "%s\n", reason );

		fprintf( file, "%s\n", m_pThis->DecodeExceptionRecord( pExPtrs->ExceptionRecord ) );
		//fprintf( file, "%s\n", m_pThis->DecodeContext( pExPtrs->ContextRecord) );
		
		const char * szBuff = m_pThis->GetFirstStackTraceString( GSTSO_PARAMS | GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE, pExPtrs );
		do
		{
			fprintf( file, "%s\n", szBuff );
			szBuff = m_pThis->GetNextStackTraceString ( GSTSO_PARAMS | GSTSO_MODULE | GSTSO_SYMBOL | GSTSO_SRCLINE, pExPtrs ) ;
		}
		while ( NULL != szBuff ) ;

		fprintf( file, "%s\n", m_pThis->DumpLoadedModules() );
	
		
		fclose( file );
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        lRet = EXCEPTION_CONTINUE_SEARCH ;
    }
    return lRet;
}

void ExceptionHandler::InitSymbolEngine()
{
	HANDLE hProcess = GetCurrentProcess();
	if( !::SymInitialize ( hProcess, NULL, FALSE ) )
		return;
	TCHAR szName[256];
	if( !::GetModuleFileName( NULL, szName, sizeof(szName) ) )
		return;
	HANDLE hFile = ::CreateFile (	szName,
									GENERIC_READ,
									FILE_SHARE_READ,
									NULL,
									OPEN_EXISTING,
									FILE_ATTRIBUTE_NORMAL,
									NULL   );
	if ( INVALID_HANDLE_VALUE == hFile )
	{
		return;
	}
	DWORD dwOpts = SymGetOptions () ;
	SymSetOptions ( dwOpts | SYMOPT_LOAD_LINES | SYMOPT_DEFERRED_LOADS	) ;
	DWORD dwBase = SymLoadModule ( hProcess, hFile, szName, NULL, 0, 0);
	if( dwBase == 0 )
		return;
}

void ExceptionHandler::CleanupSymbolEngine()
{
	::SymCleanup( GetCurrentProcess() );
}

LPCTSTR ExceptionHandler::ConvertSimpleException ( DWORD dwExcept )
{
    switch ( dwExcept )
    {
        case EXCEPTION_ACCESS_VIOLATION         :
            return ( _T ( "EXCEPTION_ACCESS_VIOLATION" ) ) ;
        break ;

        case EXCEPTION_DATATYPE_MISALIGNMENT    :
            return ( _T ( "EXCEPTION_DATATYPE_MISALIGNMENT" ) ) ;
        break ;

        case EXCEPTION_BREAKPOINT               :
            return ( _T ( "EXCEPTION_BREAKPOINT" ) ) ;
        break ;

        case EXCEPTION_SINGLE_STEP              :
            return ( _T ( "EXCEPTION_SINGLE_STEP" ) ) ;
        break ;

        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED    :
            return ( _T ( "EXCEPTION_ARRAY_BOUNDS_EXCEEDED" ) ) ;
        break ;

        case EXCEPTION_FLT_DENORMAL_OPERAND     :
            return ( _T ( "EXCEPTION_FLT_DENORMAL_OPERAND" ) ) ;
        break ;

        case EXCEPTION_FLT_DIVIDE_BY_ZERO       :
            return ( _T ( "EXCEPTION_FLT_DIVIDE_BY_ZERO" ) ) ;
        break ;

        case EXCEPTION_FLT_INEXACT_RESULT       :
            return ( _T ( "EXCEPTION_FLT_INEXACT_RESULT" ) ) ;
        break ;

        case EXCEPTION_FLT_INVALID_OPERATION    :
            return ( _T ( "EXCEPTION_FLT_INVALID_OPERATION" ) ) ;
        break ;

        case EXCEPTION_FLT_OVERFLOW             :
            return ( _T ( "EXCEPTION_FLT_OVERFLOW" ) ) ;
        break ;

        case EXCEPTION_FLT_STACK_CHECK          :
            return ( _T ( "EXCEPTION_FLT_STACK_CHECK" ) ) ;
        break ;

        case EXCEPTION_FLT_UNDERFLOW            :
            return ( _T ( "EXCEPTION_FLT_UNDERFLOW" ) ) ;
        break ;

        case EXCEPTION_INT_DIVIDE_BY_ZERO       :
            return ( _T ( "EXCEPTION_INT_DIVIDE_BY_ZERO" ) ) ;
        break ;

        case EXCEPTION_INT_OVERFLOW             :
            return ( _T ( "EXCEPTION_INT_OVERFLOW" ) ) ;
        break ;

        case EXCEPTION_PRIV_INSTRUCTION         :
            return ( _T ( "EXCEPTION_PRIV_INSTRUCTION" ) ) ;
        break ;

        case EXCEPTION_IN_PAGE_ERROR            :
            return ( _T ( "EXCEPTION_IN_PAGE_ERROR" ) ) ;
        break ;

        case EXCEPTION_ILLEGAL_INSTRUCTION      :
            return ( _T ( "EXCEPTION_ILLEGAL_INSTRUCTION" ) ) ;
        break ;

        case EXCEPTION_NONCONTINUABLE_EXCEPTION :
            return ( _T ( "EXCEPTION_NONCONTINUABLE_EXCEPTION" ) ) ;
        break ;

        case EXCEPTION_STACK_OVERFLOW           :
            return ( _T ( "EXCEPTION_STACK_OVERFLOW" ) ) ;
        break ;

        case EXCEPTION_INVALID_DISPOSITION      :
            return ( _T ( "EXCEPTION_INVALID_DISPOSITION" ) ) ;
        break ;

        case EXCEPTION_GUARD_PAGE               :
            return ( _T ( "EXCEPTION_GUARD_PAGE" ) ) ;
        break ;

        case EXCEPTION_INVALID_HANDLE           :
            return ( _T ( "EXCEPTION_INVALID_HANDLE" ) ) ;
        break ;

        default :
            return ( NULL ) ;
        break ;
    }
}

LPCTSTR ExceptionHandler::GetFaultReason( EXCEPTION_POINTERS * pExPtrs )
{
    if( TRUE == IsBadReadPtr ( pExPtrs, sizeof ( EXCEPTION_POINTERS ) ) )
    {
        TRACE0 ( "Bad parameter to GetFaultReasonA\n" ) ;
        return ( NULL ) ;
    }

    // The value that holds the return.
    LPCTSTR szRet ;

    __try
    {

        // The current position in the buffer.
        int iCurr = 0 ;
        // A temp value holder.  This is to keep the stack usage to a
        //  minimum.
        DWORD dwTemp ;

        iCurr += GetModuleBaseName ( GetCurrentProcess(), NULL, g_szBuff, BUFF_SIZE );
        iCurr += wsprintf( g_szBuff + iCurr , _T ( " caused a " ) ) ;
        dwTemp = (DWORD)ConvertSimpleException(pExPtrs->ExceptionRecord->ExceptionCode);
        if ( NULL != dwTemp )
        {
            iCurr += wsprintf ( g_szBuff+iCurr, _T ( "%s" ), dwTemp);
        }
        else
        {
            iCurr += (FormatMessage( FORMAT_MESSAGE_IGNORE_INSERTS |
                                            FORMAT_MESSAGE_FROM_HMODULE,
                                     GetModuleHandle (_T("NTDLL.DLL")) ,
                                     pExPtrs->ExceptionRecord->
                                                          ExceptionCode,
                                     0                                 ,
                                     g_szBuff + iCurr                  ,
                                     BUFF_SIZE ,
                                     0                                 )
                      * sizeof ( TCHAR ) ) ;
        }

        ASSERT ( iCurr < BUFF_SIZE ) ;

        iCurr += wsprintf ( g_szBuff + iCurr , _T ( " in module " ) ) ;

        dwTemp = SymGetModuleBase ( GetCurrentProcess(), (DWORD)pExPtrs->ExceptionRecord->ExceptionAddress );

        ASSERT ( NULL != dwTemp ) ;

        if ( NULL == dwTemp )
        {
            iCurr += wsprintf ( g_szBuff + iCurr , _T ( "<UNKNOWN>" ) );
        }
        else
        {
            iCurr += GetModuleBaseName( GetCurrentProcess(), 
											(HINSTANCE)dwTemp, 
											g_szBuff+iCurr, 
											BUFF_SIZE-iCurr );
        }

        iCurr += wsprintf ( g_szBuff + iCurr,
                            _T ( " at %04X:%08X" ),
                            pExPtrs->ContextRecord->SegCs,
                            pExPtrs->ExceptionRecord->ExceptionAddress);

        ASSERT ( iCurr < BUFF_SIZE ) ;

        // Start looking up the exception address.
        PIMAGEHLP_SYMBOL pSym = (PIMAGEHLP_SYMBOL)&g_stSymbol ;
        FillMemory ( pSym , NULL , SYM_BUFF_SIZE ) ;
        pSym->SizeOfStruct = sizeof ( IMAGEHLP_SYMBOL ) ;
        pSym->MaxNameLength = SYM_BUFF_SIZE - sizeof ( IMAGEHLP_SYMBOL);

        DWORD dwDisp ;
        if ( TRUE ==
              SymGetSymFromAddr ( GetCurrentProcess ( )               ,
                                  (DWORD)pExPtrs->ExceptionRecord->
                                                     ExceptionAddress ,
                                  &dwDisp                             ,
                                  pSym                                ))
        {
            iCurr += wsprintf ( g_szBuff + iCurr , _T ( ", " ) ) ;

            // Copy no more than there is room for.
            dwTemp = lstrlen ( pSym->Name ) ;
            if ( (int)dwTemp > ( BUFF_SIZE - iCurr - 20 ) )
            {
                lstrcpyn ( g_szBuff + iCurr      ,
                           pSym->Name            ,
                           BUFF_SIZE - iCurr - 1  ) ;
                // Gotta leave now.
                szRet = g_szBuff ;
                __leave ;
            }
            else
            {
                if ( dwDisp > 0 )
                {
                    iCurr += wsprintf ( g_szBuff + iCurr         ,
                                        _T ( "%s()+%d byte(s)" ) ,
                                        pSym->Name               ,
                                        dwDisp                    ) ;
                }
                else
                {
                    iCurr += wsprintf ( g_szBuff + iCurr ,
                                        _T ( "%s " )     ,
                                        pSym->Name        ) ;
                }
            }
        }
        else
        {
            // If the symbol was not found, the source and line will not
            //  be found either so leave now.
            szRet = g_szBuff ;
            __leave ;
        }

        ASSERT ( iCurr < BUFF_SIZE ) ;

        // Do the source and line lookup.
        ZeroMemory ( &g_stLine , sizeof ( IMAGEHLP_LINE ) ) ;
        g_stLine.SizeOfStruct = sizeof ( IMAGEHLP_LINE ) ;

        if ( TRUE ==
              InternalSymGetLineFromAddr ( GetCurrentProcess ( )       ,
                                          (DWORD)pExPtrs->
                                                    ExceptionRecord->
                                                      ExceptionAddress ,
                                          &dwDisp                      ,
                                          &g_stLine                   ))
        {
            iCurr += wsprintf ( g_szBuff + iCurr , _T ( ", " ) ) ;

            // Copy no more than there is room for.
            dwTemp = lstrlen ( g_stLine.FileName ) ;
            if ( (int)dwTemp > ( BUFF_SIZE - iCurr - 25 ) )
            {
                lstrcpyn ( g_szBuff + iCurr      ,
                           g_stLine.FileName     ,
                           BUFF_SIZE - iCurr - 1  ) ;
                // Gotta leave now.
                szRet = g_szBuff ;
                __leave ;
            }
            else
            {
                if ( dwDisp > 0 )
                {
                    iCurr += wsprintf ( g_szBuff + iCurr              ,
                                        _T ( "%s, line %d+%d byte(s)"),
                                        g_stLine.FileName             ,
                                        g_stLine.LineNumber           ,
                                        dwDisp                        );
                }
                else
                {
                    iCurr += wsprintf ( g_szBuff + iCurr     ,
                                        _T ( "%s, line %d" ) ,
                                        g_stLine.FileName    ,
                                        g_stLine.LineNumber   ) ;
                }
            }
        }
        szRet = g_szBuff ;
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        ASSERT ( FALSE ) ;
        szRet = NULL ;
    }
    return ( szRet ) ;
}

BOOL ExceptionHandler::InternalSymGetLineFromAddr ( HANDLE hProcess,
                                  DWORD  dwAddr, 
								  PDWORD pdwDisplacement ,
                                  PIMAGEHLP_LINE  Line )
{
#ifdef WORK_AROUND_SRCLINE_BUG

        // The problem is that the symbol engine only finds those source
        //  line addresses (after the first lookup) that fall exactly on
        //  a zero displacement.  I will walk backwards 100 bytes to
        //  find the line and return the proper displacement.
        DWORD dwTempDis = 0 ;
        while ( FALSE == ::SymGetLineFromAddr( hProcess, dwAddr-dwTempDis,
                                                   pdwDisplacement ,
                                                   Line ) )
        {
            dwTempDis += 1 ;
            if ( 100 == dwTempDis )
            {
                return ( FALSE ) ;
            }
        }

        // It was found and the source line information is correct so
        //  change the displacement if it was looked up multiple times.
        if ( 0 != dwTempDis )
        {
            *pdwDisplacement = dwTempDis ;
        }
        return ( TRUE ) ;

#else  // WORK_AROUND_SRCLINE_BUG
        return ( ::SymGetLineFromAddr ( hProcess         ,
                                           dwAddr           ,
                                           pdwDisplacement  ,
                                           Line              ) ) ;
#endif
    return ( FALSE ) ;
}

LPCTSTR ExceptionHandler::GetFirstStackTraceString(DWORD dwOpts, EXCEPTION_POINTERS * pExPtrs)
{
    // All of the error checking is in the InternalGetStackTraceString
    //  function.

    // Initialize the STACKFRAME structure.
    ZeroMemory ( &g_stFrame , sizeof ( STACKFRAME ) ) ;

    #ifdef _X86_
    g_stFrame.AddrPC.Offset       = pExPtrs->ContextRecord->Eip ;
    g_stFrame.AddrPC.Mode         = AddrModeFlat                ;
    g_stFrame.AddrStack.Offset    = pExPtrs->ContextRecord->Esp ;
    g_stFrame.AddrStack.Mode      = AddrModeFlat                ;
    g_stFrame.AddrFrame.Offset    = pExPtrs->ContextRecord->Ebp ;
    g_stFrame.AddrFrame.Mode      = AddrModeFlat                ;
    #else
    g_stFrame.AddrPC.Offset       = (DWORD)pExPtrs->ContextRecord->Fir ;
    g_stFrame.AddrPC.Mode         = AddrModeFlat ;
    g_stFrame.AddrReturn.Offset   =
                                   (DWORD)pExPtrs->ContextRecord->IntRa;
    g_stFrame.AddrReturn.Mode     = AddrModeFlat ;
    g_stFrame.AddrStack.Offset    =
                                   (DWORD)pExPtrs->ContextRecord->IntSp;
    g_stFrame.AddrStack.Mode      = AddrModeFlat ;
    g_stFrame.AddrFrame.Offset    =
                                   (DWORD)pExPtrs->ContextRecord->IntFp;
    g_stFrame.AddrFrame.Mode      = AddrModeFlat ;
    #endif

    return ( InternalGetStackTraceString ( dwOpts , pExPtrs ) ) ;
}

LPCTSTR ExceptionHandler::GetNextStackTraceString ( DWORD dwOpts,
                                       EXCEPTION_POINTERS * pExPtrs  )
{
    // All error checking is in InternalGetStackTraceString.
    // Assume that GetFirstStackTraceString has already initialized the
    //  stack frame information.
    return ( InternalGetStackTraceString ( dwOpts , pExPtrs ) ) ;
}


// The internal function that does all the stack walking.
LPCTSTR ExceptionHandler::InternalGetStackTraceString( DWORD dwOpts, EXCEPTION_POINTERS * pExPtrs )
{

    ASSERT ( FALSE == IsBadReadPtr ( pExPtrs, sizeof (EXCEPTION_POINTERS )));
    if ( TRUE == IsBadReadPtr ( pExPtrs, sizeof ( EXCEPTION_POINTERS ) ) )
    {
        TRACE0 ( "GetStackTraceString - invalid pExPtrs!\n" ) ;
        return ( NULL ) ;
    }

    // The value that is returned.
    LPCTSTR szRet ;
    // A temporary for all to use.  This saves stack space.
    DWORD dwTemp ;

    __try
    {
        // Note:  If the source and line functions are used, then
        //        StackWalk can access violate.
        BOOL bSWRet = ::StackWalk ( IMAGE_FILE_MACHINE_I386                ,
                                  GetCurrentProcess ( )     ,
                                  GetCurrentThread ( )      ,
                                  &g_stFrame                ,
                                  pExPtrs->ContextRecord    ,
                                  NULL                      ,
                                  SymFunctionTableAccess    ,
                                  SymGetModuleBase          ,
                                  NULL                       ) ;
        if ( ( FALSE == bSWRet ) || ( 0 == g_stFrame.AddrFrame.Offset ))
        {
            szRet = NULL ;
            __leave ;
        }

        int iCurr = 0 ;

        // At a minimum, put the address in.
        iCurr += wsprintf ( g_szBuff + iCurr              ,
                            _T ( "%04X:%08X" )            ,
                            pExPtrs->ContextRecord->SegCs ,
                            g_stFrame.AddrPC.Offset        ) ;

        // Do the parameters?
        if ( GSTSO_PARAMS == ( dwOpts & GSTSO_PARAMS ) )
        {
            iCurr += wsprintf ( g_szBuff + iCurr          ,
                                _T ( " (0x%08X 0x%08X "\
                                      "0x%08X 0x%08X)"  ) ,
                                g_stFrame.Params[ 0 ]     ,
                                g_stFrame.Params[ 1 ]     ,
                                g_stFrame.Params[ 2 ]     ,
                                g_stFrame.Params[ 3 ]      ) ;
        }

        if ( GSTSO_MODULE == ( dwOpts & GSTSO_MODULE ) )
        {
            iCurr += wsprintf ( g_szBuff + iCurr  , _T ( " " ) ) ;

            dwTemp = SymGetModuleBase ( GetCurrentProcess ( )   ,
                                        g_stFrame.AddrPC.Offset  ) ;

            ASSERT ( NULL != dwTemp ) ;

            if ( NULL == dwTemp )
            {
                iCurr += wsprintf ( g_szBuff + iCurr  ,
                                    _T ( "<UNKNOWN>" ) ) ;
            }
            else
            {
                iCurr += GetModuleBaseName ( GetCurrentProcess ( ) ,
                                                (HINSTANCE)dwTemp     ,
                                                g_szBuff + iCurr      ,
                                                BUFF_SIZE - iCurr     );
            }
        }

        ASSERT ( iCurr < BUFF_SIZE ) ;
        DWORD dwDisp ;

        if ( GSTSO_SYMBOL == ( dwOpts & GSTSO_SYMBOL ) )
        {

            // Start looking up the exception address.
            //lint -e545
            PIMAGEHLP_SYMBOL pSym = (PIMAGEHLP_SYMBOL)&g_stSymbol ;
            //lint +e545
            ZeroMemory ( pSym , SYM_BUFF_SIZE ) ;
            pSym->SizeOfStruct = sizeof ( IMAGEHLP_SYMBOL ) ;
            pSym->MaxNameLength = SYM_BUFF_SIZE -
                                  sizeof ( IMAGEHLP_SYMBOL ) ;

            if ( TRUE ==
                  SymGetSymFromAddr ( GetCurrentProcess ( )   ,
                                      g_stFrame.AddrPC.Offset ,
                                      &dwDisp                 ,
                                      pSym                     ) )
            {
                iCurr += wsprintf ( g_szBuff + iCurr , _T ( ", " ) ) ;

                // Copy no more than there is room for.
                dwTemp = lstrlen ( pSym->Name ) ;
                if ( dwTemp > (DWORD)( BUFF_SIZE - iCurr - 20 ) )
                {
                    lstrcpyn ( g_szBuff + iCurr      ,
                               pSym->Name            ,
                               BUFF_SIZE - iCurr - 1  ) ;
                    // Gotta leave now.
                    szRet = g_szBuff ;
                    __leave ;
                }
                else
                {
                    if ( dwDisp > 0 )
                    {
                        iCurr += wsprintf ( g_szBuff + iCurr         ,
                                            _T ( "%s()+%d byte(s)" ) ,
                                            pSym->Name               ,
                                            dwDisp                    );
                    }
                    else
                    {
                        iCurr += wsprintf ( g_szBuff + iCurr ,
                                            _T ( "%s" )      ,
                                            pSym->Name        ) ;
                    }
                }
            }
            else
            {
                // If the symbol was not found, the source and line will
                //  not be found either so leave now.
                szRet = g_szBuff ;
                __leave ;
            }

        }

        if ( GSTSO_SRCLINE == ( dwOpts & GSTSO_SRCLINE ) )
        {
            ZeroMemory ( &g_stLine , sizeof ( IMAGEHLP_LINE ) ) ;
            g_stLine.SizeOfStruct = sizeof ( IMAGEHLP_LINE ) ;

            if ( TRUE ==
                   InternalSymGetLineFromAddr ( GetCurrentProcess ( )  ,
                                                g_stFrame.AddrPC.Offset,
                                                &dwDisp                ,
                                                &g_stLine             ))
            {
                iCurr += wsprintf ( g_szBuff + iCurr , _T ( ", " ) ) ;

                // Copy no more than there is room for.
                dwTemp = lstrlen ( g_stLine.FileName ) ;
                if ( dwTemp > (DWORD)( BUFF_SIZE - iCurr - 25 ) )
                {
                    lstrcpyn ( g_szBuff + iCurr      ,
                               g_stLine.FileName     ,
                               BUFF_SIZE - iCurr - 1  ) ;
                    // Gotta leave now.
                    szRet = g_szBuff ;
                    __leave ;
                }
                else
                {
                    if ( dwDisp > 0 )
                    {
                        iCurr += wsprintf(g_szBuff + iCurr             ,
                                          _T ("%s, line %d+%d byte(s)"),
                                          g_stLine.FileName            ,
                                          g_stLine.LineNumber          ,
                                          dwDisp                      );
                    }
                    else
                    {
                        iCurr += wsprintf ( g_szBuff + iCurr     ,
                                            _T ( "%s, line %d" ) ,
                                            g_stLine.FileName    ,
                                            g_stLine.LineNumber   ) ;
                    }
                }
            }
        }

        szRet = g_szBuff ;
    }
    __except ( EXCEPTION_EXECUTE_HANDLER )
    {
        ASSERT ( FALSE ) ;
        szRet = NULL ;
    }
    return ( szRet ) ;
}

LPCTSTR ExceptionHandler::DecodeExceptionRecord( EXCEPTION_RECORD * pE )
{
	CString sRC = "";

	CString sException;

	switch(pE->ExceptionCode)
	{
	case EXCEPTION_ACCESS_VIOLATION:
	  sException = "EXCEPTION_ACCESS_VIOLATION";
	  break;
	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
	  sException = "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
	  break;
	case EXCEPTION_BREAKPOINT:
	  sException = "EXCEPTION_BREAKPOINT";
	  break;
	case EXCEPTION_DATATYPE_MISALIGNMENT:
	  sException = "EXCEPTION_DATATYPE_MISALIGNMENT";
	  break;
	case EXCEPTION_FLT_DENORMAL_OPERAND:
	  sException = "EXCEPTION_FLT_DENORMAL_OPERAND";
	  break;
	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	  sException = "EXCEPTION_FLT_DIVIDE_BY_ZERO";
	  break;
	case EXCEPTION_FLT_INEXACT_RESULT:	
	  sException = "EXCEPTION_FLT_INEXACT_RESULT";
	  break;
	case EXCEPTION_FLT_INVALID_OPERATION:
	  sException = "EXCEPTION_FLT_INVALID_OPERATION";
	  break;
	case EXCEPTION_FLT_OVERFLOW:
	  sException = "EXCEPTION_FLT_OVERFLOW";
	  break;
	case EXCEPTION_FLT_STACK_CHECK:	
	  sException = "EXCEPTION_FLT_STACK_CHECK";
	  break;
	case EXCEPTION_FLT_UNDERFLOW:
	  sException = "EXCEPTION_FLT_UNDERFLOW";
	  break;
	case EXCEPTION_ILLEGAL_INSTRUCTION:
	  sException = "EXCEPTION_ILLEGAL_INSTRUCTION";
	  break;
	case EXCEPTION_IN_PAGE_ERROR:
	  sException = "EXCEPTION_IN_PAGE_ERROR";
	  break;
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
	  sException = "EXCEPTION_INT_DIVIDE_BY_ZERO";
	  break;
	case EXCEPTION_INT_OVERFLOW:
	  sException = "EXCEPTION_INT_OVERFLOW";
	  break;
	case EXCEPTION_INVALID_DISPOSITION:
	  sException = "EXCEPTION_INVALID_DISPOSITION";
	  break;
	case EXCEPTION_NONCONTINUABLE_EXCEPTION:
	  sException = "EXCEPTION_NONCONTINUABLE_EXCEPTION";
	  break;
	case EXCEPTION_PRIV_INSTRUCTION:
	  sException = "EXCEPTION_PRIV_INSTRUCTION";
	  break;
	case EXCEPTION_SINGLE_STEP:
	  sException = "EXCEPTION_SINGLE_STEP";
	  break;
	case EXCEPTION_STACK_OVERFLOW:
	  sException = "EXCEPTION_STACK_OVERFLOW";
	  break;
	case DBG_CONTROL_C:
	  sException = "DBG_CONTROL_C";
	  break;
	default:	
	  sException = "Unknown Exception";
	  break;
	}

	CString sNestedException;

	if (pE->ExceptionRecord != NULL)
	{
	   sNestedException = "\r\n... " + CString(DecodeExceptionRecord(pE->ExceptionRecord));
	}

	CString sAddress;
	sAddress.Format("0x%08lx", pE->ExceptionAddress);

	sRC = sException + " at " + sAddress + sNestedException;

	return (LPCTSTR)sRC;
}

LPCSTR ExceptionHandler::DumpLoadedModules(void)
{
	unsigned char *p = NULL;
	MEMORY_BASIC_INFORMATION info;
	CString sModules("Loaded Modules: \r\n");    // space for retrieved mod names.
	char mod_name[MAX_PATH];

	for(p = NULL; VirtualQuery(p, &info, sizeof(info)) == sizeof(info);
		p += info.RegionSize) 
	{
	   if ((info.AllocationBase == info.BaseAddress) && 
		   (GetModuleFileName((HINSTANCE)info.AllocationBase, mod_name, sizeof(mod_name))) ) 
	   {
		 CString sModule;
		 sModule.Format("%s : 0x%08lx\r\n", mod_name, info.AllocationBase);
		 sModules = sModules + sModule;
	   }
	}

	return sModules;
}