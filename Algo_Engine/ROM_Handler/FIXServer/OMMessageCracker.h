/* -*- C++ -*- */
/* ====================================================================
* The QuickFIX Software License, Version 1.0
*
* Copyright (c) 2001 ThoughtWorks, Inc.  All rights
* reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following conditions
* are met:
*
* 1. Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
* 2. Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in
*    the documentation and/or other materials provided with the
*    distribution.
*
* 3. The end-user documentation included with the redistribution,
*    if any, must include the following acknowledgment:
*       "This product includes software developed by
*        ThoughtWorks, Inc. (http://www.thoughtworks.com/)."
*    Alternately, this acknowledgment may appear in the software itself,
*    if and wherever such third-party acknowledgments normally appear.
*
* 4. The names "QuickFIX" and "ThoughtWorks, Inc." must
*    not be used to endorse or promote products derived from this
*    software without prior written permission. For written
*    permission, please contact quickfix-users@lists.sourceforge.net.
*
* 5. Products derived from this software may not be called "QuickFIX",
*    nor may "QuickFIX" appear in their name, without prior written
*    permission of ThoughtWorks, Inc.
*
* THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESSED OR IMPLIED
* WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
* OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
* DISCLAIMED.  IN NO EVENT SHALL THOUGHTWORKS INC OR
* ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
* USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
* ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
* SUCH DAMAGE.
* ====================================================================
*/

#ifndef OM_MESSAGECRACKER_H
#define OM_MESSAGECRACKER_H

#include "OMMessages.h"
#include "Exceptions.h"

namespace OM
{
class MessageCracker
{
public:
	virtual ~MessageCracker() {}
	virtual void onMessage( const Message& ) 
	{ throw FIX::UnsupportedMessageType(); }
	virtual void onMessage( const Login& )
	{ throw FIX::UnsupportedMessageType(); }
	virtual void onMessage( const EnterOrder& )
	{ throw FIX::UnsupportedMessageType(); }
	virtual void onMessage( const Status& ) 
	{ throw FIX::UnsupportedMessageType(); }
	virtual void onMessage( const Query& )
	{ throw FIX::UnsupportedMessageType(); }
	virtual void onMessage( const Invalid& )
	{ throw FIX::UnsupportedMessageType(); }
	virtual void onMessage( const Logout& )
	{ throw FIX::UnsupportedMessageType(); }

	void crack( const Message& message )
	{

		switch(message.GetCommand())
		{
			case 'L':
				onMessage( Login(message) );
				break;
			case 'E':
				onMessage( EnterOrder(message) );
				break;
			case 'S':
				onMessage( Status(message) );
				break;
			case 'Q':
				onMessage( Query(message) );
				break;
			case 'I':
			case 'K':
				onMessage( Invalid(message) );
				break;
			case 'O':
				onMessage( Logout(message) );
				break;
			default:
				onMessage( message );
		}
	}

  };
}

#endif 

