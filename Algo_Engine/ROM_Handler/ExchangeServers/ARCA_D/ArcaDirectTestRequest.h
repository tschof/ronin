/*******************************************************************************
*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
*  : Defines the class behaviors for the application.
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
*     Parnell     
*
*******************************************************************************/
#ifndef ARCADIRECTTESTREQUEST_H__
#define ARCADIRECTTESTREQUEST_H__
#include "stdafx.h"
#include <sstream>
#include "ArcaDirectMessage.h"

/**
 * The Test Request is a short message sent to ArcaDirect
 * that will be sent when we have not gotten responses to
 * heartbeats sent.
 */
class ArcaDirectTestRequest : public ArcaDirectMessage {
    
public:
    ArcaDirectTestRequest() {
        std::stringstream message(std::stringstream::binary
            | std::stringstream::in 
            | std::stringstream::out);
        message.put(TestReq);
        message.put('\0');
        short len = htons(ArcaDirect::TEST_LEN);
        message.write((char*)&len, sizeof(len));
        message.put('\0');
        message.put(ArcaDirect::TERMINATOR);
        
        //Read the length of the message;
        message.seekg(0, std::ios::end);
        int length = message.tellg();
        message.seekg(0, std::ios::beg);
        
        message.read(m_messageStr, ArcaDirect::TEST_LEN);
    }
    
    virtual ~ArcaDirectTestRequest() {}
    
    virtual const char* Message() {
        return m_messageStr;
    }

    virtual int Length() {
        return ArcaDirect::TEST_LEN;
    }
private:
    char m_messageStr[ArcaDirect::TEST_LEN];
};

#endif //ARCADIRECTTESTREQUEST_H__