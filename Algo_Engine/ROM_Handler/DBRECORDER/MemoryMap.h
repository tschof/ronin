// MemoryMap.h: interface for the CMemoryMap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMORYMAP_H__EAA1C7F3_2102_11D4_8CBF_005004DA7701__INCLUDED_)
#define AFX_MEMORYMAP_H__EAA1C7F3_2102_11D4_8CBF_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>
#include <map>

template<class T1> class CMemoryMap
{
public:
	CMemoryMap();

	virtual ~CMemoryMap();

	virtual int SizeOfStruct() { return sizeof(T1); }

private:
	void AllocateMemoryMap(int iNumRecords, bool bReallocate);

protected:
	T1 *Current() { return m_pCurrent; }
	void MapTag(int Key, CString &Tag, T1* pMem);
	virtual void MapTags(T1* pMem);
	
private:
	HANDLE m_hFile;
	HANDLE m_hFileMap;
	LPVOID m_pFileView;

	T1 *m_pMemTags;
	T1 *m_pCurrent;

	std::list<T1*> m_FreeList;
	std::list<T1*> m_UsedList;
	int m_iNumRecords;

	std::string m_Path;
	std::string m_Name;

	CMapPtrToPtr m_KeyMap;
};

template<class T1>
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
		CString(m_Path.c_str()) + CString(m_Name.c_str()) + ".file",
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
			CString(m_Path.c_str()) + CString(m_Name.c_str()) + ".file",
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
		CString(m_Name.c_str()) + "_MEM");

	m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0);

	if(m_pFileView)
	{
		m_pMemTags = (T1*)m_pFileView;

		if(bNewFile)
		{
			for (int i = 0; i < iNumRecords; i++)
			{
				(m_pMemTags + i)->SetUsed(FALSE);
				m_FreeList.push_back(m_pMemTags + i);
			}
		}
		else
		{
			for (int i = 0; i < iNumRecords; i++)
			{
				m_pCurrent = &m_pMemTags[i];
				if(m_pCurrent->Used())
				{
					MapTags(m_pCurrent);
				}
				else
					m_FreeList.push_back(m_pMemTags + i);
			}
		}
	}
	else
		m_pMemTags = NULL;

	m_pCurrent = m_pMemTags;
}


template<class T1>
void CMemoryMap<T1>::MapTag(int Key, CString &Tag, T1* pMem)
{
	//m_KeyMap.Lookup(Key, 
}

template<class T1>
void CMemoryMap<T1>::MapTags(T1* pMem)
{
	char buffer[20];
	MapTag(0, CString(_ltoa((long)&pMem, buffer, 10)), pMem);
}

#endif // !defined(AFX_MEMORYMAP_H__EAA1C7F3_2102_11D4_8CBF_005004DA7701__INCLUDED_)
