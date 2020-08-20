#include "stdafx.h"
#include "TranslationManager.h"
#include "TranslationMessages.h"
#include "Values.h"
#include "Log.h"

TranslationManager::~TranslationManager()
{
}

OM::Message TranslationManager::Translate( const FIX42::NewOrderSingle& single ) throw( TranslationError )
{
	return EnterOrderFromFIX(single);
}

OM::Message TranslationManager::Translate( const FIX42::OrderCancelRequest& cancel ) throw( TranslationError )
{
	return CancelOrderFromFIX(cancel);
}

OM::Message TranslationManager::Translate( const FIX42::OrderCancelReplaceRequest& replace ) throw( TranslationError )
{
	return ReplaceOrderFromFIX(replace);
}

FIX42::Message TranslationManager::Translate( const OM::Status& status ) throw( TranslationError )
{
	try
	{
		switch( status.GetStatus() )
		{
			case OM::Status_CancelRejected:
			case OM::Status_ReplaceRejected:
				return OrderCancelRejectFromOM(status);
			default:
				return ExecutionReportFromOM(status);
		}
	}
	catch(...)
	{
		Log::Write(LOG_ERROR, "Translate Error: "+status.getString());
		throw TranslationError();
	}
}

FIX::OrdStatus TranslationManager::TranslateStatus( int omStatus )
{
	switch( omStatus )
	{
		case OM::Status_New: // = 0,
			return FIX::OrdStatus_PENDING_NEW;
		case OM::Status_PartiallyFilled: // = 1,
			return FIX::OrdStatus_PARTIALLY_FILLED;
		case OM::Status_Filled: // = 2,
			return FIX::OrdStatus_FILLED;
		case OM::Status_DoneForDay: // = 3,
			return FIX::OrdStatus_DONE_FOR_DAY;
		case OM::Status_Cancelled: // = 4,
			return FIX::OrdStatus_CANCELED;
		case OM::Status_Replaced: // = 5,
			return FIX::OrdStatus_REPLACED;
		case OM::Status_PendingCancel: // = 6,
			return FIX::OrdStatus_PENDING_CANCEL;
		case OM::Status_Stopped: // = 7,
			return FIX::OrdStatus_STOPPED;
		case OM::Status_Rejected: // = 8,
			return FIX::OrdStatus_REJECTED;
		case OM::Status_Suspended: // = 9,
			return FIX::OrdStatus_SUSPENDED;
		case OM::Status_PendingNew: // = 10,
			return FIX::OrdStatus_PENDING_NEW; // A
		case OM::Status_Calculated: // = 11,
			return FIX::OrdStatus_CALCULATED;
		case OM::Status_Expired: // = 12,
			return FIX::OrdStatus_EXPIRED;
		case OM::Status_Open: // = 13,
			return FIX::OrdStatus_NEW; // 0
		case OM::Status_ReplacePending: // = 21,
			return FIX::OrdStatus_PENDING_REPLACE;
		case OM::Status_CancelRejected: // = 14,
			return FIX::OrdStatus_REJECTED;
		case OM::Status_ReplaceRejected: // = 20,
			return FIX::OrdStatus_REJECTED;
		case OM::Status_Corrected: // = 15,
		case OM::Status_Busted: // = 16,
		default:
			{
				std::stringstream stm;
				stm << "Invalid OM status: " << omStatus;
				Log::Write( LOG_ERROR, stm.str() );
				Log::Write( LOG_DEBUG, stm.str() );
			}
			return FIX::OrdStatus_REJECTED; // 0
	}
}
