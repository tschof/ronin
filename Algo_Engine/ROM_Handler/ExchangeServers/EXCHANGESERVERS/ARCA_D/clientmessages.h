/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *  ClientMessages defines a namespace of static methods that create messages
 *  (orders, cancels, replaces, etc ...) to send to the ArcaDirect exchange
 *  interface.  These methods have ARE NOT THREAD SAFE! and take advantage of
 *  the single threaded design.  They do this by limiting mallocs during execution.
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 *     Parnell     
 *
 *******************************************************************************/
#ifndef CLIENTMESSAGES_H__
#define CLIENTMESSAGES_H__

#include <sstream>
#include <iostream>
#include "ArcaDirectMessage.h"
#include "Replace.h"
#include "Cancel.h"
#include "Order.h"


/**
 * Empty namespace for private static variables used in the
 * static functions of namespace ClientMessages.
 */
namespace {

    static char replaceMessage_[ArcaDirect::CANCEL_REP_LEN];
    static char orderMessage_[ArcaDirect::ORDER_LEN];
    static char cancelMessage_[ArcaDirect::CANCEL_LEN];
    static char heartbeatMessage_[ArcaDirect::HEARTB_LEN];
    static char logonMessage_[ArcaDirect::LOGON_LEN];
    static char testRequestMessage_[ArcaDirect::TEST_LEN];

    
    static char m_symbol[ArcaDirect::SYMBOL_LEN];
    static char m_accountBuff[ArcaDirect::ACCOUNT_LEN];

    static std::stringstream message(std::stringstream::binary
                                     | std::stringstream::in 
                                     | std::stringstream::out);
    static const int OUTPUT_MESSAGE_LEN = 200;
    static char outputStr[OUTPUT_MESSAGE_LEN];

    static std::stringstream output(std::stringstream::in 
                                   | std::stringstream::out);

}


/**
 * Namespace ClientMessages defines static variables and functions
 * used to create  New order, Cancel,
 * Cancel Replace, Heartbeat, TestRequest, and Logon ArcaDirect binary
 * messages.
 */
namespace ClientMessages {

    inline const char* GetReplaceMessage() {
        return replaceMessage_;
    }

    inline const char* GetOrderMessage() {
        return orderMessage_;
    }

    inline const char* GetCancelMessage() {
        return cancelMessage_;
    }

    inline const char* GetHBMessage() {
        return heartbeatMessage_;
    }

    inline const char* GetLogonMessage() {
        return logonMessage_;
    }
    inline const char* GetTestReqMessage() {
        return testRequestMessage_;
    }
    
