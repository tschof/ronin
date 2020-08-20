// MemTagSet.cpp: implementation of the COptionMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OptionMemTagSet.h"
#include "ExchangeApp.h"
#include "Order.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
void COptionMemTagSet::InitMap()
{
	CMessageApp* pApp = (CMessageApp*)AfxGetApp();

	if (TRUE == pApp->IsFirstRun())
	{
		m_deleteFile = true;
	}

	if (true == m_forceRemove)
	{
		m_deleteFile = true;
	}

	int iNumRecords = GetPrivateProfileInt("MemoryMapSettings","Allocated Orders", 5000, pApp->DatFile());
	AllocateMemoryMap(iNumRecords, false);
}

void COptionMemTagSet::InitMap(bool& fileCreated, bool forceRemove)
{
	fileCreated = false;

	m_forceRemove = forceRemove;

	InitMap();

	fileCreated = m_fileCreated;
}

COptionMemTagSet::~COptionMemTagSet()
{
	if (m_pFileView)
		UnmapViewOfFile(m_pFileView);

	if (m_hFileMap)
		CloseHandle(m_hFileMap);

	if (m_ppMemTags != NULL)
	{
		delete m_ppMemTags;
	}
}

void COptionMemTagSet::AllocateMemoryMap(int iNumRecords, bool bReallocate) 
{
	CString StrTime = CTime::GetCurrentTime().Format("%B%d%Y");

	if(m_pFileView)
		UnmapViewOfFile(m_pFileView);
	if(m_hFileMap)
		CloseHandle(m_hFileMap);
	if(m_hFile)
		CloseHandle(m_hFile);

	m_FreeList.RemoveAll();

	m_CancelTagMap.clear();
	m_ExchangeTagMap.clear();
	m_OrderTagMap.clear();
	m_StaffordTagMap.clear();
	m_SequenceTagMap.RemoveAll();
	m_ReplaceTagMap.clear();

	if (m_ppMemTags != NULL)
	{
		delete m_ppMemTags;
	}

	CExchangeApp* pApp = (CExchangeApp*)AfxGetApp();

	CString MapPath;
	GetPrivateProfileString("Log File Information", "Map File Path", "c:\\OrderManager\\", MapPath.GetBuffer(128), 128, pApp->DatFile()); MapPath.ReleaseBuffer();

	if (true == m_deleteFile && ! bReallocate)
	{
		DeleteFile((LPCTSTR)(MapPath + pApp->Name() + "Tag.map"));
	}

	m_hFile = CreateFile(
		MapPath + pApp->Name() + "Tag.map",
		GENERIC_READ | GENERIC_WRITE,
		FILE_SHARE_READ | FILE_SHARE_WRITE,
		NULL,
		OPEN_ALWAYS,   // CREATE_NEW,
		FILE_ATTRIBUTE_NORMAL,	
		NULL
		);

	m_fileCreated = ( !bReallocate 
				   && m_hFile != (HANDLE)0xFFFFFFFF
				   && GetLastError() != ERROR_ALREADY_EXISTS );

	int iSize;
	if( bReallocate || m_deleteFile ) 
		iSize = SizeOfStruct() * iNumRecords;
	else
	{
		iSize = GetFileSize(m_hFile, NULL);
		iNumRecords = iSize / SizeOfStruct();
	}

	m_iNumRecords = iNumRecords;
	m_ppMemTags = new POptionMemTagStruct[iNumRecords];

	m_hFileMap = CreateFileMapping(
		(HANDLE)m_hFile,
		NULL,
		PAGE_READWRITE,
		0,
		SizeOfStruct() * iNumRecords,
		pApp->Name() + "_MEM");

	m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0);
	char *pTmp = (char *)m_pFileView;

	if (m_fileCreated == false)
	{
		m_ppMemTags[0] = (OptionMemTagStruct*)m_pFileView;
	}

	if(m_pFileView)
	{
		if(m_fileCreated)
		{
			for (int i = 0; i < iNumRecords; i++)
			{
				memset(pTmp + SizeOfStruct() * i,0,SizeOfStruct());//clearing the object before it is recycled
				m_ppMemTags[i] = (POptionMemTagStruct)(pTmp + SizeOfStruct() * i);
				m_ppMemTags[i]->m_bUsed = FALSE;
				strcpy(m_ppMemTags[i]->m_Date, (LPCTSTR)StrTime);
				
				m_FreeList.AddTail(m_ppMemTags[i]);
			}
		}
		else
		{
			for (int i = 0; i < iNumRecords; i++)
			{
				m_ppMemTags[i] = (POptionMemTagStruct)(pTmp + SizeOfStruct() * i);
				m_pCurrent = m_ppMemTags[i];
				if(m_pCurrent->m_bUsed)
				{
					if(m_pCurrent->m_StaffordTag != NULL)
						m_StaffordTagMap.insert(TCStrPtrMap::value_type(m_ppMemTags[i]->m_StaffordTag, m_ppMemTags[i]));
					if(m_pCurrent->m_ExchangeTag != NULL)
						m_ExchangeTagMap.insert(TCStrPtrMap::value_type(m_ppMemTags[i]->m_ExchangeTag, m_ppMemTags[i]));
					if(m_pCurrent->m_CancelTag != NULL)
						m_CancelTagMap.insert(TCStrPtrMap::value_type(m_ppMemTags[i]->m_CancelTag, m_ppMemTags[i]));
					if(m_pCurrent->m_OrderTag != NULL)
						m_OrderTagMap.insert(TCStrPtrMap::value_type(m_ppMemTags[i]->m_OrderTag, m_ppMemTags[i]));
					if(m_pCurrent->m_SequenceTag != NULL)
						m_SequenceTagMap.SetAt((void*)(m_ppMemTags[i]->m_SequenceTag), (void*)m_ppMemTags[i]);
					if(m_pCurrent->m_ReplaceTag != NULL)
						m_ReplaceTagMap.insert(TCStrPtrMap::value_type(m_ppMemTags[i]->m_ReplaceTag, m_ppMemTags[i]));
				}
				else
				{
					memset(m_pCurrent,0,SizeOfStruct());
					strcpy(m_ppMemTags[i]->m_Date, (LPCTSTR)StrTime);
					m_ppMemTags[i]->m_bUsed = FALSE;
					m_FreeList.AddTail(m_ppMemTags[i]);
				}
			}
		}
	}
	else
	{
		if (m_ppMemTags != NULL)
		{
			delete m_ppMemTags;
			m_ppMemTags = NULL;
		}
	}

	if (m_ppMemTags != NULL)
	{
		m_pCurrent = m_ppMemTags[0];
	}
}

