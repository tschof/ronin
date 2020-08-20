/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *  BRUTHandler.h  - Definition of the Handling code for the Brut Exchange.
 *                   
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 11-22-2005   Parnell     I added the InitializeBrutMemoryMap function in order 
 *                          to initialize the memory map file for the Brut exchange
 *                          server.  I added the CreateReplaceTag function in order
 *                          to create unique tags for replace orders.
 *******************************************************************************/

#ifndef __BRUTHANDLER_H__
#define __BRUTHANDLER_H__
#include "BrutMemTagSet.h"
#include "OrderTypeValidator.h"


class CFixSession;

class CBRUTHandler : public COrderHandler
{
public:
	void ProcessMessage(COrder& Order);
	void ProcessMessage(CReplace& Replace);
	void ProcessMessage(CCancel& Cancel);
	
	void ProcessMessage(CExecution& pReport);
	void ProcessMessage(CCancelReject& pReject);
	void ProcessMessage(CReject& FixReject);
	void ProcessMessage(const char *Status);
		
private:
	CBrutMemTagSet			*m_pTags;
	CString				m_DefaultAccount;

    void CreateReplaceTag(CString& orderTag, int seq);
    void InitializeBrutMemoryMap();
    bool IsNASDSym(const char* symbol);
public:
	CBRUTHandler();
	virtual ~CBRUTHandler();

	COrderTypeValidator*	m_orderTypeValidator;
};
#endif