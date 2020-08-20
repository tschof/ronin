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

#ifndef OM_SESSION_H
#define OM_SESSION_H

#include "Mutex.h"
#include "OMApplication.h"
#include "SessionID.h"

namespace FIX
{
	class UtcTimeOnly;
	class UtcTimeStamp;
	class SendingTime;
	struct SessionNotFound;
}

using namespace FIX;

namespace OM
{
  class Message;

  /// Maintains the state and implements the logic of a %OM% session.
  class Session
  {
  public:
    class Responder;

    Session( Application&, const SessionID&, const UtcTimeOnly& startTime, const UtcTimeOnly &endTime );
    ~Session();

    bool sentLogon()     { return m_sentLogon;                    }
    bool sentLogout()    { return m_sentLogout;                   }
    bool receivedLogon() { return m_receivedLogon;                }
    bool isLoggedOn()    { return receivedLogon() && sentLogon(); }
    void reset()         
	{ 
		disconnect(); 
	}

    const SessionID& getSessionID() const { return m_sessionID; }
    void setResponder(Responder* pR) { m_pResponder = pR; }


    static void SetCancelChoice(int cancelChoice);
    static bool sendToTarget( Message& )
      throw(SessionNotFound&);
    static bool isSessionTime(const UtcTimeOnly& start, const UtcTimeOnly& end,
                              const UtcTimeStamp& time );
   
    bool send( Message& );
    void next();
    void next( const Message& );
    void disconnect();

  private:
    bool send(const std::string message)
    { if(m_pResponder) return m_pResponder->send(message); return false; }
    
    bool isGoodTime(const SendingTime& sendingTime)
    {
      UtcTimeStamp now;
      return abs(now - sendingTime) <= 120;
    }

    void doBadTime(const Message& msg);
    
    void generateLogon();
    void generateLogout( const std::string& text = "" );

    Application& m_application;
    Responder* m_pResponder;
    SessionID m_sessionID;
    UtcTimeOnly m_startTime;
    UtcTimeOnly m_endTime;

    bool m_receivedLogon;
    bool m_sentLogout;
    bool m_sentLogon;
    
    static int m_cancelChoice;
    static Session* s_pSession;
    static Mutex s_mutex;

  public:
    /// Interface implements sending on and disconnecting a transport.
    class Responder
    {
      friend class Session;
    public:
      virtual ~Responder() {}
      virtual bool send( const std::string& ) = 0;
      virtual void disconnect() = 0;
      virtual void connect() {}
    };
  };
}

#endif 