    inline void CreateReplaceMessage(CReplace& order) {
        output <<" REPLACE MESSAGE: ";            
        message.put(ArcaDirectMessage::CancelReplace);
        message.put('\0');
        //Set the predefined length of all ArcaDirect orders.
        short len = htons(ArcaDirect::CANCEL_REP_LEN);
        message.write((char*)&len, sizeof(len));
        //set sequence
        long seq = htonl(order.Sequence());
        message.write((char*)&seq, sizeof(seq));
        output <<" Sequence Num: "<<order.Sequence()<<" ";
        /*The working order id at the exchange is always in the cancel tag.*/
        /*Setting a unique value for the ID of the new order replacing the old order.*/   
        int clientID = htonl(atoi(order.lpszCancelTag()));
        message.write((char*)&clientID, sizeof(clientID));
        output <<" Client ID: "<<order.lpszCancelTag()<<" ";
        /*Original client ID. The orderID of the orginal order that we are canceling 
          will be set in the order tag and the new order id will be set in the cancel
          field.*/
        int hOrigID = atoi(order.lpszOrderTag());
        int origID = htonl(hOrigID);
        message.write((char*)&origID, sizeof(origID));
        output <<" Original ORder Tag: "<<hOrigID<<" ";
        /*ARCA ISSUED ORDER ID. This is the arca issued order id that came back with 
          the orginal order ack.*/
        int hArcaID = atoi(order.lpszExchangeTag());
        int arcaID = htonl(hArcaID);
        message.write((char*)&arcaID, sizeof(arcaID));
        output <<" ARCA ID: "<<hArcaID<<" ";
        /*Set the number of shares.*/
        long numShares = htonl(order.Shares());
        message.write((char*)&numShares, sizeof(numShares));
        output <<" Num Shares: "<<order.Shares()<<" ";
        //Set price and price scale
        ArcaDirect::SetPriceFields(message, order.Price());
        output <<" PRICE: "<<order.Price()<<" ";
        /* This is the Deliver To CompID, in other words the ID of the 
           market maker to route a Directed order to. Since we do not
           currently support this we send in a NULL string.*/
        message.write(ArcaDirect::DELIVER_TO_COMP_ID,
                      ArcaDirect::DELIVER_TO_COMP_ID_LEN);

        /*Set the Symbol. CReplace saves it's Symbol information
          in a variable length
          CString so we have to be careful when extracting it.
          We create our char array since we need
          to null pad out any value to the ArcaDirect::SYMBOL_LEN.*/
        memset(m_symbol, '\0', ArcaDirect::SYMBOL_LEN);
        int ordSymbLen = order.Symbol().GetLength();
        int cpyLen = ordSymbLen < ArcaDirect::SYMBOL_LEN ?
            ordSymbLen : ArcaDirect::SYMBOL_LEN;
        strncpy(m_symbol, ((const char*)order.Symbol()), cpyLen);
        message.write(m_symbol, ArcaDirect::SYMBOL_LEN);  
        output <<" Symbol: "<<order.Symbol()<<" ";
        //Set execution instructions.
        if(order.ExecInst() && order.ExecInst() != COrder::LastExecInst) {
            message.put(order.ExecInst());
        } else {
            message.put('\0');
        }
     
        message.put(ArcaDirectMessage::ToExchangeSide(order.Side()));
    
        message.put(ArcaDirectMessage::ToExchangeType(order.Type()));
    
        message.put(ArcaDirectMessage::ToExchangeTIF(order.TIF())); 
        /*Set Rule80A*/  
        message.put(order.Capacity());

        /*Most orders get the all day Trading session.  ArcaDirect will not
          allow market orders to be placed in any sessions except the core.*/
        if(order.Type() != COrder::Market) {
            message.write(ArcaDirect::TRADE_SES_ID,
                          ArcaDirect::TRADING_SES_ID_LEN);
        } else {
            message.write(ArcaDirect::MARKET_TRADE_SES_ID, ArcaDirect::TRADING_SES_ID_LEN);
        }
        
        /*Setting the clearing ID in the Account field.*/
        const char* localClear = order.lpszClearingId();
        memset(m_accountBuff, '\0', ArcaDirect::ACCOUNT_LEN);
        if(localClear && strlen(localClear) < ArcaDirect::ACCOUNT_LEN) {
            strcpy(m_accountBuff, localClear);
        } else if(localClear){
            strncpy(m_accountBuff, localClear, ArcaDirect::ACCOUNT_LEN);       
        }
        message.write(m_accountBuff, ArcaDirect::ACCOUNT_LEN);
        message.put(ArcaDirect::TERMINATOR);
    
        message.read(replaceMessage_, ArcaDirect::CANCEL_REP_LEN);
        message.seekg(0, std::ios::beg);
        message.seekp(0, std::ios::beg);

        memset(outputStr, '\0', OUTPUT_MESSAGE_LEN);
        int tellp = output.tellp();
        int outputMessLen = tellp <= OUTPUT_MESSAGE_LEN ? tellp : OUTPUT_MESSAGE_LEN;
        output.read(outputStr, outputMessLen);
        ((CMessageApp*)AfxGetApp())->ProcessTraffic(
            "ARCADIRECT Created replaced. ", output.str().c_str());
        output.seekg(0, std::ios::beg);
        output.seekp(0, std::ios::beg);
    }

