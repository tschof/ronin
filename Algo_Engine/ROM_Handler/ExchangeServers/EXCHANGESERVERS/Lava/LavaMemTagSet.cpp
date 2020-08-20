// LavaMemTagSet.cpp: implementation of the CLavaMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "LavaMemTagSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "LavaMemTagSet.h"
#include "ExchangeApp.h"
#include "Order.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
CLavaMemTagSet ::CLavaMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettings, bool forceReload, const char* storProcName )
	 : COptionMemTagSet(lDatabaseSettings)
{
	//defualt initialization
	m_pFileView				= NULL;
	m_hFile					= NULL;
	m_hFileMap				= NULL;
	m_iNumRecords			= 0;
	m_ppMemTags				= NULL;
	m_forceRemove			= false;
	m_fileCreated			= false;

	//defualt initialization

	bool fileCreated = true;

	CMessageApp *pApp = (CMessageApp *)AfxGetApp();

	TagSet(this);

	InitMap(fileCreated, forceReload);
		
	if (true == fileCreated)
	{
		if(0 != strcmp(storProcName,""))
			SetStoreProcName(storProcName);
		Initialize(fileCreated, forceReload);
	}

}


CLavaMemTagSet ::	~CLavaMemTagSet()
{
}
