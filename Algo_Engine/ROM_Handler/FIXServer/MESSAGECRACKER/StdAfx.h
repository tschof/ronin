// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__07F95FE0_499C_4A69_AC2C_D6EC01995BF0__INCLUDED_)
#define AFX_STDAFX_H__07F95FE0_499C_4A69_AC2C_D6EC01995BF0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers
#pragma warning(disable:4786)
#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT
#include <winsock2.h>
#include <afxsock.h>		// MFC socket extensions
#include <list>
#include <vector>
#include <string>
typedef std::vector<std::string>	tMessages;
typedef tMessages::iterator			tMessageIterator;

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__07F95FE0_499C_4A69_AC2C_D6EC01995BF0__INCLUDED_)
