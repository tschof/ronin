// BRASSInterface.cpp: implementation of the CBRASSInterface class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BRASS.h"
#include "BRASSInterface.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBRASSInterface::CBRASSInterface(CDatabase* pDb, BOOL DoQuery)
{
	m_IsCounting = FALSE;
	LinkMemory(new CBRASSData());
	LinkRecord(new CBRASSRecord(pDb));
	if (DoQuery)
		m_Query = Query();
	else
		m_Query = FALSE;
}

BOOL CBRASSInterface::Query()
{
	m_MemoryList->Clear();
	CBRASSData *InsertMemory = (CBRASSData*)m_MemoryList;
	CBRASSRecord *ReadRecord = (CBRASSRecord*)m_RecordList;
	if(!ReadRecord->IsOpen())
		if(!ReadRecord->Open())
			return (m_Query = FALSE);
	//ReadRecord->Requery();
	SetFinalCount(ReadRecord->GetRecordCount());
	ReadRecord->MoveFirst();

	//if (ReadRecord->m_TraderAcr.GetLength() < 2)
	//	ReadRecord->MoveNext();

	while(!ReadRecord->IsEOF())
	{		
		IncrementCount();

		InsertMemory->SetCustomerID((CString&)ReadRecord->GetCustomerID());
		InsertMemory->SetTraderAcr((CString&)ReadRecord->GetTraderAcr());
		InsertMemory->SetLocalAcAcr((CString&)ReadRecord->GetLocalAcAcr());
		InsertMemory->SetClearingAcID((CString&)ReadRecord->GetClearingAcID());
		InsertMemory->SetFirmAcr((CString&)ReadRecord->GetFirmAcr());

		m_RecordList->MoveNext();
		if(!ReadRecord->IsEOF())
			InsertMemory = (CBRASSData*)InsertMemory->Insert();
	}
	ReadRecord->Close();
	return (m_Query = TRUE);
}
