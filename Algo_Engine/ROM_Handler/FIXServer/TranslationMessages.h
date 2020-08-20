#ifndef TRANSLATION_MESSAGES_H
#define TRANSLATION_MESSAGES_H

#include "OMMessages.h"
#include "FIX42_Messages.h"

class EnterOrderFromFIX : public OM::EnterOrder
{
	public:
		EnterOrderFromFIX( const FIX42::NewOrderSingle& single );
};

class CancelOrderFromFIX : public OM::CancelOrder
{
	public:
		CancelOrderFromFIX( const FIX42::OrderCancelRequest& cancel );
};

class ReplaceOrderFromFIX : public OM::ReplaceOrder
{
	public:
		ReplaceOrderFromFIX( const FIX42::OrderCancelReplaceRequest& replace );
};

class ExecutionReportFromOM : public FIX42::ExecutionReport
{
	public:
		ExecutionReportFromOM( const OM::Status& status );
	private:
		std::string NextExecID();
};

class OrderCancelRejectFromOM : public FIX42::OrderCancelReject
{
	public:
		OrderCancelRejectFromOM( const OM::Status& status );
};

#endif
