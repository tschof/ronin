// OMOrder.cpp: implementation of the OMOrder class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "TradeClient.h"
#include "OMOrder.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
namespace FIX42
{

OMOrder::OMOrder() : NewOrderSingle()
{
	m_Status = FIX::OrdStatus_NEW;
}

OMOrder::~OMOrder()
{

}

void OMOrder::UpdateStatus( const ExecutionReport & report )
{

}

}