    inline void CreateOrderMessage(COrder& order) {
        output<<" NEW ORDER "<<ArcaDirectMessage::NewOrder<<" length "<<ArcaDirect::ORDER_LEN<<" Sequence "<<order.Sequence()<<" " ; 
        message.put(ArcaDirectMessage::NewOrder);
        message.put('\0');
        //Set the predefined length of all ArcaDirect orders.
        short len = htons(ArcaDirect::ORDER_LEN);
        message.write((char*)&len, sizeof(len));
        //set sequence
        long seq = htonl(order.Sequence());
        message.write((char*)&seq, sizeof(seq));
        //set client ID from the order tag which is set in the handler.
        output<<" Order ID "<<order.lpszOrderTag()<<" ";
        int clientID = htonl(atoi(order.lpszOrderTag()));
        message.write((char*)&clientID, sizeof(clientID));
        /*This field is for the common basket ID for all orders
          beloning in a portfolio cross.  We do not support this at
          this time and so a 0 will be inserted here.*/
        message.put('\0');
        message.put('\0');
        message.put('\0');
        message.put('\0');
        /*Set the number of shares.*/
        long numShares = htonl(order.Shares());
        message.write((char*)&numShares, sizeof(numShares));
        output<<" Shares "<<order.Shares()<<" Price "<<order.dPrice()<<" Symbol "<<order.lpszSymbol()<<" ";
        //Set price and price scale
        ArcaDirect::SetPriceFields(message, order.dPrice());
        /*Set the Symbol. We "know" that COrder stores it's Symbol
          in a 36 char array.  So we just grab the first 
          5 chars and set them as our symbol.*/
        const char* symb = order.lpszSymbol();
        memset(m_symbol, '\0', ArcaDirect::SYMBOL_LEN);
        if(symb) {
            int symlen = strlen(symb);
            int copyLen = symlen >= ArcaDirect::SYMBOL_LEN ?
                ArcaDirect::SYMBOL_LEN : symlen;
            strncpy(m_symbol, symb, copyLen);
        }
        message.write(m_symbol, ArcaDirect::SYMBOL_LEN);
        output<<"SYMBOL "<<m_symbol<<" ";
        //Set the company ID. This value is in the dat file.
        message.write(ArcaDirectMessage::GetCompanyGroupID(),
                      ArcaDirect::COMP_GROUP_ID_LEN);

        /* This is the Deliver To CompID, in other words the ID of the 
           market maker to route a Directed order to. Since we do not
           currently support this we send in a NULL string.*/
        message.write(ArcaDirect::DELIVER_TO_COMP_ID,
                      ArcaDirect::DELIVER_TO_COMP_ID_LEN);
        //Set execution instructions.
        if(order.ExecInst() && order.ExecInst() != COrder::LastExecInst) {
            message.put(order.ExecInst());
        } else {
            message.put('\0');
        }
    
        message.put(ArcaDirectMessage::ToExchangeSide(order.Side()));

        message.put(ArcaDirectMessage::ToExchangeType(order.Type()));

        message.put(ArcaDirectMessage::ToExchangeTIF(order.TIF())); 
        /*Set Rule80A*/  
        message.put(order.Capacity());
        /*Most orders get the all day Trading session.  ArcaDirect will not
          allow market orders to be placed in any sessions except the core.*/
        if(order.Type() != COrder::Market) {
            message.write(ArcaDirect::TRADE_SES_ID,
                          ArcaDirect::TRADING_SES_ID_LEN);
        } else {
            message.write(ArcaDirect::MARKET_TRADE_SES_ID, ArcaDirect::TRADING_SES_ID_LEN);
        }
        output<<""<<order.Side()<<" TYPE "<<order.Type()<<" TIF "<<order.TIF()<<" Capacity "<<order.Capacity()<<" ";
        /*Setting the clearing ID in the Account field.*/
        const char* localClear = order.lpszClearingId();
        memset(m_accountBuff, '\0', ArcaDirect::ACCOUNT_LEN);
        if(localClear && strlen(localClear) < ArcaDirect::ACCOUNT_LEN) {
            strcpy(m_accountBuff, localClear);
        } else if(localClear){
            strncpy(m_accountBuff, localClear, ArcaDirect::ACCOUNT_LEN);       
        }
        message.write(m_accountBuff, ArcaDirect::ACCOUNT_LEN);
        /*Required Padding*/
        message.put('\0');
        message.put('\0');
        message.put('\0');
        message.put(ArcaDirect::TERMINATOR);

        message.read(orderMessage_, ArcaDirect::ORDER_LEN);
        message.seekg(0, std::ios::beg);
        message.seekp(0, std::ios::beg);

        memset(outputStr, '\0', OUTPUT_MESSAGE_LEN);
        int tellp = output.tellp();
        int outputMessLen = tellp <= OUTPUT_MESSAGE_LEN ? tellp : OUTPUT_MESSAGE_LEN;
        output.read(outputStr, outputMessLen);    
        ((CMessageApp*)AfxGetApp())->ProcessTraffic(
            "ArcaDirect New Order: ", output.str().c_str());
        output.seekg(0, std::ios::beg);
        output.seekp(0, std::ios::beg);

    }

