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
#ifndef ARCADIRECTMEMETAGSET_H__
#define ARCADIRECTMEMETAGSET_H__

#include "stdafx.h"
#include "MemTagSet.h"
#include "TMemoryMapInitializer.h"

struct ArcaDirectMemTagStruct : public MemTagStruct
{
    long m_Shares;
    long m_executedShares;
    double m_price;
    char m_source[36];
};

class ArcaDirectMemTagSet : public CMemTagSet, public TArcaDirectMemoryMapInitializer<ArcaDirectMemTagSet> {

public:
    ArcaDirectMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr, bool forceReload)
        : TArcaDirectMemoryMapInitializer<ArcaDirectMemTagSet>(lDatabaseSettingsAutoPtr) {
        bool fileCreated = true;
        InitMap(fileCreated, forceReload);
        TagSet(this);
        TArcaDirectMemoryMapInitializer<ArcaDirectMemTagSet>::Initialize(fileCreated, forceReload);
    }

    virtual ~ArcaDirectMemTagSet() {}

    const char* Source() { 
        if(Current()) {
            return ((ArcaDirectMemTagStruct*)Current())->m_source;
        }
    }

    void SetSource(const CString& source) {
        if(Current()) {   
            memset(((ArcaDirectMemTagStruct*)Current())->m_source, '\0',36);
            int cpylen = source.GetLength() < 36 ? source.GetLength() : 36;
            strncpy(((ArcaDirectMemTagStruct*)Current())->m_source,
                    ((CString&)source).GetBuffer(0), cpylen);
        }
    }

    long Shares() { return ((ArcaDirectMemTagStruct*)Current())->m_Shares; }
    
    void SetShares(long value) { 
        if(Current()) {
            ((ArcaDirectMemTagStruct*)Current())->m_Shares = value;
        }
    }

    long ExecutedShares() { return ((ArcaDirectMemTagStruct*)Current())->m_executedShares; }
    void SetExecutedShares(long value) {
        if(Current()) {
            ((ArcaDirectMemTagStruct*)Current())->m_executedShares = value;
        }
    }
    void AddToExecutedShares(long value) { 
        if(Current()) {
            ((ArcaDirectMemTagStruct*)Current())->m_executedShares += value;
        }
    }

    double Price() {
	return ((ArcaDirectMemTagStruct*)Current())->m_price;
    }
    
    void SetPrice(double price) {
	if(Current()) {
	    ((ArcaDirectMemTagStruct*)Current())->m_price = price;
	}
    }


protected:
    virtual int SizeOfStruct() { return sizeof(ArcaDirectMemTagStruct); }
};


#endif //ARCADIRECTMEMETAGSET_H__
