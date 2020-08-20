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

#pragma warning( disable : 4503 4355 4786 )
#include "stdafx.h"
#include "OMSession.h"
#include "OMMessages.h"

namespace OM
{
    Session* Session::s_pSession;
    Mutex Session::s_mutex;
    int Session::m_cancelChoice = 1;
    
    Session::Session( Application& application, const SessionID& sessionID,
        const UtcTimeOnly& startTime, const UtcTimeOnly& endTime )
        : m_application(application), m_pResponder(0), m_sessionID(sessionID),
        m_startTime(startTime), m_endTime(endTime), m_receivedLogon(false),
        m_sentLogout(false), m_sentLogon(false)
    {
        Locker locker(s_mutex);
        s_pSession = this;
        char pszPath[MAX_PATH];
		GetCurrentDirectory( sizeof(pszPath), pszPath );
        strcat(pszPath,"\\om.cfg");
        m_cancelChoice = GetPrivateProfileInt(
        "SESSION", 
        "CancelChoice", 0, pszPath);
    }
    
    Session::~Session()
    {
        Logout logout;
        send(logout);
        
        Locker locker(s_mutex);
        if( s_pSession == this)
            s_pSession = 0;
    }
    
    void Session::next()
    {
        if( !m_sentLogon )
        {
            generateLogon();
            m_sentLogon = true;
        }
    }
    
    bool Session::send( Message& message )
    {
        std::stringstream stream;
        stream << message;
        
        bool result = m_pResponder->send(stream.str());
        
        if( result )
            m_application.toApp( message, FIX::SessionID(std::string(),std::string(),std::string()) );
        return result;
    }
    
    void Session::disconnect()
    {
        m_sentLogon = false;
    }
    
    
    
    void Session::generateLogon()
    {
        Login login;
        login.SetLoginName(m_sessionID.getSenderCompID());
        login.SetPassword(m_sessionID.getSenderCompID());
        login.SetAutoCancel(m_cancelChoice);
        send(login);
    }
    
    void Session::generateLogout( const std::string& text )
    {
        Logout logout;
        logout.SetLoginName(m_sessionID.getSenderCompID());
        send(logout);
    }
    
    void Session::next( const Message& message )
    {
        switch(message.GetCommand())
        {
        case 'L':
            m_receivedLogon = true;
        default:
            m_application.fromApp( message, m_sessionID );
            break;
        };
    }
    
    bool Session::sendToTarget( Message& message ) throw(SessionNotFound&)
    {
        Locker locker(s_mutex);
        try
        {
            if( !s_pSession ) throw SessionNotFound();
            bool result = s_pSession->send(message);
            return result;
        }
        catch( FieldNotFound& ) { throw SessionNotFound(); }
    }
    
    void Session::SetCancelChoice(int cancelChoice) 
    {
      m_cancelChoice = cancelChoice;
    }
}
