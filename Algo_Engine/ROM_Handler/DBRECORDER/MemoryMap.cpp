// MemoryMap.cpp: implementation of the CMemoryMap class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemoryMap.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

/*template<class T1>
CMemoryMap<T1>::CMemoryMap()
{
	m_pFileView = NULL;
	m_hFile = NULL;
	m_hFileMap = NULL;
	m_iNumRecords = 0;

	AllocateMemoryMap(2000, false);
}

template<class T1>
CMemoryMap<T1>::~CMemoryMap()
{
	if (m_pFileView)
		UnmapViewOfFile(m_pFileView);

	if (m_hFileMap)
		CloseHandle(m_hFileMap);
}

template<class T1>
void CMemoryMap<T1>::AllocateMemoryMap(int iNumRecords, bool bReallocate)
{
	if(m_pFileView)
		UnmapViewOfFile(m_pFileView);

	if(m_hFileMap)
		CloseHandle(m_hFileMap);

	if(m_hFile)
		CloseHandle(m_hFile);

	m_FreeList.clear();

	BOOL bNewFile;
	int iSize;

	m_hFile = CreateFile(
		m_Path + m_Name + ".file",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,	
		NULL
		);

	bNewFile = !(m_hFile == (HANDLE)0xFFFFFFFF ? TRUE : FALSE);
	
	if(bNewFile) // file already exists
	{
		iSize = SizeOfStruct() * iNumRecords;
	}
	else
	{
		m_hFile = CreateFile(
			m_Path + m_Name + ".file",
			GENERIC_READ | GENERIC_WRITE,
			FILE_SHARE_READ | FILE_SHARE_WRITE,
			NULL,
			OPEN_ALWAYS,
			FILE_ATTRIBUTE_NORMAL,	
			NULL
			);

		if(bReallocate)
			iSize = SizeOfStruct() * iNumRecords;
		else
		{
			iSize = GetFileSize(m_hFile, NULL);
			iNumRecords = iSize / SizeOfStruct();
		}
	}

	m_iNumRecords = iNumRecords;
	
	m_hFileMap = CreateFileMapping(
		(HANDLE)m_hFile,
		NULL,
		PAGE_READWRITE,
		0,
		SizeOfStruct() * iNumRecords,
		m_Name + "_MEM");

	m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0);

	if(m_pFileView)
	{
		m_pMemTags = (struct MemTagStruct*)m_pFileView;

		if(bNewFile)
		{
			for (int i = 0; i < iNumRecords; i++)
			{
				(m_pMemTags + i)->m_bUsed = FALSE;
				m_FreeList.push(m_pMemTags + i);
			}
		}
		else
		{
			for (int i = 0; i < iNumRecords; i++)
			{
				m_pCurrent = &m_pMemTags[i];
				if(m_pCurrent->m_bUsed)
				{
				}
				else
					m_FreeList.push(m_pMemTags + i);
			}
		}
	}
	else
		m_pMemTags = NULL;

	m_pCurrent = m_pMemTags;
}*/