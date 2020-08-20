/*******************************************************************************
*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
*  : Defines the class behaviors for the application.
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
*     Parnell     
*
*******************************************************************************/

#ifndef _GLOBEX_MEM_TAG_SET_H__
#define _GLOBEX_MEM_TAG_SET_H__

#include "MemoryTagSet.h"
#include "TMemoryMapInitializer.h"

struct GlobexMemTagStruct : public MTAG::MemoryTagStruct {
    char accountId[32];
    char tradeFor[32];
    int originalShares;
    int iCumShares;
    int iLeavesShares;
    int shares;
};


class CGlobexMemTagSet : public MTAG::CMemoryTagSet,
public TGlobexMemoryMapInitializer<CGlobexMemTagSet> {
public:
    CGlobexMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr,
        bool forceReload,const char* path, const char* appName, int numRecords)
        : CMemoryTagSet(path, appName, numRecords),
        TGlobexMemoryMapInitializer<CGlobexMemTagSet>(lDatabaseSettingsAutoPtr) {
        InitMap(forceReload);
        TagSet(this);
        try {
            TGlobexMemoryMapInitializer<CGlobexMemTagSet>::Initialize(forceReload);
        } catch (...) {
            
        }
    }
    
    inline  void SetShares(int total) {
        if(Current()) {            
            ((GlobexMemTagStruct*)Current())->shares = total;
        }
    }
    
    inline void SetShares(const std::string& total) {
        if(Current()) {            
            ((GlobexMemTagStruct*)Current())->shares = atoi(total.c_str());
        }
    }
    
    inline int GetShares() {
        if(Current()) {
            return ((GlobexMemTagStruct*)Current())->shares;
        } else {
            return 0;
        }
    }
    
    inline  void SetOriginalShares(int total) {
        if(Current()) {            
            ((GlobexMemTagStruct*)Current())->originalShares = total;
        }
    }
    
    inline void SetOriginalShares(const std::string& total) {
        if(Current()) {            
            ((GlobexMemTagStruct*)Current())->originalShares = atoi(total.c_str());
        }
    }
    
    inline int GetOriginalShares() {
        if(Current()) {
            return ((GlobexMemTagStruct*)Current())->originalShares;
        } else {
            return 0;
        }
    }
    
    
    inline  void SetICumShares(int total) {
        if(Current()) {            
            ((GlobexMemTagStruct*)Current())->iCumShares = total;
        }
    }
    
    inline void SetICumShares(const std::string& total) {
        if(Current()) {            
            ((GlobexMemTagStruct*)Current())->iCumShares = atoi(total.c_str());
        }
    }
    inline int GetICumShares() {
        if(Current()) {
            return ((GlobexMemTagStruct*)Current())->iCumShares;
        } else {
            return 0;
        }
    }
    inline  void SetILeavesShares(int total) {
        if(Current()) {            
            ((GlobexMemTagStruct*)Current())->iLeavesShares = total;
        }
    }
    
    inline void SetILeavesShares(const std::string& total) {
        if(Current()) {            
            ((GlobexMemTagStruct*)Current())->iLeavesShares = atoi(total.c_str());
        }
    }
    inline int GetILeavesShares() {
        if(Current()) {
            return ((GlobexMemTagStruct*)Current())->iLeavesShares;
        } else {
            return 0;
        }
    }
    
    
    void SetAccountId(const std::string& account) {
        if(Current()) {            
            memset(((GlobexMemTagStruct*)Current())->accountId, '\0', 32);
            strcpy(((GlobexMemTagStruct*)Current())->accountId, account.c_str());
        }
    }
    
    char* AccountId() {
        if(Current()) { 
            return ((GlobexMemTagStruct*)Current())->accountId;
        } else {
            return NULL;
        }
    }
    
    void SetTradeFor(const std::string& trader) {
        if(Current()) {            
            memset(((GlobexMemTagStruct*)Current())->tradeFor, '\0', 32);
            strcpy(((GlobexMemTagStruct*)Current())->tradeFor, trader.c_str());
        }
    }
    
    char* TradeFor() {
        if(Current()) { 
            return ((GlobexMemTagStruct*)Current())->tradeFor;
        } else {
            return NULL;
        }
    }
    
    virtual ~CGlobexMemTagSet() {}
    
protected:
    virtual int SizeOfStruct() {return sizeof(GlobexMemTagStruct);}
    
};


#endif //_GLOBEX_MEM_TAG_SET_H__
