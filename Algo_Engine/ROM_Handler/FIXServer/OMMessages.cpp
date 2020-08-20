
#include "stdafx.h"
#include "OMMessages.h"
#include <time.h>
namespace OM
{
	Login::Login()
		: Message( 'L', 2 )
	{

		time_t t;
		time(&t);
		SetTimestamp(t);

	}

	Login::Login( const Message& message )
		: Message( message )
	{

	}

	EnterOrder::EnterOrder()
		: Message( 'E', 33 )
	{

		time_t t;
		time(&t);
		SetTimestamp(t);

	}

	EnterOrder::EnterOrder( const Message& message )
		: Message( message )
	{

	}

	CancelOrder::CancelOrder()
		: Message( 'C', 28 )
	{

		time_t t;
		time(&t);
		SetTimestamp(t);

	}

	CancelOrder::CancelOrder( const Message& message )
		: Message( message )
	{

	}

	ReplaceOrder::ReplaceOrder()
		: Message( 'R', 28 )
	{

		time_t t;
		time(&t);
		SetTimestamp(t);

	}

	ReplaceOrder::ReplaceOrder( const Message& message )
		: Message( message )
	{

	}

	Status::Status()
		: Message( 'S', 28 )
	{

		time_t t;
		time(&t);
		SetTimestamp(t);

	}

	Status::Status( const Message& message )
		: Message( message )
	{

	}

	Invalid::Invalid()
		: Message( 'I', 26 )
	{

		time_t t;
		time(&t);
		SetTimestamp(t);

	}

	Invalid::Invalid( const Message& message )
		: Message( message )
	{

	}

	Query::Query()
		: Message( 'Q', 27 )
	{

		time_t t;
		time(&t);
		SetTimestamp(t);

	}

	Query::Query( const Message& message )
		: Message( message )
	{

	}

	Logout::Logout()
		: Message( 'O', 2 )
	{

		time_t t;
		time(&t);
		SetTimestamp(t);

	}

	Logout::Logout( const Message& message )
		: Message( message )
	{

	}


}

