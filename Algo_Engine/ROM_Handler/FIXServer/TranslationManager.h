#ifndef TRANSLATION_MANAGER_H
#define TRANSLATION_MANAGER_H

#include "FIX42_Messages.h"
#include "OMMessage.h"
#include "OMMessages.h"
#include "SessionID.h"
#include <string>
#include <map>

namespace OM
{
	class Status;
}

class TranslationError : public std::logic_error
{
	public:
		TranslationError()
			: std::logic_error("Error in translation") {}
};

class TranslationManager
{
public:
	~TranslationManager();

	OM::Message Translate( const FIX42::NewOrderSingle& single ) throw( TranslationError );
	OM::Message Translate( const FIX42::OrderCancelRequest& cancel ) throw( TranslationError );
	OM::Message Translate( const FIX42::OrderCancelReplaceRequest& replace ) throw( TranslationError );
	FIX42::Message Translate( const OM::Status& status ) throw( TranslationError );
	static FIX::OrdStatus TranslateStatus( int omStatus );

};

#endif
