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
#ifndef ARCADIRECTHANDLER_H__
#define ARCADIRECTHANDLER_H__
#include "stdafx.h"
#include "OrderHandler.h"
#include "ArcaDirectMemTagSet.h"
#include "OrderTypeValidator.h"


class ArcaDirectHandler : public COrderHandler  {


private:
    ArcaDirectMemTagSet *m_pTags;
    COrderTypeValidator* m_orderTypeValidator;
    CString	m_DefaultAccount;
    void InitializeArcaDirectMemoryMap();
    void SetExecutionStatus(CExecution& exe);
    //static 
   // static int m_clientOrderID;
   // static 
   // static int GetNextClientOrderID();
   // static void SetNextClientOrderID(int id);

public:
    ArcaDirectHandler();
    virtual ~ArcaDirectHandler();
    
    // incoming
    virtual void ProcessMessage(COrder& Order);
    virtual void ProcessMessage(CCancel& Order);
    virtual void ProcessMessage(CReplace& Order);
    
    // outgoing
    virtual void ProcessMessage(const char *Status);
    virtual void ProcessMessage(CExecution& Report);
    virtual void ProcessMessage(CCancelReject& Reject);
    virtual void ProcessMessage(CReplaceReject& Reject);
    virtual void ProcessMessage(CReject& Reject);
};

#endif //ARCADIRECTHANDLER_H__
