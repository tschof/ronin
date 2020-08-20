/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *   IslandMemTagSet.h   - An extention of the MemTagSet class in COMMON.  This
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

#ifndef __ISLAND_MEM_TAG_SET_H__
#define __ISLAND_MEM_TAG_SET_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "MemTagSet.h"
#include "TMemoryMapInitializer.h"

template<class T>
class TIslandMemoryMapInitializer : public TMemoryMapInitializer<T>
{
    
protected:
    
    void AddMemoryMapEntry(CMemoryMapInitializerData& memoryMapEntry)
    {
        
        try
        {
            if (true == FileCreated() || true == ForceReload())
            {
                TagSet().CreateNewMap(memoryMapEntry.OMTag(), memoryMapEntry.OrderTag(), -1,
                    memoryMapEntry.Tag(), memoryMapEntry.ExchangeTag(), memoryMapEntry.OrderTag());
                CString tradeFor(memoryMapEntry.TradeFor());
                TagSet().SetTradeFor(tradeFor, tradeFor.GetLength());
                
            }
        }
        catch(bool)
        {
        }
    }
    
protected:
    
    TIslandMemoryMapInitializer(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr) : TMemoryMapInitializer<T>(lDatabaseSettingsAutoPtr)
    {
    }
};

struct IslandMemTagStruct : public MemTagStruct
{
    char szTradeFor[10];
};

class CIslandMemTagSet : public CMemTagSet, public TIslandMemoryMapInitializer<CIslandMemTagSet>  
{
public:
    CIslandMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr, bool forceReload) 
        : TIslandMemoryMapInitializer<CIslandMemTagSet>(lDatabaseSettingsAutoPtr)
    {
        bool fileCreated = true;
        
        InitMap(fileCreated, forceReload);
        
        TagSet(this);
        
        TIslandMemoryMapInitializer<CIslandMemTagSet>::Initialize(fileCreated, forceReload);
    }
    
    virtual ~CIslandMemTagSet() {}
    
    void SetTradeFor(const char* szpTradeFor, int len) {
        if(Current()) {
            memset(((IslandMemTagStruct*)Current())->szTradeFor, '\0', 10);
            if(len < 4) {
                strncpy(((IslandMemTagStruct*)Current())->szTradeFor, szpTradeFor, len);
            } else {
                strncpy(((IslandMemTagStruct*)Current())->szTradeFor, szpTradeFor, 4);
            }
        }

    }

    const char* GetTradeFor() {
        if(Current()) {
            return  ((IslandMemTagStruct*)Current())->szTradeFor;
        } else {
            return NULL;
        }
    }
    
protected:
    virtual int SizeOfStruct() { return sizeof(IslandMemTagStruct); }
    
};





#endif //__ISLAND_MEM_TAG_SET_H__