BOOL COptionMemTagSet::UpdateTags(const CString& StaffordTag, const CString& ExchangeTag, const CString& CancelTag, const CString& OrderTag, const long SeqNum, const CString& Client)
{
	if (m_ppMemTags)
	{
		TCStrPtrMap::iterator l_It;

		switch (m_QueryType)
		{
		case MAP_NEW_MAP:
			if(!m_FreeList.GetCount())
			{
				CString name = ((CMessageApp*)AfxGetApp())->Name();
				((CMessageApp*)AfxGetApp())->ProcessError(name, ":: Reallocating memory for bigger map size !!!");
				 AllocateMemoryMap(m_iNumRecords * 2, true);
			}
			m_pCurrent = (OptionMemTagStruct*)m_FreeList.RemoveHead();
			m_pCurrent->m_bUsed = TRUE;
			if(StaffordTag.GetLength())
				SetStaffordTag(StaffordTag);
			if(ExchangeTag.GetLength())
				SetExchangeTag(ExchangeTag);
			if(CancelTag.GetLength())
				SetCancelTag(CancelTag);
			if(OrderTag.GetLength())
				SetOrderTag(OrderTag);
			if(SeqNum)
				m_SequenceTagMap.SetAt((void*)SeqNum, m_pCurrent);
//			if(ReplaceTag.GetLength())
//				SetReplaceTag(ReplaceTag);
			break;
		case MAP_STAFFORD_TAG_QUERY:
			l_It = m_StaffordTagMap.find(StaffordTag);
			if(l_It == m_StaffordTagMap.end())
				m_pCurrent = NULL;
			else
				m_pCurrent = l_It->second;
			break;
		case MAP_EXCHANGE_TAG_QUERY:
			l_It = m_ExchangeTagMap.find(ExchangeTag);
			if(l_It == m_ExchangeTagMap.end())
				m_pCurrent = NULL;
			else
				m_pCurrent = l_It->second;
			break;
		case MAP_CANCEL_TAG_QUERY:
			l_It = m_CancelTagMap.find(CancelTag);
			if(l_It == m_CancelTagMap.end())
				m_pCurrent = NULL;
			else
				m_pCurrent = l_It->second;
			break;
		case MAP_ORDER_TAG_QUERY:
			l_It = m_OrderTagMap.find(OrderTag);
			if(l_It == m_OrderTagMap.end())
				m_pCurrent = NULL;
			else
				m_pCurrent = l_It->second;
			break;
		case MAP_SEQUENCE_TAG_QUERY:
			m_SequenceTagMap.Lookup((void*)SeqNum, (void*&)m_pCurrent);
			break;
/*		case MAP_REPLACE_TAG_QUERY:
			l_It = m_ReplaceTagMap.find(ReplaceTag);
			if(l_It == m_ReplaceTagMap.end())
				m_pCurrent = NULL;
			else
				m_pCurrent = l_It->second;
			break;*/
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
/*			if (ReplaceTag != "")		
				strcpy(m_pCurrent->m_ReplaceTag, ((CString&)ReplaceTag).GetBuffer(0));*/
			return TRUE;
		}
	}

	return FALSE;
}

