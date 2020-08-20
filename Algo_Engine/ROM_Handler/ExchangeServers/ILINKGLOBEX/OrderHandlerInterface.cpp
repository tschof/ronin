//**************************************************************************
// 
//	File:
//	OrderHandlerInterface.cpp
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
#include "OrderHandler.h"

void CILinkGlobexApp::InitializeOrderHandler(void)
{
	CMessageApp::m_pHandler	= new CILinkGlobexOrderHandler();
}