#include "stdafx.h"
#include "ClientMsg.h"
#include "OatsCallback.h"
#include "OatsDlg.h"


#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

extern COatsDlg *theDlg;
CClientMsg::CClientMsg()
{
}


CClientMsg::CClientMsg(const CString & msg, bool bAdmin) : m_bAdmin(bAdmin)
{
	m_Msg = msg;
}

bool CClientMsg::Process()
{
//	if (m_bAdmin)
//	{
//		(theDlg->GetCallback())->SendToAdmin(m_Msg);
//	}
//	else
//	{
		int location = -1;
		CString str = "";

		while ((location = m_Msg.Find('\n')) >= 0)
		{
			if (location == 0)
			{
				str = m_Msg.Mid(1);
			}
			else
			{
				str = m_Msg.Left(location);
				m_Msg = m_Msg.Mid(location+1);
			}
			
			CCsv csv(str);
			(theDlg->GetCallback())->QueueDBMessage(csv);
		}
//	}

	return true;
}

