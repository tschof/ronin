/*******************************************************************************
 *
 * Copyright (c) 2006 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *   LavaMemTagSet.h   - An extention of the MemTagSet class in COMMON.  This
 *                       version adds shares and cancelReplace count fields.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *                          so that the memory map file is now properly initialized.
 *******************************************************************************/

// LavaMemTagSet.h: interface for the CLavaMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LavaMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
#define AFX_LavaMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "MemTagSet.h"
#include "TMemoryMapInitializer.h"

struct LavaMemTagStruct : public MemTagStruct
{
};

class CLavaMemTagSet : public CMemTagSet, public TBRUTMemoryMapInitializer<CLavaMemTagSet>  
{
public:
    CLavaMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr, bool forceReload) 
        : TBRUTMemoryMapInitializer<CLavaMemTagSet>(lDatabaseSettingsAutoPtr)
    {
        bool fileCreated = true;
        
        InitMap(fileCreated, forceReload);
        
        TagSet(this);
        
        TBRUTMemoryMapInitializer<CLavaMemTagSet>::Initialize(fileCreated, forceReload);
    }
    
    virtual ~CLavaMemTagSet() {}
    
        
protected:
    virtual int SizeOfStruct() { return sizeof(LavaMemTagStruct); }
    
};

#endif // !defined(AFX_LavaMEMTAGSET_H__1B6783F3_04CD_11D4_8CB8_005004DA7701__INCLUDED_)