    inline void CreateCancelMessage(CCancel& order) {
        message.put(ArcaDirectMessage::CancelRequest);
        message.put('\0');
        //Set the predefined length of all ArcaDirect orders.
        short len = htons(ArcaDirect::CANCEL_LEN);
        message.write((char*)&len, sizeof(len));
        //set sequence
        long seq = htonl(order.Sequence());
        message.write((char*)&seq, sizeof(seq));
        output<<" Sequence Num "<<order.Sequence()<<" ";

        /* The ID of the order to be cancelled will be 
           set as the orderID and passed on to Arca. */
        int hClientID = atoi(order.lpszOrderTag());
        output<<" ClientID "<<hClientID<<" ";
        int clientID = htonl(hClientID);
        message.write((char*)&clientID, sizeof(clientID));
        /*ARCA ISSUED ORDER ID: This is the id we get out of the
          order ack sent back to us from arca when the order was placed.*/
        int hArcaID = atoi(order.lpszExchangeTag());
        output<<" ARCA ID "<<hArcaID<<" ";
        int arcaID = htonl(hArcaID);
        message.write((char*)&arcaID, sizeof(arcaID));

        /* This is the Deliver To CompID, in other words the ID of the 
           market maker to route a Directed order to. Since we do not
           currently support this we send in a NULL string.*/
        char deliverToCompid[ArcaDirect::DELIVER_TO_COMP_ID_LEN];
        memset(deliverToCompid, '\0', ArcaDirect::DELIVER_TO_COMP_ID_LEN);
        message.write(deliverToCompid, ArcaDirect::DELIVER_TO_COMP_ID_LEN);

        /*Set the Symbol. CReplace saves it's Symbol information in a variable length
          CString so we have to be careful when extracting it. We create our char array since we need
          to null pad out any value to the ArcaDirect::SYMBOL_LEN.*/       
        memset(m_symbol, '\0', ArcaDirect::SYMBOL_LEN);
        int ordSymbLen = order.Symbol().GetLength();
        int cpyLen = ordSymbLen < ArcaDirect::SYMBOL_LEN ? ordSymbLen : ArcaDirect::SYMBOL_LEN;
        strncpy(m_symbol, ((const char*)order.Symbol()), cpyLen);
        message.write(m_symbol, ArcaDirect::SYMBOL_LEN);  
        output<<" SYMBOL "<<m_symbol<<" ";
        /*Set the side.*/
        char side = ArcaDirectMessage::ToExchangeSide(order.Side());
        output<<" SIDE "<<side<<" ";
        message.put(side);
        /*Add padding*/
        message.put('\0');
        /*Append a message terminator.*/
        message.put(ArcaDirect::TERMINATOR);
    
        message.read(cancelMessage_, ArcaDirect::CANCEL_LEN);
        message.seekg(0, std::ios::beg);
        message.seekp(0, std::ios::beg);

        memset(outputStr, '\0', OUTPUT_MESSAGE_LEN);
        int tellp = output.tellp();
        int outputMessLen = tellp <= OUTPUT_MESSAGE_LEN ? tellp : OUTPUT_MESSAGE_LEN;
        output.read(outputStr, outputMessLen);    
        ((CMessageApp*)AfxGetApp())->ProcessTraffic(
            "ArcaDirect CANCEL: ", output.str().c_str());
        output.seekg(0, std::ios::beg);
        output.seekp(0, std::ios::beg);
    }

    /**
     * The ArcaDirectHeartbeat message is sent in response to
     * a test request from the Exchange and/or after 30 seconds
     * of inactivity.
     */
    inline void CreateHeartbeatMessage() {
        message.put(ArcaDirectMessage::HeartBeat);
        message.put('\0');
        short len = htons(ArcaDirect::HEARTB_LEN);
        message.write((char*)&len, sizeof(len));
        message.put('\0');
        message.put(ArcaDirect::TERMINATOR);
        
        message.read(heartbeatMessage_, ArcaDirect::HEARTB_LEN);
        message.seekg(0, std::ios::beg);
        message.seekp(0, std::ios::beg);
    }
    /**
     * The Test Request is a short message sent to ArcaDirect
     * that will be sent when we have not gotten responses to
     * heartbeats sent.
     */
    inline void CreateTestRequestMessage() {
        message.put(ArcaDirectMessage::TestReq);
        message.put('\0');
        short len = htons(ArcaDirect::TEST_LEN);
        message.write((char*)&len, sizeof(len));
        message.put('\0');
        message.put(ArcaDirect::TERMINATOR);
        
        message.read(testRequestMessage_, ArcaDirect::TEST_LEN);
        message.seekg(0, std::ios::beg);
        message.seekp(0, std::ios::beg);
    }
    
    inline void CreateLogonMessage(const char* username, int sequence) {
        message.put(ArcaDirectMessage::Login);
        message.put('\0');
        short len = htons(ArcaDirect::LOGON_LEN);
        message.write((char*)&len, sizeof(len));
        long seq = htonl(sequence);
        message.write((char*)&seq, sizeof(seq));
        message.write(username, ArcaDirect::USER_NAME_LEN);
        message.put('\0');
        message.put('\0');
        message.put(ArcaDirect::TERMINATOR);

        message.read(logonMessage_, ArcaDirect::LOGON_LEN);
        message.seekg(0, std::ios::beg);
        message.seekp(0, std::ios::beg);
    }
}

#endif //CLIENTMESSAGES_H__
