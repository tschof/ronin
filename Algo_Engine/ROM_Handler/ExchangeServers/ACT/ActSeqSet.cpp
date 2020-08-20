// ActSequenceSet.cpp: implementation of the CActSequenceSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Act.h"
#include "ActSequenceSet.h"
#include "MessageApp.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActSequenceSet::CActSequenceSet()
{
	m_pFileView = NULL;
	m_hFile = NULL;
	m_hFileMap = NULL;
	m_SequenceSet = NULL;

	AllocateMemoryMap();
}

CActSequenceSet::~CActSequenceSet()
{
	if (m_pFileView)
		UnmapViewOfFile(m_pFileView);

	if (m_hFileMap)
		CloseHandle(m_hFileMap);

	if(m_hFile)
		CloseHandle(m_hFile);
}


void CActSequenceSet::AllocateMemoryMap()
{
	if(m_pFileView)
		UnmapViewOfFile(m_pFileView);

	if(m_hFileMap)
		CloseHandle(m_hFileMap);

	if(m_hFile)
		CloseHandle(m_hFile);

	BOOL bNewFile;
	//CMessageApp* pApp = (CMessageApp*)AfxGetApp();

	m_hFile = CreateFile("C:\\OrderManager\\ActSequence.map",
						 GENERIC_READ | GENERIC_WRITE,
						 FILE_SHARE_READ | FILE_SHARE_WRITE,
						 NULL,
						 CREATE_NEW,
						 FILE_ATTRIBUTE_NORMAL,	
						 NULL
						);

	bNewFile = (m_hFile == INVALID_HANDLE_VALUE) ? FALSE : TRUE;

	if (bNewFile == FALSE)
	{
		m_hFile = CreateFile("C:\\OrderManager\\ActSequence.map",
							 GENERIC_READ | GENERIC_WRITE,
							 FILE_SHARE_READ | FILE_SHARE_WRITE,
							 NULL,
							 OPEN_ALWAYS,
							 FILE_ATTRIBUTE_NORMAL,	
							 NULL
							);
	}

	m_hFileMap = CreateFileMapping(	(HANDLE)m_hFile,
									NULL,
									PAGE_READWRITE,
									0,
									sizeof(SequneceSetStruct),
									"ACT_SEQUENCE"
								  );

	m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0);

	if(m_pFileView)
	{
		m_SequenceSet = (SequneceSetStruct*)m_pFileView;
		CString sTime = CTime::GetCurrentTime().Format("%Y%m%d");

		if(bNewFile)
		{
			SetSequence(0);
			//SetSequence(1);
			SetDate((LPCTSTR)sTime);
		}
		else
		{
			if (strcpy(m_SequenceSet->m_Date, (LPCTSTR)sTime) != 0)
			{
				SetSequence(0);
				//SetSequence(1);
				SetDate((LPCTSTR)sTime);
			}
		}
	}
}


int CActSequenceSet::GetSequence()
{
	//m_SequenceSet->m_NextOut++;
	return m_SequenceSet->m_Sequence;
}

void CActSequenceSet::SetSequence(int Sequence)
{
	m_SequenceSet->m_Sequence = Sequence;
}


void CActSequenceSet::SetDate(const char * pDate)
{
	strcpy(m_SequenceSet->m_Date, pDate);
}
