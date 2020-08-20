/*******************************************************************************
*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
* 
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
*
*******************************************************************************/


#if !defined(_GLOBEXFIXSESSION_H)
#define _GLOBEXFIXSESSION_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FixSession.h"

//**************************************************************************
// 
//	Class:
//	CGlobexFixSession
//
//	Purpose:
//
//	Provides the GLOBEX specific message processing for messages sent to the exchange.
//
//	Base Class:
//  CFixSession
//
//	History
//
//	Name							Reason				Date
//	Randy Fredrick				Created				20-NOV-02
//****************************************************************************
class CGlobexFixSession : public CFixSession
{
public:
	CGlobexFixSession();
	virtual ~CGlobexFixSession();

	// Method declarations that provide GLOBEX ILINK (FIX 4.2) specific handling.
    virtual void Process(const CFixCancelReject& a_Reject);
    virtual long ProcessEntry(COrder& a_Order);
    virtual BOOL ProcessReplace(CReplace& a_Replace);
    virtual BOOL Logout();
    virtual void Process(const CFixLogout& a_Logout);
	virtual BOOL CancelOverride(const CCancel &a_Cxl, CFixCancel& a_FixCxl);
    virtual BOOL ReplaceOverride(const CReplace &a_Replace, CFixReplace& a_FixReplace);
	virtual void HeartbeatOverride(CFixHeartbeat& heartbeat);
	virtual void LogonOverride(CFixLogon& logon);
	virtual void LogoutOverride(CFixLogout& logout);
	virtual BOOL OrderEntryOverride(const COrder& a_Order, CFixOrder& a_FixOrder);
	virtual void RejectOverride(CFixReject& rejectRequest);
	virtual void ResendOverride(CFixResendRequest& resendRequest);
	virtual void SequenceResetOverride(CFixSequenceReset& sequenceReset);
	virtual void TestRequestOverride(CFixTestRequest& testRequest);

private:

	// Methods
	BOOL AddFixAdminMsgHeader(CFixMsg& fixMsg, const char* trader,
        const CString& clearingId);
	BOOL AddFixApplicationMsgHeader(CFixMsg& fixMsg, const char* trader,
        const CString& clearingId);
	BOOL AddFix42HeaderFields(CFixMsg& fixMsg, const char* trader,
        const CString& clearingId);
	void AddFix42OrderFields(const COrder& order, CFixOrder& fixMsg);
	void AddFix42CancelRequestFields(const CCancel& cancelRequest, CFixCancel& fixCancelRequest);
    void AddFix42ReplaceRequestFields(const CReplace& replace, CFixReplace& fixReplace);
	void SetSenderSubId(CFixMsg& fixMsg, CString& senderCompId);

	// Data

	CMapStringToString  m_SenderToSubMap;
   
	// FIX 4.2 tags
	static const EFixFieldID FIX_42_ExpireDate;

	// CME FIX custom tags
	static const EFixFieldID FIX_CME_LoginRouteId;
	static const EFixFieldID FIX_CME_CustomerTypeCode;
	static const EFixFieldID FIX_CME_FeeBilling;
    static const EFixFieldID FIX_CME_CorrelationOrdId;
    static const EFixFieldID FIX_CME_NextExpectedMsgSeqNum;
    static const EFixFieldID FIX_CME_OrderQtyFlag;

    /*Fix Session info*/
    CString m_firmName;
    CString m_firmId;
    CString m_fti;
    CString m_targetId;
    CString m_targetSubId;
    CString m_sessionId;
    CString m_password;
    CString m_SenderCompId;
    CString m_SenderSubId;
    CString m_SenderLocationId;

	// clearing info
    CString m_CustomerOrFirm;
    CString m_CustomerTypeCode;
    CString m_FeeBilling;
};

#endif // !defined(_GLOBEXFIXSESSION_H)