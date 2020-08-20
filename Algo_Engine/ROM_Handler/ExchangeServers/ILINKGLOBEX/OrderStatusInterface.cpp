//**************************************************************************
// 
//	File:
//	
//  Summary:
//
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#include "stdafx.h"
#include "iLinkGlobex.h"
#include "GlobexStatusHandler.h"

void CILinkGlobexApp::InitializeOrderStatus(void)
{
	m_pStatus = new CGlobexStatusHandler;
}
