// MemTagSet.cpp: implementation of the CMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MemTagSet.h"
#include "MessageApp.h"
#include "Order.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CMemTagSet::CMemTagSet()
{
	m_pFileView = NULL;
	m_hFile = NULL;
	m_hFileMap = NULL;
	m_iNumRecords = 0;

	AllocateMemoryMap(2000, false);
}

CMemTagSet::~CMemTagSet()
{
	if (m_pFileView)
		UnmapViewOfFile(m_pFileView);

	if (m_hFileMap)
		CloseHandle(m_hFileMap);
}

void CMemTagSet::AllocateMemoryMap(int iNumRecords, bool bReallocate)
{
	if(m_pFileView)
		UnmapViewOfFile(m_pFileView);

	if(m_hFileMap)
		CloseHandle(m_hFileMap);

	if(m_hFile)
		CloseHandle(m_hFile);

	m_FreeList.RemoveAll();

	m_CancelTagMap.RemoveAll();
	m_ExchangeTagMap.RemoveAll();
	m_OrderTagMap.RemoveAll();
	m_StaffordTagMap.RemoveAll();
	m_SequenceTagMap.RemoveAll();

	CMessageApp* pApp = (CMessageApp*)AfxGetApp();
	BOOL bNewFile;
	int iSize;

	m_hFile = CreateFile(
		"C:\\OrderManager\\" + pApp->Name() + "Tag.map",
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
			"C:\\OrderManager\\" + pApp->Name() + "Tag.map",
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
		pApp->Name() + "_MEM");

	m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0);

	if(m_pFileView)
	{
		m_pMemTags = (struct MemTagStruct*)m_pFileView;

		if(bNewFile)
		{
			for (int i = 0; i < iNumRecords; i++)
			{
				(m_pMemTags + i)->m_bUsed = FALSE;
				m_FreeList.AddTail(m_pMemTags + i);
			}
		}
		else
		{
			for (int i = 0; i < iNumRecords; i++)
			{
				m_pCurrent = &m_pMemTags[i];
				if(m_pCurrent->m_bUsed)
				{
					if(m_pCurrent->m_StaffordTag != NULL)
						m_StaffordTagMap.SetAt((m_pMemTags + i)->m_StaffordTag, (void*)(m_pMemTags + i));
					if(m_pCurrent->m_ExchangeTag != NULL)
						m_ExchangeTagMap.SetAt(m_pMemTags[i].m_ExchangeTag, (void*)(m_pMemTags + i));
					if(m_pCurrent->m_CancelTag != NULL)
						m_CancelTagMap.SetAt(m_pMemTags[i].m_CancelTag, (void*)(m_pMemTags + i));
					if(m_pCurrent->m_OrderTag != NULL)
						m_OrderTagMap.SetAt(m_pMemTags[i].m_OrderTag, (void*)(m_pMemTags + i));
					if(m_pCurrent->m_SequenceTag != NULL)
						m_SequenceTagMap.SetAt((void*)m_pMemTags[i].m_SequenceTag, (void*)(m_pMemTags + i));
				}
				else
					m_FreeList.AddTail(m_pMemTags + i);
			}
		}
	}
	else
		m_pMemTags = NULL;

	m_pCurrent = m_pMemTags;
}