BOOL COptionMemTagSet::UpdateTags(int QueryType, const CString& QueryTag, const CString& StaffordTag, const CString& ExchangeTag, const CString& CancelTag, const CString& OrderTag, const long SeqNum, const CString& Client)
{
	m_QueryType = QueryType;

	return(UpdateTags(StaffordTag, ExchangeTag, CancelTag, OrderTag, SeqNum, Client));
}

BOOL COptionMemTagSet::GetTags(int QueryType, const CString& QueryTag)
{
	if(QueryTag.IsEmpty())
		return false;
	
	m_QueryType = QueryType;
	BOOL bFound = FALSE;
	TCStrPtrMap::iterator l_It;
		
	switch (m_QueryType)
	{
		case MAP_STAFFORD_TAG_QUERY:
			l_It = m_StaffordTagMap.find(QueryTag);
			if(bFound = (l_It != m_StaffordTagMap.end()))
				m_pCurrent = l_It->second;
			else
				m_pCurrent = NULL;
			break;
		case MAP_EXCHANGE_TAG_QUERY:
			l_It = m_ExchangeTagMap.find(QueryTag);
			if(bFound = (l_It != m_ExchangeTagMap.end()))
				m_pCurrent = l_It->second;
			else
				m_pCurrent = NULL;
			break;
		case MAP_CANCEL_TAG_QUERY:
			l_It = m_CancelTagMap.find(QueryTag);
			if(bFound = (l_It != m_CancelTagMap.end()))
				m_pCurrent = l_It->second;
			else
				m_pCurrent = NULL;
			break;
		case MAP_ORDER_TAG_QUERY:
			l_It = m_OrderTagMap.find(QueryTag);
			if(bFound = (l_It != m_OrderTagMap.end()))
				m_pCurrent = l_It->second;
			else
				m_pCurrent = NULL;
			break;
		case MAP_SEQUENCE_TAG_QUERY:
			char sSequence[20];
			strcpy(sSequence, (LPCTSTR)QueryTag);
			bFound = m_SequenceTagMap.Lookup((void *)atol(sSequence), (void*&)m_pCurrent);
			break;
		case MAP_REPLACE_TAG_QUERY:
			l_It = m_ReplaceTagMap.find(QueryTag);
			if(bFound = (l_It != m_ReplaceTagMap.end()))
				m_pCurrent = l_It->second;
			else
				m_pCurrent = NULL;
			break;
		default:
			m_pCurrent = NULL;
			return FALSE;
	}
	return bFound;
}

BOOL COptionMemTagSet::RemoveTags(int QueryType, const CString& QueryTag)
{
	if(GetTags(QueryType, QueryTag))
	{
		return RemoveTags();	
	}
	else
		return FALSE;
}

BOOL COptionMemTagSet::RemoveTags()
{
	if(m_pCurrent)
	{
		CString StrTime = CTime::GetCurrentTime().Format("%B%d%Y");

		m_StaffordTagMap.erase(m_pCurrent->m_StaffordTag);
		m_ExchangeTagMap.erase(m_pCurrent->m_ExchangeTag);
		m_CancelTagMap.erase(m_pCurrent->m_CancelTag);
		m_OrderTagMap.erase(m_pCurrent->m_OrderTag);
		m_SequenceTagMap.RemoveKey((void*)m_pCurrent->m_SequenceTag);
		m_ReplaceTagMap.erase(m_pCurrent->m_ReplaceTag);
		
		memset(m_pCurrent,0,SizeOfStruct());
		m_pCurrent->m_bUsed = FALSE;
		strcpy(m_pCurrent->m_Date, (LPCTSTR)StrTime);
		m_FreeList.AddTail(m_pCurrent);
		return TRUE;
	}
	return FALSE;
}