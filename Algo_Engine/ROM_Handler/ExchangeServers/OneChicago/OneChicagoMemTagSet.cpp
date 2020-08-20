// OneChicagoMemTagSet.cpp: implementation of the COneChicagoMemTagSet class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "OneChicagoMemTagSet.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#include "stdafx.h"
#include "OneChicagoMemTagSet.h"
#include "ExchangeApp.h"
#include "Order.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
COneChicagoMemTagSet ::COneChicagoMemTagSet(TIDatabaseSettingsAutoPtr lDatabaseSettings, bool forceReload, const char* storProcName )
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

/*
	bool fileCreated = true;
      
	InitMap(fileCreated, forceReload);
	if (true == fileCreated)
	{
		if(0 != strcmp(storProcName,""))
			SetStoreProcName(storProcName);
		Initialize(fileCreated, forceReload);
	}
*/
}


COneChicagoMemTagSet ::	~COneChicagoMemTagSet()
{
}
