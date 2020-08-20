// ActHost.cpp: implementation of the CActHost class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "actor.h"
#include "ActHost.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActHost::CActHost()
{
	m_nHost = 0;
	m_Hosts.SetSize(6);
	CString Path = "c:\\OrderManager\\Act.dat";
	CString PHost, AHost, DPHost, DAHost, Host;

	GetPrivateProfileString("ACT Connection", "PHost", "", PHost.GetBuffer(128), 128, Path); PHost.ReleaseBuffer();
	GetPrivateProfileString("ACT Connection", "AHost", "", AHost.GetBuffer(128), 128, Path); AHost.ReleaseBuffer();
	//GetPrivateProfileString("ACT Connection", "DPHost", "", DPHost.GetBuffer(128), 128, Path); DPHost.ReleaseBuffer();
	//GetPrivateProfileString("ACT Connection", "DAHost", "", DAHost.GetBuffer(128), 128, Path); DAHost.ReleaseBuffer();
	GetPrivateProfileString("ACT Connection", "Current Host", "", Host.GetBuffer(128), 128, Path); Host.ReleaseBuffer();
	
	m_Hosts[0] = PHost;
	m_Hosts[1] = AHost;
	m_Hosts[2] = PHost;
	m_Hosts[3] = AHost;
	m_Hosts[4] = PHost;
	m_Hosts[5] = AHost;
	//m_Hosts[6] = DPHost;
	//m_Hosts[7] = DAHost;
	//m_Hosts[8] = DPHost;
	//m_Hosts[9] = DAHost;
	//m_Hosts[10] = DPHost;
	//m_Hosts[11] = DAHost;

	for (int i; i < 6; i++)
	{
		if (Host == m_Hosts[i])
		{
			m_nHost = i;
			break;
		}
	}
}

CActHost::~CActHost()
{

}


CString CActHost::GetHost()
{
	if (m_nHost == 6)
	{
		m_nHost = 0;
	}
	return m_Hosts[m_nHost++];
}
