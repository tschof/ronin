// CrackerDoc.cpp : implementation of the CCrackerDoc class
//

#include "stdafx.h"
#include "OMMessageCracker.h"
#include "resource.h"
#include "CrackerDoc.h"
#include "CrackerView.h"

#include <sstream>
#include <iomanip>
#include <fstream>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CCrackerDoc

IMPLEMENT_DYNCREATE(CCrackerDoc, CDocument)

BEGIN_MESSAGE_MAP(CCrackerDoc, CDocument)
	//{{AFX_MSG_MAP(CCrackerDoc)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CCrackerDoc construction/destruction

CCrackerDoc::CCrackerDoc()
{
	// TODO: add one-time construction code here
	m_pApp = (CCrackerApp *)AfxGetApp();
}

CCrackerDoc::~CCrackerDoc()
{
}

BOOL CCrackerDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: add reinitialization code here
	// (SDI documents will reuse this document)
	return TRUE;
}



/////////////////////////////////////////////////////////////////////////////
// CCrackerDoc serialization

void CCrackerDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: add storing code here
	}
	else
	{
		// TODO: add loading code here
	}
}

/////////////////////////////////////////////////////////////////////////////
// CCrackerDoc diagnostics

#ifdef _DEBUG
void CCrackerDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CCrackerDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CCrackerDoc commands

BOOL CCrackerDoc::OnOpenDocument(LPCTSTR lpszPathName) 
{
//	if (!CDocument::OnOpenDocument(lpszPathName))
//		return FALSE;
	
	// TODO: Add your specialized creation code here
	m_szPathName = lpszPathName;
	LoadLogFile();
	return TRUE;
}

void CCrackerDoc::LoadLogFile()
{
	try
	{
		if( m_szPathName.IsEmpty() )
			return;
		m_Messages.clear();
		POSITION pos = GetFirstViewPosition();
		CCrackerView * pView = (CCrackerView *)GetNextView( pos );
		pView->Reset();
		WIN32_FILE_ATTRIBUTE_DATA info;
		GetFileAttributesEx( m_szPathName,  GetFileExInfoStandard, &info ); 
		DWORD dwFileSize = info.nFileSizeLow;
		int nBufferSize = min( 4096, dwFileSize/40 );
		char * pBuffer = new char[nBufferSize+1];
		DWORD dwRead = 0;
		std::ifstream fin(m_szPathName);
		int nMaxLen = 0;
		std::string szMsg = "";
		MSG msg; 
		while (true) 
		{ 
			// Remove any messages that may be in the queue. If the 
			// queue contains any mouse or keyboard 
			// messages, end the operation. 
			ZeroMemory( pBuffer, nBufferSize+1 );
			int size = 0;
			fin.read( pBuffer, nBufferSize );
			int nread = fin.gcount();
			dwRead += nread;
			float fPercent = 0.0f;
			if( dwRead >= dwFileSize )
				fPercent = 100.0f;
			else
				fPercent = dwRead*100.0/dwFileSize;
			szMsg += pBuffer;
			int nIndex = -1;
			while( ( nIndex = szMsg.find("\n") ) != -1 )
			{
				m_Messages.push_back( szMsg.substr( 0, nIndex ) );
				szMsg = szMsg.substr( nIndex+1, -1 );
			}
			UpdateAllViews( NULL, (LPARAM)fPercent, NULL );
			if( nread != nBufferSize )
			{
				UpdateAllViews( NULL, 100, NULL );
				break;
			}

			while(PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) 
			{ 
				DispatchMessage( &msg );
			} 
		} 
		delete []pBuffer;
		pView->AddLogMsg();
	}
	catch(...)
	{
		AfxMessageBox("Something is wrong!");
	}
}