#ifndef CMSSESSION
#define CMSSESSION

#include "ExchangeSession.h"
#include "CMSMsgSequence.h"
#include "Order.h"

//class CCMSMsg;
class CMessageApp;
class CExecution;

class CCMSSession : public CExchangeSession
{
	// Constructors & destructors
	public:
		CCMSSession();
		~CCMSSession() {}
		bool StartSession();
		const CCMSMsgSequence& OutgoingMsgs() const
			{ return m_OutgoingMsgs; }

	// Accessors
	protected:
		const CCMSMsgSequence& IncomingMsgs() const
			{ return m_IncomingMsgs; }
		const CString& TargetID() const
			{ return m_TargetID; }
		const CString& SenderID() const
			{ return m_SenderID; }
		int SendHeartbtInt() const
			{ return m_SendHeartbtInt; }
		int RcvHeartbtInt() const
			{ return m_RcvHeartbtInt; }

	//call back functions
		virtual void ExecutionOverride(CExecution& Exec, const IncomingCMSMsg& a_Exec) {}
		virtual BOOL OrderEntryOverride(const COrder& a_Order, CCMSMsg& a_CMSOrder) { return TRUE;}
//		virtual BOOL ReplaceOverride(const CReplace &a_Replace, CCMSMsg& a_FixReplace) {return TRUE;}
		virtual BOOL CancelOverride(const CCancel &a_Cxl, CCMSMsg& a_CMSCxl) {return TRUE;}

	// Services
	public:
		virtual BOOL Logon() ;
		virtual BOOL Logout();

		virtual long ProcessEntry(COrder& a_Order);
		virtual BOOL ProcessReplace(CReplace& a_Replace);
		virtual BOOL ProcessCancel(CCancel& a_Cancel);

		virtual void ProcessTimer();

		inline long GetNextIncomingSeqNum() const
			{ return m_IncomingMsgs.NextMsgSeqNum(); }

		inline long GetNextOutgoingSeqNum() const
			{ return m_OutgoingMsgs.NextMsgSeqNum(); }

		inline const CString& GetTargetID() 
			{ return m_TargetID;}

		inline const CString& GetSenderID() 
			{ return m_SenderID;}

		inline const CString& GetBranchCode() const
			{ return m_BranchCode; }

		inline const CString& GetPassword() const
			{ return m_Password; }

		void SetNextOutgoingSeqNum(const long& a_MsgSeqNum)
			{ m_OutgoingMsgs.ResetNextMsgSeqNum(a_MsgSeqNum);}

		void SetNextIncomingSeqNum(const long& a_MsgSeqNum)
			{ m_IncomingMsgs.ResetNextMsgSeqNum(a_MsgSeqNum);}



		virtual void BuildCMSTrailer(TString& a_CMSTrailer)
		{
			a_CMSTrailer.append(CTime::GetCurrentTime().Format("%Y-%m-%d/%H:%M:%S").GetBuffer(0));
			char l_SeqNo[16];
			sprintf(l_SeqNo, " OL %4d ",GetNextOutgoingSeqNum());
			a_CMSTrailer.append(l_SeqNo);
		}

		virtual CString	ProcessMessage(CString a_MsgStr, CBaseConnection* a_pConnection = NULL);

	protected:
		BOOL SendMsg(CCMSMsg& a_Msg);
		virtual void Process(IncomingCMSMsg& a_Msg);
		virtual void ProcessLogonReply(const IncomingCMSMsg& a_Msg);
		virtual void ProcessHeartbeat(const IncomingCMSMsg& a_Msg) {;}
		virtual void ProcessSeqReset(const IncomingCMSMsg& a_Msg);
		virtual void ProcessLogout(const IncomingCMSMsg& a_Msg);
		virtual void ProcessResendRequest(const IncomingCMSMsg& a_Msg);
		virtual void ProcessReport(const IncomingCMSMsg& a_Msg);
		virtual void ProcessCancelReject(const IncomingCMSMsg& a_Msg);
		virtual void ProcessReplaceReject(const IncomingCMSMsg& a_Msg){;}
		virtual void ProcessReject(const IncomingCMSMsg& a_Msg) {;}
		void Shutdown();

