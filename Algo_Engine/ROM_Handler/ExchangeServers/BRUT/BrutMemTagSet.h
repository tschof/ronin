/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *   BrutMemTagSet.h   - An extention of the MemTagSet class in COMMON.  This
 *                       version adds shares and cancelReplace count fields.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 11-11-2005   Parnell     Added a field to count cancel replaces and Fixed the constructor
 *                          so that the memory map file is now properly initialized.
 *******************************************************************************/

// BrutMemTagSet.h: interface for the CBrutMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_BrutMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
#define AFX_BrutMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MemTagSet.h"
#include "TMemoryMapInitializer.h"

struct BrutMemTagStruct : public MemTagStruct
{
    long m_Shares;
    int  m_nCancelReplaceCnt;
    int m_status;
};

class CBrutMemTagSet : public CMemTagSet, public TBRUTMemoryMapInitializer<CBrutMemTagSet>  
{
public:
    CBrutMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr, bool forceReload) 
        : TBRUTMemoryMapInitializer<CBrutMemTagSet>(lDatabaseSettingsAutoPtr)
    {
        bool fileCreated = true;
        
        InitMap(fileCreated, forceReload);
        
        TagSet(this);
        
        TBRUTMemoryMapInitializer<CBrutMemTagSet>::Initialize(fileCreated, forceReload);
    }
    
    virtual ~CBrutMemTagSet() {}
    
    long Shares() { return ((BrutMemTagStruct*)Current())->m_Shares; }
    
    void SetShares(long value)
    { 
        if(Current()) 
        {
            ((BrutMemTagStruct*)Current())->m_Shares = value;
        }
    }

    int Status() { return ((BrutMemTagStruct*)Current())->m_status; }
    
    void SetStatus(int status) {
        if(Current()) {
            ((BrutMemTagStruct*)Current())->m_status = status;
        }
    }
    const int CancelReplaceCnt()
    {
        return ((BrutMemTagStruct*)Current())->m_nCancelReplaceCnt;
    }
    BOOL SetCancelReplaceCnt(const int nCount)
    { 
        if(Current())
        {
            ((BrutMemTagStruct*)Current())->m_nCancelReplaceCnt = nCount;
            return (BOOL)Current();
        }
        
        return FALSE;
    }
    
protected:
    virtual int SizeOfStruct() { return sizeof(BrutMemTagStruct); }
    
};

#endif // !defined(AFX_BrutMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
