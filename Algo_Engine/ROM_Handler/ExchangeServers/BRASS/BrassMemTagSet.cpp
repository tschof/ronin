// BrassMemTagSet.cpp: implementation of the CBrassMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Brass.h"
#include "BrassMemTagSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CBrassMemTagSet::CBrassMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettingsAutoPtr, bool forceReload)  : TBrassMemoryMapInitializer<CBrassMemTagSet>(lDatabaseSettingsAutoPtr)
{
	bool fileCreated = true;

	InitMap(fileCreated, forceReload);

	TagSet(this);

	TMemoryMapInitializer<CBrassMemTagSet>::Initialize(fileCreated, forceReload);
}

CBrassMemTagSet::~CBrassMemTagSet()
{

}