	//call back functions
		virtual bool OnSetAccountType(const COrder& a_Order,TString& a_OutStr);
		virtual void OnSetLineOneA(const COrder& a_Order,TString& a_OutStr) {;}
		virtual bool OnSetLinkage(const COrder& a_Order,TString& a_OutStr)=0;
		virtual bool OnPackTCPMsg(CCMSMsg& a_Msg) = 0;
		virtual bool OnUnpackTCPMsg(IncomingCMSMsg& a_Msg, const CString&,int& a_Offset) = 0;
		virtual bool GetLogonMsg(CCMSMsg& a_Msg) { return false;}
		virtual bool GetLogoutMsg(CCMSMsg& a_Msg) { return false;}
		virtual bool GetResetSeqMsg(long a_ResetTo,CCMSMsg& a_ResetMsg) { return false;}
		virtual bool GetHeartBeatMsg(CCMSMsg& a_Msg) {return false;}
		virtual bool GetResendReqMsg(CCMSMsg& a_Msg) {return false;}

	// Laborers
	protected:

		bool AddIncomingMsgToSequence(const CCMSMsg& a_Msg);
		//this is only for msg from exchange.
		virtual bool ParseCMSMsg(IncomingCMSMsg& a_Msg);
		virtual bool ParseHeader(IncomingCMSMsg& a_CMSMsg);
		virtual bool ParseAdminMsg(IncomingCMSMsg& a_CMSMsg);
		virtual bool ParseStatusMsg(IncomingCMSMsg& a_CMSMsg);
		virtual bool ParseReportMsg(IncomingCMSMsg& a_CMSMsg);
		virtual bool BuildCxlStr(CCMSMsg* a_Order, TString& l_MsgBody,CCancel& a_Cancel);
		static inline const char* GetExpirationMonth(const CString& a_ExpDate)
		{
			if(a_ExpDate.IsEmpty())
				throw "Expiration date not specified";
			LPCSTR l_MonthStr = ((LPCSTR) a_ExpDate) + 4;
			return m_MonthArray[atoi(l_MonthStr)-1];
		}

		//utility
	private:	
		
		static const char* m_MonthArray[12]; 
		virtual inline bool GetTIF(int a_Tif, TString& a_OutStr)
		{
			switch(a_Tif)
			{
			case COrder::Day:
				a_OutStr += "DAY\n";
				break;
			/*case COrder::GTC:
				a_OutStr += "GTC\n";
				break;*/
			case COrder::IC :
				a_OutStr += "OC\n";
				break;
			case COrder::FOK:
				a_OutStr += "FOK\n";
				break;
			default:
				return false;
			}
			return true;
		}

		virtual inline bool GetOrderType(COrder& a_Order, TString& a_OutStr)
		{
			CString l_Temp;

			switch(a_Order.Type())
			{
			case COrder::Stop:
				l_Temp.Format("%.6f STP\n",a_Order.StopPrice());
				break;
			case COrder::StopLimit:
				l_Temp.Format("%.6f STP %.6f LMT\n",a_Order.StopPrice(),a_Order.dPrice());
				break;
			case COrder::Market:
				l_Temp = "MKT\n";
				break;
			case COrder::Limit:
				l_Temp.Format("%.6f\n",a_Order.dPrice());
				break;
			default:
				return false;
			}
			a_OutStr += l_Temp;
			return true;
		}
	// Members
	//private:
	protected:
		CCMSMsgSequence m_IncomingMsgs;
		CCMSMsgSequence m_OutgoingMsgs;

		CString m_SenderID;
		CString m_TargetID;
		CString m_BranchCode;
		CString m_Password;

		int m_SendHeartbtInt;
		int m_RcvHeartbtInt;
		int m_StartTime;
		int m_EndTime;
		CTime m_LastSendTime;	
		CTime m_LastRcvTime;
		bool m_AwaitingResend;
		bool m_LogonInitiated;
		bool m_LogoutInitiated;

	protected:
		CMessageApp*	m_pApp;
		bool			m_bDoHeartBeat;
		bool			m_bDoLogon;
		bool			m_bDoLogout;
		bool			m_bDoResetSequence;
};

#endif