BOOL CMemTagSet::UpdateTags(const CString& StaffordTag, const CString& ExchangeTag, const CString& CancelTag, const CString& OrderTag, const long SeqNum, const CString& Client)
{
	if (m_pMemTags)
	{
		switch (m_QueryType)
		{
		case MAP_NEW_MAP:
			if(!m_FreeList.GetCount())
				 AllocateMemoryMap(m_iNumRecords * 2, true);
			m_pCurrent = (MemTagStruct*)m_FreeList.RemoveHead();
			m_pCurrent->m_bUsed = TRUE;
			if(StaffordTag.GetLength())
				m_StaffordTagMap.SetAt(StaffordTag, (void*)m_pCurrent);
			if(ExchangeTag.GetLength())
				m_ExchangeTagMap.SetAt(ExchangeTag, (void*)m_pCurrent);
			if(CancelTag.GetLength())
				m_CancelTagMap.SetAt(CancelTag, (void*)m_pCurrent);
			if(OrderTag.GetLength())
				m_OrderTagMap.SetAt(OrderTag, (void*)m_pCurrent);
			if(SeqNum)
				m_SequenceTagMap.SetAt((void*)SeqNum, m_pCurrent);
			break;
		case MAP_STAFFORD_TAG_QUERY:
			m_StaffordTagMap.Lookup(StaffordTag, (void*&)m_pCurrent);
			break;
		case MAP_EXCHANGE_TAG_QUERY:
			m_ExchangeTagMap.Lookup(ExchangeTag, (void*&)m_pCurrent);
			break;
		case MAP_CANCEL_TAG_QUERY:
			m_CancelTagMap.Lookup(CancelTag, (void*&)m_pCurrent);
			break;
		case MAP_ORDER_TAG_QUERY:
			m_OrderTagMap.Lookup(OrderTag, (void*&)m_pCurrent);
			break;
		case MAP_SEQUENCE_TAG_QUERY:
			m_SequenceTagMap.Lookup((void*)SeqNum, (void*&)m_pCurrent);
			break;
		default:
			m_pCurrent = NULL;
		}	

		if(m_pCurrent)
		{
			if (StaffordTag != "")	
				strcpy(m_pCurrent->m_StaffordTag, ((CString&)StaffordTag).GetBuffer(0));
			if (CancelTag != "")		
				strcpy(m_pCurrent->m_CancelTag, ((CString&)CancelTag).GetBuffer(0));
			if (OrderTag != "")		
				strcpy(m_pCurrent->m_OrderTag, ((CString&)OrderTag).GetBuffer(0));
			if (ExchangeTag != "")	
				strcpy(m_pCurrent->m_ExchangeTag, ((CString&)ExchangeTag).GetBuffer(0));
			if (Client != "")
				strcpy(m_pCurrent->m_Client, ((CString&)Client).GetBuffer(0));
			if (SeqNum > 0)		
				m_pCurrent->m_SequenceTag = SeqNum;

			return TRUE;
		}
	}

	return FALSE;
}

BOOL CMemTagSet::UpdateTags(int QueryType, const CString& QueryTag, const CString& StaffordTag, const CString& ExchangeTag, const CString& CancelTag, const CString& OrderTag, const long SeqNum, const CString& Client)
{
	m_QueryType = QueryType;

	return(UpdateTags(StaffordTag, ExchangeTag, CancelTag, OrderTag, SeqNum, Client));
}

BOOL CMemTagSet::GetTags(int QueryType, const CString& QueryTag)
{
	m_QueryType = QueryType;
	BOOL bFound = FALSE;
	
	switch (m_QueryType)
	{
		case MAP_STAFFORD_TAG_QUERY:
			bFound = m_StaffordTagMap.Lookup(QueryTag, (void*&)m_pCurrent);
			break;
		case MAP_EXCHANGE_TAG_QUERY:
			bFound = m_ExchangeTagMap.Lookup(QueryTag, (void*&)m_pCurrent);
			break;
		case MAP_CANCEL_TAG_QUERY:
			bFound = m_CancelTagMap.Lookup(QueryTag, (void*&)m_pCurrent);
			break;
		case MAP_ORDER_TAG_QUERY:
			bFound = m_OrderTagMap.Lookup(QueryTag, (void*&)m_pCurrent);
			break;
		default:
			m_pCurrent = NULL;
			return FALSE;
	}
	return bFound;
}

BOOL CMemTagSet::RemoveTags(int QueryType, const CString& QueryTag)
{
	if(GetTags(QueryType, QueryTag))
	{
		return RemoveTags();	
	}
	else
		return FALSE;
}

BOOL CMemTagSet::RemoveTags()
{
	if(m_pCurrent)
	{
		m_pCurrent->m_bUsed = FALSE;
		m_StaffordTagMap.RemoveKey(m_pCurrent->m_StaffordTag);
		m_ExchangeTagMap.RemoveKey(m_pCurrent->m_ExchangeTag);
		m_CancelTagMap.RemoveKey(m_pCurrent->m_CancelTag);
		m_OrderTagMap.RemoveKey(m_pCurrent->m_OrderTag);
		m_SequenceTagMap.RemoveKey((void*)m_pCurrent->m_SequenceTag);
		m_FreeList.AddTail(m_pCurrent);
		return TRUE;
	}
	return FALSE;
}