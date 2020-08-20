//**************************************************************************
// 
//	File:
//	OrderManagerInterface.cpp
//
//  Summary:
//
//	History
//
//	Name						Reason								Date
//	Randy Fredrick				Created								20-NOV-02
//****************************************************************************

#include "stdafx.h"
#include "iLinkGlobex.h"

void CILinkGlobexApp::InitializeOrderManager(void)
{
	m_pServer = new CServer();

	m_pServer->StartListening();
}