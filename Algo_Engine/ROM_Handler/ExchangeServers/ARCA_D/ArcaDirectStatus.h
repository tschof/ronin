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
#ifndef ARCADIRECTSTATUS_H__
#define ARCADIRECTSTATUS_H__
#include "stdafx.h"
#include "StatusHandler.h"

class ArcaDirectStatus : public CStatusHandler {
    
public:
    ArcaDirectStatus();
    virtual ~ArcaDirectStatus();
    BOOL SetStatus(COrder& Order);
    BOOL SetStatus(CExecution& Exec);
    BOOL SetStatus(CReplace& replace);
    BOOL SetStatus(CCancel& Cancel);
    BOOL SetStatus(CReject& Reject);
    BOOL SetStatus(CCancelReject& Reject);
    BOOL SetStatus(CReplaceReject& Reject);
    BOOL ProcessTrade(CExecution& Exec);

};
#endif //ARCADIRECTSTATUS_H__
