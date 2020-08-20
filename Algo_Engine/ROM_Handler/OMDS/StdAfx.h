// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__549F7381_E5B0_4D4E_BBE9_FA121EC4254E__INCLUDED_)
#define AFX_STDAFX_H__549F7381_E5B0_4D4E_BBE9_FA121EC4254E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#pragma warning( disable : 4786 )
#pragma warning( disable : 4503 )
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxsock.h>		// MFC socket extensions
#include <afxdb.h>
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
#define GLOBAL_SYNC_LOCK CLock global_sync_lock ("OMDSGlobalSyncLock")
//#define GLOBAL_SYNC_LOCK CLock2 global_sync_lock

#endif // !defined(AFX_STDAFX_H__549F7381_E5B0_4D4E_BBE9_FA121EC4254E__INCLUDED_)
