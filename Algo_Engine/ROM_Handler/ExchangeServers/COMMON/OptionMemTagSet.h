// MemTagSet.h: interface for the COptionMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MEMTAGSET_H__67DE3C93_EEB9_11D3_8CB3_005004DA7701__INCLUDED_)
#define AFX_MEMTAGSET_H__67DE3C93_EEB9_11D3_8CB3_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "OrderActions.h"
#include "ExchangeApp.h"
#include "MemoryMapInitializer\TMemoryMapInitializer.h"

#include <map>


#define MAP_STAFFORD_TAG_QUERY	0
#define MAP_EXCHANGE_TAG_QUERY	1
#define MAP_CANCEL_TAG_QUERY	2
#define MAP_ORDER_TAG_QUERY		3
#define MAP_NEW_MAP				4
#define MAP_SEQUENCE_TAG_QUERY	5
#define MAP_REMOVE_MAP			6
#define MAP_REPLACE_TAG_QUERY	7

#define ORDACT_NONE			0x00
#define ORDACT_CXL			0x01
#define ORDACT_REPLACE		0x02
#define ORDACT_CLOSED		0x04
#define ORDACT_REPLACE_ACK	0x08
#define ORDACT_OPEN			0x10

typedef int TOrderAction;

typedef struct
{
	bool			m_bUsed;
					
	char			m_CancelTag[36];
	char			m_ExchangeTag[36];
	char			m_OrderTag[36];
	long			m_SequenceTag;
	char			m_StaffordTag[36];
	char			m_Client[36];
	char			m_Date[20];
	char			m_ReplaceTag[36];

	long			m_LeavesShares;
	long			m_OriginalShares;
	long			m_LastCumQty;
	long			m_PendingReplaceShares;
	TOrderAction	m_Action;
	int				m_CustOrFirm;
	char			m_OpenClose;
	char			m_Capacity;
	int				m_MMExchange;
	char			m_MMAccount[17];
	int				m_CancelCount;
	int				m_ReplaceCount;		//Used to cache replace info
	int				m_Exchange;
	double			m_ReplacePrice;		//Used to cache replace info
	double			m_ReplaceStopPrice;	//Used to cache replace info
	int		 		m_ReplaceExecInst;	//Used to cache replace info
	int				m_TIF;				//Used to cache replace info
	int				m_ReplaceOrderType; //Used to cache replace info
	char			m_TradeFor[32];		//Used to cache reaplce info
	char			m_ClearingAccount[32];

}  OptionMemTagStruct, *POptionMemTagStruct;

typedef std::map<CString, OptionMemTagStruct *> TCStrPtrMap;

class COptionMemTagSet : public TOptionMemoryMapInitializer<COptionMemTagSet>
{
public:
	// COptionMemTagSet();
	COptionMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettings) 
		: m_deleteFile(false), TOptionMemoryMapInitializer<COptionMemTagSet>(lDatabaseSettings) 
	{
		m_pFileView				= NULL;
		m_hFile					= NULL;
		m_hFileMap				= NULL;
		m_iNumRecords			= 0;
		m_ppMemTags				= NULL;
		m_forceRemove			= false;
		m_fileCreated			= false;
	}

	COptionMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettings, bool forceReload, const char* storProcName = "") 
		: m_deleteFile(false), TOptionMemoryMapInitializer<COptionMemTagSet>(lDatabaseSettings)
	{
		//defualt initialization
		m_pFileView				= NULL;
		m_hFile					= NULL;
		m_hFileMap				= NULL;
		m_iNumRecords			= 0;
		m_ppMemTags				= NULL;
		m_forceRemove			= false;
		m_fileCreated			= false;

		TagSet(this);

		bool fileCreated = true;
		InitMap(fileCreated, forceReload);
		if ( fileCreated)
		{
			if(0 != strcmp(storProcName,""))
				SetStoreProcName(storProcName);
			Initialize(fileCreated, forceReload);
		}
	}
	
	virtual ~COptionMemTagSet();
	void InitMap();
	void InitMap(bool &FileCreated, bool ForceReload=false );

protected:
	virtual int SizeOfStruct() { return sizeof(OptionMemTagStruct); }
	virtual void AllocateMemoryMap(int iNumRecords, bool bReallocate = false);

private:
	BOOL UpdateTags(const CString& StaffordTag, const CString& ExchangeTag, const CString& CancelTag, const CString& OrderTag, const long SeqNum, const CString& Client);
	
