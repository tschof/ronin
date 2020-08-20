//**************************************************************************
// 
//	File:
//	
//	GlobexMemTagSet.h
//
//  Summary:
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#if !defined(AFX_GLOBEXMEMTAGSET_H__50808D9D_C506_4311_A121_1B135F3959EF__INCLUDED_)
#define AFX_GLOBEXMEMTAGSET_H__50808D9D_C506_4311_A121_1B135F3959EF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MemTagSet.h"
#include "TMemoryMapInitializer.h"

struct GlobexMemTagStruct : public MemTagStruct
{
	BOOL	m_bCancel;
    bool    m_bPendingCancel;
	long	m_Shares;
	long	m_OriginalShares;
	double	m_OriginalPrice;
	char	m_SecurityType;
    char    m_TradeFor[50];
    char    m_ClearingId[50];
    char    m_ClearingAccount[50];
};

class CGlobexMemTagSet  : public CMemTagSet, public TGLOBEXMemoryMapInitializer<CGlobexMemTagSet> 
{
public:
	CGlobexMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr, bool forceReload) 
        : TGLOBEXMemoryMapInitializer<CGlobexMemTagSet>(lDatabaseSettingsAutoPtr)
    {
        bool fileCreated = true;
        
        InitMap(fileCreated, forceReload);
        
        TagSet(this);
        
        TGLOBEXMemoryMapInitializer<CGlobexMemTagSet>::Initialize(fileCreated, forceReload);
    }
    virtual ~CGlobexMemTagSet() {}


	const double OriginalPrice()
	{ 
		return ((GlobexMemTagStruct*)Current())->m_OriginalPrice;
	}

	const long Shares()
	{ 
		return ((GlobexMemTagStruct*)Current())->m_Shares;	
	}

	const long OriginalShares()
	{
		return ((GlobexMemTagStruct*)Current())->m_OriginalShares;
	}

	const BOOL Cancel()
	{
		return ((GlobexMemTagStruct*)Current())->m_bCancel;
	}

    void SetClearingId(const char* clearingId) {
        if(Current() && clearingId) {
            memset(((GlobexMemTagStruct*)Current())->m_ClearingId, '\0', 50);
            strcpy(((GlobexMemTagStruct*)Current())->m_ClearingId, clearingId);
        }
    }

    const char* GetClearingId() {
        return ((GlobexMemTagStruct*)Current())->m_ClearingId;
    }

    void SetClearingAccount(const char* clearingAccount) {
        if(Current() && clearingAccount) {
            memset(((GlobexMemTagStruct*)Current())->m_ClearingAccount, '\0', 50);
            strcpy(((GlobexMemTagStruct*)Current())->m_ClearingAccount, clearingAccount);
        }
    }

    const char* GetClearingAccount() {
        return ((GlobexMemTagStruct*)Current())->m_ClearingAccount;
    }


    void SetTradeFor(const char* tradeFor) {
        if(Current() && tradeFor) {
            strcpy(((GlobexMemTagStruct*)Current())->m_TradeFor, tradeFor);
        }
    }

    const char* GetTradeFor() {
        return ((GlobexMemTagStruct*)Current())->m_TradeFor;
    }

	BOOL SetCancel(const BOOL bCancel)
	{ 
		if(Current())
		{
			((GlobexMemTagStruct*)Current())->m_bCancel = bCancel;
		}

		return (BOOL)Current();
	}

    const bool IsPendingCancel() {
        bool pendingCancel = false;
        if(Current()) {
            pendingCancel = ((GlobexMemTagStruct*)Current())->m_bPendingCancel;
        }
        return pendingCancel;
    }

    void SetPendingCancel(bool pendingCancel) {
        if(Current()) {
            ((GlobexMemTagStruct*)Current())->m_bPendingCancel = pendingCancel;
        }
    }

	BOOL SetShares(const long Shares)
	{ 
		if(Current())
		{
			((GlobexMemTagStruct*)Current())->m_Shares = Shares;
		}

		return (BOOL)Current();
	}

	BOOL SetOriginalShares(const long Shares)
	{ 
		if(Current())
		{
			((GlobexMemTagStruct*)Current())->m_OriginalShares = Shares;
		}

		return (BOOL)Current();
	}

	BOOL SetOriginalPrice(const double Price)
	{ 
		if(Current())
		{
			((GlobexMemTagStruct*)Current())->m_OriginalPrice = Price;
		}

		return (BOOL)Current();
	}

	void SetSecurityType(const char securityType)
	{
		((GlobexMemTagStruct*)Current())->m_SecurityType = securityType;
	}

	const char GetSecurityType()
	{
		return ((GlobexMemTagStruct*)Current())->m_SecurityType;
	}

protected:

	virtual int SizeOfStruct() { return sizeof(GlobexMemTagStruct); }
};

#endif // !defined(AFX_GLOBEXMEMTAGSET_H__50808D9D_C506_4311_A121_1B135F3959EF__INCLUDED_)
