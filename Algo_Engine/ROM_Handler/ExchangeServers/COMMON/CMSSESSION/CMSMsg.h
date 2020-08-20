#ifndef CMSMSG
#define CMSMSG


#pragma warning(disable:4786)

#include <string>
#include "cms.h"

typedef std::string TString;

class CCMSMsg 
{

	// Constructors & destructors
	public:
		CCMSMsg();
		virtual ~CCMSMsg() {}

	// Accessors
	public:		
		inline unsigned int MsgSeqNum() const
			{ return m_nSeqNumb;}
		
		inline bool PossDupFlag() const
			{ return m_bPossDup;}

		inline  ECMSMsgType MsgType() const 
			{ return m_MsgType; }

		inline TMsgSeqNum ExchangeReqSeqNum() const
			{ return m_ExchangeReqSeqNum;}

		inline  void SetMsgType(ECMSMsgType a_Value) 
			{ m_MsgType = a_Value; }

		inline void SetMsgSeqNum(unsigned int a_SeqNum) 
			{ m_nSeqNumb = a_SeqNum;}

		inline void SetPossDupFlag() 
			{ m_bPossDup = true;}

		inline SetExchangeReqSeqNum(int a_ReqSeqNum)
		{ m_ExchangeReqSeqNum = a_ReqSeqNum;}

		inline TString&	MsgStrForChange() { return m_MsgString;}
		inline const TString&	MsgStr() const { return m_MsgString;}
		inline const char*	CMSMsgPtr() const { return m_MsgString.c_str() + m_CMSMsgOffset;}

//		static void	SetCMSMsgOffset(int a_Pos)
//			{	m_CMSMsgOffset = a_Pos;	}

		TString ParseOutOrderTag() const;

/*		inline void SetPossDupOffset(int a_Offset)
		{
			m_PossDupOffset = a_Offset;
		}
*/
	protected:
		TString			m_MsgString;
//		int				m_PossDupOffset; //offset from m_CMSMsgOffset
		unsigned int	m_nSeqNumb;
		bool			m_bPossDup;
		ECMSMsgType		m_MsgType;
		TMsgSeqNum		m_ExchangeReqSeqNum;

		//static.  This one is set by different session
		static const int		m_CMSMsgOffset;

};

class IncomingCMSMsg : public CCMSMsg
{
public:

	IncomingCMSMsg();
	inline const TString& OrderID() const
	{		return m_OrderID;	}
	inline const TString& Text() const
	{		return m_Text;	}
	inline const TString& Symbol() const
	{		return m_Symbol;	}
	inline float LastPrice() const
	{		return m_LastPrice;	}
	inline int LastShares() const
	{		return m_LastShares;}
	inline int Leaves() const
	{		return m_Leaves;	}
	inline int TotalShares() const
	{		return m_TotalShares;}
	inline TMsgSeqNum ResendStart() const
	{		return m_ResendStart;}
	inline TMsgSeqNum ResendEnd() const
	{		return m_ResendEnd;}

	inline void SetOrderID(const TString& a_Value) 
	{		m_OrderID = a_Value;	}
	inline void SetText(const TString& a_Value) 
	{		m_Text = a_Value;	}
	inline void SetSymbol(const TString& a_Value) 
	{		m_Symbol = a_Value;	}
	inline void SetLastPrice(double a_Value)
	{		m_LastPrice = a_Value;	}
	inline void SetLastShares(int a_Value)
	{		m_LastShares = a_Value;}
	inline void SetLeaves(int a_Value) 
	{		m_Leaves = a_Value;}
	inline void SetTotalShares(int a_Value) 
	{		m_TotalShares = a_Value;}
	inline void SetResendStart(TMsgSeqNum a_Value )
	{		m_ResendStart = a_Value;}
	inline void SetResendEnd(TMsgSeqNum a_Value)
	{		m_ResendEnd = a_Value;}

private:
	TString m_OrderID;
	TString m_Text;
	TString m_Symbol;
	float	m_LastPrice;
	int		m_LastShares;
	int		m_Leaves;
	int		m_TotalShares;
	TMsgSeqNum	m_ResendStart;
	TMsgSeqNum	m_ResendEnd;

};


#endif