public:
	BOOL UpdateTags(int QueryType, const CString& QueryTag, const CString& StaffordTag, const CString& ExchangeTag, const CString& CancelTag, const CString& OrderTag, const long SeqNum, const CString& Client);
	BOOL GetTags(int QueryType, const CString& QueryTag);
	BOOL RemoveTags(int QueryType, const CString& QueryTag);
	BOOL RemoveTags();
	BOOL CreateNewMap(const CString& StaffordTag, const CString& OrderTag, const long SeqNum, const CString& Client)	
		{ return(UpdateTags(MAP_NEW_MAP, "", StaffordTag, "", "", OrderTag, SeqNum, Client)); }
	
	const char* StaffordTag()	{ return m_pCurrent->m_StaffordTag; }
	const char* CancelTag()		{ return m_pCurrent->m_CancelTag;	}
	const char* ExchangeTag()	{ return m_pCurrent->m_ExchangeTag; }
	const char* OrderTag()		{ return m_pCurrent->m_OrderTag;	}
	const long Sequence()		{ return m_pCurrent->m_SequenceTag; }
	const char*	Client()		{ return m_pCurrent->m_Client;		}
	const char* Date()			{ return m_pCurrent->m_Date;		}
	const char* ReplaceTag()	{ return m_pCurrent->m_ReplaceTag;	}
	
	const double ReplacePrice()		{ return m_pCurrent->m_ReplacePrice;	}
	const long LeavesShares()		{ return m_pCurrent->m_LeavesShares;	}
	const long OriginalShares()		{ return m_pCurrent->m_OriginalShares;	}
	const long LastCumQty()			{ return m_pCurrent->m_LastCumQty;		}
	const TOrderAction OrderAction(){ return m_pCurrent->m_Action;			}
	const int CustFirm()			{ return m_pCurrent->m_CustOrFirm;		}
	const char OpenClose()			{ return m_pCurrent->m_OpenClose;		}
	const int Exchange()			{ return m_pCurrent->m_Exchange;		}
	const char Capacity()			{ return m_pCurrent->m_Capacity;		}
	const int MMExchange()			{ return m_pCurrent->m_MMExchange;		}
	const char* MMAccount()			{ return m_pCurrent->m_MMAccount;		}
	const long ReplaceShares()		{ return m_pCurrent->m_PendingReplaceShares;	}
	const int CancelCount()			{ return m_pCurrent->m_CancelCount;		}
	const int ReplaceCount()		{ return m_pCurrent->m_ReplaceCount;	}
	const int TimeInForce()			{ return m_pCurrent->m_TIF;	}
	const double ReplaceStopPrice()	{ return m_pCurrent->m_ReplaceStopPrice;}
	const int ReplaceExecInst()		{ return m_pCurrent->m_ReplaceExecInst;	}
	const int ReplaceOrderType()	{ return m_pCurrent->m_ReplaceOrderType;}
	const char* TradeFor()			{ return m_pCurrent->m_TradeFor;	}
	const char* ClearingAccount()	{ return m_pCurrent->m_ClearingAccount; }

	BOOL SetStaffordTag(const CString& Tag)	
	{ 
		if(m_pCurrent) 
		{
			m_StaffordTagMap.erase(m_pCurrent->m_StaffordTag);
			strcpy(m_pCurrent->m_StaffordTag, ((CString&)Tag).GetBuffer(0));
			return m_StaffordTagMap.insert(TCStrPtrMap::value_type(Tag, m_pCurrent)).second;
		}
		return FALSE;
	}
	BOOL SetCancelTag(const CString& Tag)
	{ 
		if(m_pCurrent) 
		{
			m_CancelTagMap.erase(m_pCurrent->m_CancelTag);
			strcpy(m_pCurrent->m_CancelTag, ((CString&)Tag).GetBuffer(0));
			return m_CancelTagMap.insert(TCStrPtrMap::value_type(Tag, m_pCurrent)).second;
		}
		return FALSE;
	}
	BOOL RemoveCancelTag()
	{ 
		if(m_pCurrent) 
		{
			m_CancelTagMap.erase(m_pCurrent->m_CancelTag);
			strcpy(m_pCurrent->m_CancelTag, "");
			return TRUE;
		}
		return FALSE;
	}
	BOOL SetExchangeTag(const CString& Tag)
	{ 
		if(m_pCurrent) 
		{
			m_ExchangeTagMap.erase(m_pCurrent->m_ExchangeTag);
			strcpy(m_pCurrent->m_ExchangeTag, ((CString&)Tag).GetBuffer(0));
			return m_ExchangeTagMap.insert(TCStrPtrMap::value_type(Tag, m_pCurrent)).second;
		}
		return FALSE;
	}
	BOOL SetClearingAccount(const CString& value)
	{ 
		if(m_pCurrent) 
		{
			strcpy(m_pCurrent->m_ClearingAccount, ((CString&)value).GetBuffer(0));
			return TRUE;
		}
		return FALSE;
	}
	BOOL RemoveExchangeTag()
	{ 
		if(m_pCurrent) 
		{
			m_ExchangeTagMap.erase(m_pCurrent->m_ExchangeTag);
			strcpy(m_pCurrent->m_ExchangeTag, "");
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetOrderTag(const CString& Tag)
	{ 
		if(m_pCurrent) 
		{
			m_OrderTagMap.erase(m_pCurrent->m_OrderTag);
			strcpy(m_pCurrent->m_OrderTag, ((CString&)Tag).GetBuffer(0));
			return m_OrderTagMap.insert(TCStrPtrMap::value_type(Tag, m_pCurrent)).second;
		}
		return FALSE;
	}
	BOOL RemoveOrderTag()
	{ 
		if(m_pCurrent) 
		{
			m_OrderTagMap.erase(m_pCurrent->m_OrderTag);
			strcpy(m_pCurrent->m_OrderTag, "");
			return TRUE;
		}
		return FALSE;
	}
	BOOL SetSequenceTag(const long SeqNum)
	{ 
		if(m_pCurrent) 
		{
			m_SequenceTagMap.RemoveKey((void*)m_pCurrent->m_SequenceTag);
			if(m_pCurrent) 
				m_pCurrent->m_SequenceTag = SeqNum;
			m_SequenceTagMap.SetAt((void*)SeqNum, (void*)m_pCurrent);
		}
		return (BOOL)m_pCurrent;
	}
	BOOL RemoveSequenceTag()
	{ 
		if(m_pCurrent) 
		{
			m_SequenceTagMap.RemoveKey((void*)m_pCurrent->m_SequenceTag);
			if(m_pCurrent) 
				m_pCurrent->m_SequenceTag = -1;
		}
		return (BOOL)m_pCurrent;
	}
	BOOL SetClient(const CString& Client)
	{ 
		if(m_pCurrent) strcpy(m_pCurrent->m_Client, ((CString&)Client).GetBuffer(0));
		return (BOOL)m_pCurrent;
	}
	BOOL SetDate(const CString& Date)
	{
		if(m_pCurrent) strcpy(m_pCurrent->m_Date, ((CString&)Date).GetBuffer(0));
		TRACE("%s\n%s\n\n", m_pCurrent->m_Date, Date);
		return (BOOL)m_pCurrent;
	}
	BOOL SetReplaceTag(const CString& Tag)
	{ 
		if(m_pCurrent) 
		{
			m_ReplaceTagMap.erase(m_pCurrent->m_ReplaceTag);
			strcpy(m_pCurrent->m_ReplaceTag, ((CString&)Tag).GetBuffer(0));
			return m_ReplaceTagMap.insert(TCStrPtrMap::value_type(Tag, m_pCurrent)).second;
		}
		return FALSE;
	}
	BOOL RemoveReplaceTag()
	{ 
		if(m_pCurrent) 
		{
			m_ReplaceTagMap.erase(m_pCurrent->m_ReplaceTag);
			strcpy(m_pCurrent->m_ReplaceTag, "");
			return TRUE;
		}
		return FALSE;
	}

	const BOOL IsOrderActionSet(const TOrderAction OrderAction)
	{
		if(OrderAction)
			return Current()->m_Action & OrderAction ? TRUE : FALSE;
		else 
			return Current()->m_Action == OrderAction;
	}

	BOOL SetCancelCount(int val)	
	{ 
		if(Current()) 
		{
			Current()->m_CancelCount = val;	
			return TRUE;
		}
		return FALSE;
	}
	
	BOOL SetReplaceCount(int val)	
	{ 
		if(Current()) 
		{
			Current()->m_ReplaceCount = val;	
			return TRUE;
		}
		return FALSE;
	}
	
	BOOL SetMMAccount(const CString& MMAccount)	
	{ 
		if(Current()) 
		{
			strcpy(Current()->m_MMAccount , ((CString&)MMAccount).GetBuffer(0));
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetMMExchange(int val)	
	{ 
		if(Current()) 
		{
			Current()->m_MMExchange = val;	
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetCapacity(char val)	
	{ 
		if(!Current()) 
			return FALSE;

		Current()->m_Capacity = val;
		return TRUE;
	}

	BOOL SetExchange(int val)	
	{ 
		if(Current()) 
		{
			Current()->m_Exchange = val;	
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetLeavesShares(const long Shares)
	{ 
		if(Current()) 
		{
			Current()->m_LeavesShares = Shares;
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetOrderAction(const TOrderAction OrderAction)
	{ 
		if(Current()) 
		{
			if(OrderAction)
				Current()->m_Action |= OrderAction;
			else
				Current()->m_Action = ORDACT_NONE;							
			
			return TRUE;
		}
		return FALSE;
	}

	BOOL UnsetOrderAction(const TOrderAction OrderAction)
	{ 
		if(Current()) 
		{
			if(OrderAction)
				Current()->m_Action &= ~OrderAction;
			else
				Current()->m_Action = ORDACT_NONE;							

			return TRUE;
		}
		return FALSE;
	}

	BOOL SetReplacePrice(const double Price)
	{ 
		if(Current()) 
		{
			Current()->m_ReplacePrice = Price;
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetOriginalShares(const long Qty)
	{ 
		if(Current()) 
		{
			Current()->m_OriginalShares = Qty;
			return TRUE;
		}
		return FALSE;
	}
	BOOL SetLastCumQty(const long Qty)
	{ 
		if(Current()) 
		{
			Current()->m_LastCumQty = Qty;
			return TRUE;
		}
		return FALSE;
	}
	BOOL SetCustFirm(int val)	
	{ 
		if(Current()) 
		{
			Current()->m_CustOrFirm = val;	
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetOpenClose(char val)	
	{ 
		if(!Current()) 
			return FALSE;

		Current()->m_OpenClose = val;
		return TRUE;
	}

	BOOL SetReplaceShares(const long Shares)
	{ 
		if(Current()) 
		{
			Current()->m_PendingReplaceShares = Shares;
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetTimeInForce(const int val)	
	{ 
		if(Current()) 
		{
			Current()->m_TIF = val;	
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetReplaceStopPrice(const double StopPrice)
	{ 
		if(Current()) 
		{
			Current()->m_ReplaceStopPrice = StopPrice;
			return TRUE;
		}
		return FALSE;
	}
	
	BOOL SetReplaceExecInst(const int val)	
	{ 
		if(Current()) 
		{
			Current()->m_ReplaceExecInst = val;	
			return TRUE;
		}
		return FALSE;
	}

	BOOL SetReplaceOrderType(const int val)	
	{ 
		if(Current()) 
		{
			Current()->m_ReplaceOrderType = val;	
			return TRUE;
		}
		return FALSE;
	}

		
	BOOL SetTradeFor(const CString& TradeFor)	
	{ 
		if(Current()) 
		{
			strcpy(Current()->m_TradeFor , ((CString&)TradeFor).GetBuffer(0));
			return TRUE;
		}
		return FALSE;
	}

protected:
	OptionMemTagStruct *Current() { return m_pCurrent; }
	
protected:
	HANDLE m_hFile;
	HANDLE m_hFileMap;
	LPVOID m_pFileView;

	OptionMemTagStruct **m_ppMemTags;
	OptionMemTagStruct *m_pCurrent;

	CPtrList m_FreeList;
	CPtrList m_UsedList;
	int m_iNumRecords;

	TCStrPtrMap m_CancelTagMap;
	TCStrPtrMap m_ExchangeTagMap;
	TCStrPtrMap m_OrderTagMap;
	TCStrPtrMap m_StaffordTagMap;
	CMapPtrToPtr m_SequenceTagMap;
	TCStrPtrMap m_ReplaceTagMap;

	bool		m_fileCreated;
	bool		m_forceRemove;
	bool		m_deleteFile;

	CString	m_QueryTag;
	int		m_QueryType;
};

#endif // !defined(AFX_MEMTAGSET_H__67DE3C93_EEB9_11D3_8CB3_005004DA7701__INCLUDED_)
