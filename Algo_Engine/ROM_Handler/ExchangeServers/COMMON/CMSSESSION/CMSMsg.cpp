#include "CMSMsg.h"
#include "cmssession.h"
#include "messageapp.h"

CCMSMsg::CCMSMsg()
	:m_nSeqNumb(0),m_bPossDup(0)
{
}

TString CCMSMsg::ParseOutOrderTag() const
{ 
	if(m_MsgString.empty())
		return "";
	CMessageApp* l_pApp = (CMessageApp*) AfxGetApp();
	const CString& l_BranchCode = ((CCMSSession*) &(l_pApp->Exchange()))->GetBranchCode();
	TString::size_type l_Start = m_MsgString.find(l_BranchCode,m_CMSMsgOffset);
	if(l_Start == std::string::npos)
		return "";
	std::string::size_type l_End = m_MsgString.find(CMS_MSG_LINE_DELIMITOR,l_Start);
	if(l_End == std::string::npos)
		return "";
	return m_MsgString.substr(l_Start, l_End - l_Start);
}

IncomingCMSMsg::IncomingCMSMsg()
:m_LastPrice(0),m_LastShares(0),m_Leaves(0),m_TotalShares(0)
{
;
}