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
#ifndef IncomingTranslator_H__
#define IncomingTranslator_H__

#include <sstream>
#include <string>
#include <cmath>
#include "stdafx.h"
#include "Order.h"
#include "Execution.h"

#include "Translator.h"
#include "MessageApp.h"


class IncomingTranslator {
private:
    static CTranslator m_Translator;
    static CString m_companyGroupID;
       
protected:  
    
public:
    enum MessageTypes {
        Login = 'A',
        LoginReject = 'I',
        TestReq = '1',
        HeartBeat = '0',
        NewOrder = 'D',
        CancelRequest = 'F',
        CancelReplace = 'G',
        OrderAck = 'a',
        OrderFill = '2',
        CancelAck = '6',
        OrderKilled = '4',
        ReplaceAck = 'E',
        ReplacedMess = '5',
        Reject = '8',
        BustOrCorrect = 'C'
    };
    
    enum Side {
        Buy = '1',
        Sell = '2',
        SellShort = '5',
        SellShortEx = '6',
        Cross = '8',
        CrossShort = '9',
        CrossShortEx = 'A'
    };
    
    enum OrderType {
        Market = '1',
        Limit = '2',
        InsideLimit = '7'
    };
    
    enum TimeInForce {
        DayOrder = '0',
        IOC = '3',
        EarlyAuctionOnly = '2',
        LateAuctionOnly = '7'
    };
    
    enum ExInstructions {
        PNP = '6',
        NOW = '1'
    };
    
    enum TradingSessionID {
        SessionOne = '1',
        SessionTwo = '2',
        SessionThree = '3'
    };
    
    enum ArcaRule80A {
        Agent = 'A',
        Principal = 'P',
        EField = 'E'
    };
    static void	Init();
    
    // translation of exchange codes to stafford codes
    static const int ToStaffordSide(char ExchangeCode);
    // translation of stafford codes to exchange codes
    static const char ToExchangeSide(int StaffordCode);
    
    // translation of exchange codes to stafford codes
    static const int ToStaffordType(char ExchangeCode);
    // translation of stafford codes to exchange codes
    static const char ToExchangeType(int StaffordCode);
    
    // translation of exchange codes to stafford codes
    static const int ToStaffordTIF(char ExchangeCode);
    // translation of stafford codes to exchange codes
    static const char ToExchangeTIF(int StaffordCode);

    static const char* GetCompanyGroupID();
    
    IncomingTranslator();
    
    virtual ~IncomingTranslator() {}
    
};


/**
 * Namespace defined constants and utility functions
 * for parsing messages sent from ArcaDirect.
 */
namespace ArcaDirect {
    static const short ORDER_LEN = 60;
    static const short LOGON_LEN = 16;
    static const short LOGON_REJECT_LEN = 66;
    static const short TEST_LEN = 6;
    static const short HEARTB_LEN = 6;
    static const short CANCEL_LEN = 32;
    static const short CANCEL_REP_LEN = 56;
    static const short ORDER_ACK_LEN = 24;
    static const short ORDER_FILL_LEN = 36;
    static const short CANCEL_ACK_LEN = 24;
    static const short ORDER_KILLED_LEN = 24;
    static const short REPLACE_ACK_LEN = 24;
    static const short REPLACED_MESSAGE_LEN = 24;
    static const short REJECT_LEN = 64;
    static const short BUST_CORRECT_LEN = 32;
    static const int SYMBOL_LEN = 8;
    static const int COMP_GROUP_ID_LEN = 5;
    static const int TRADING_SES_ID_LEN = 4;
    static const int ACCOUNT_LEN = 4;
    static const int USER_NAME_LEN = 5;
    static const int DELIVER_TO_COMP_ID_LEN = 5;
    static const char TERMINATOR = '\n';
    static const int ID_STRING_LEN = 12;
    static const char TRADE_SES_ID[TRADING_SES_ID_LEN]= {'1', '2', '3', '\0'};
    static const char MARKET_TRADE_SES_ID[TRADING_SES_ID_LEN] = {'2', '\0', '\0','\0'};
    static const char DELIVER_TO_COMP_ID[DELIVER_TO_COMP_ID_LEN] =
    {'\0', '\0', '\0', '\0', '\0'};
    static const int REJECT_REASON_LEN = 40;
    static const int OUTPUT_MESS_LEN = 200;
    static const int REMAIN_BUFF_LEN = 6;
    static const int PRICE_BUFFER_LEN = 12;

    namespace {
        static char outputString[OUTPUT_MESS_LEN];
        static char priceFieldBuffer[PRICE_BUFFER_LEN];
        static  std::stringstream outStream(std::stringstream::in 
                                            | std::stringstream::out); 
    }
    
    /**
     * ArcaDirect breaks up a price into two message fields.
     * Since prices are doubles the original (double) price
     * is multiplied by the power of 10 that is appropriate 
     * to preserve the precision of that price(up to 6 decimal
     * places).  The integer that results from this operation
     * is then inserted (in binary Big Endian form) into the
     * message. The power of 10 is stored as a charater 0 - 6.
     */
    inline void SetPriceFields(std::stringstream& message,
                               double price) {
        double ndPrice;
        double remainder = modf(price, &ndPrice);
        if(remainder != 0) {
            char* buffer;
            int dec, sign;
            buffer = fcvt(remainder, REMAIN_BUFF_LEN, &dec, &sign);
            int pos = strspn(buffer, "0");
            int i = pos;
            int len = strlen(buffer);
            while (i < len) {
                if(buffer[i] != '0') {
                    pos = i;
                }
                ++i;
            }
            ++pos;
            double exponentVal = pow(10, pos);
            char precision[2];
            memset(precision, '\0', 2);
            itoa(pos, precision, 10);
            unsigned int fPrice = htonl(
                (static_cast<unsigned int>((price * exponentVal))));
            message.write((char*)&fPrice, sizeof(fPrice));
            message.put(precision[0]);
        } else {
            int finalPrice = htonl(static_cast<int>(price));
            message.write((char*)&finalPrice, sizeof(finalPrice));
            message.put('0');
        }
    }
    /**
     * This function takes allows parsers of messages from ArcaDirect
     * to send in the price and scale fields and get back the double
     * price which they refer to.
     */
    inline double GetPriceFromFields(unsigned int price, char scale) {
        double dScale = 0;
        switch(scale) {
        case '0':
            break;
        case '1':
            dScale = 10;
            break;
        case '2':
            dScale = 100;
            break;
        case '3':
            dScale = 1000;
            break;
        case '4':
            dScale = 10000;
            break;
        case '5':
            dScale = 100000;
            break;
        case '6':
            dScale =1000000;
            break;
        }
        double val = static_cast<double>(price);
        if(dScale > 0) {
            val /= dScale;
        }
        return val;
    }
    /**
     * Utility funcion for processing Ack, Cancelled and 
     * Canceled/Replaced messages.
     * \return The Arca-assigned sequence number of the Message.
     *         This number is tracked in the NextIncommingSeq field
     *         of the session and is used in case of Disconnect.
     */    
    inline int ProcessMessage(std::stringstream& stream,
                              CExecution& exe,
                              char* idStr, char* arcaIDStr) {
        char type;
        stream.get(type);
        stream.ignore(3);
        int seq;
        stream.read((char*)&seq, sizeof(seq));
        seq = ntohl(seq);
        outStream <<"MESSAGE TYPE "<<type<<" Sequence Num: "<<seq<<" ";
        stream.ignore(4);
        int clientID;
        stream.read((char*)&clientID, sizeof(clientID));
        clientID = ntohl(clientID);
        itoa(clientID, idStr, 10);
        /*Set the order ID in the OrderTag field of the message.*/
        if(exe.Status() == COrder::Replaced ||
           exe.Status() == COrder::ReplacePending) {
            exe.SetCancelTag(idStr);
        } else {
            exe.SetOrderTag(idStr);
        }
        outStream <<" CLIENT ID: "<<clientID<<" ";

        int arcaID;
        stream.read((char*)&arcaID, sizeof(arcaID));
        arcaID = ntohl(arcaID);
        itoa(arcaID, arcaIDStr, 10);
        /*Set the Exchange ID in the OrderTag field of the message.*/
        exe.SetExchangeTag(arcaIDStr);
        outStream <<" EXCHANGE TAG: "<<arcaID<<" ";
        stream.ignore(4);
        memset(outputString, '\0', OUTPUT_MESS_LEN);
        outStream.read(outputString, outStream.tellp());
        ((CMessageApp*)AfxGetApp())->ProcessTraffic(
            "ARCADIRECT Process MESSAGE= ", outputString);
        outStream.seekg(0, std::ios::beg);
        outStream.seekp(0, std::ios::beg);
        return seq;
    }
    /**
     * Utility function for processing all types of Reject message.
     */
    inline void ProcessRejectMessage(std::stringstream& stream,
                                     CMessageBase& exe,
                                     char* idStr,
                                     char* arcaIDStr,
                                     char* reason,
                                     char messType) {
        stream.ignore(12);

        int clientID;
        stream.read((char*)&clientID, sizeof(clientID));
        clientID = ntohl(clientID);
        itoa(clientID, idStr, 10);
        /*Set the order ID in the OrderTag field of the message.*/
        if(messType == '1') {
            exe.SetOrderTag(idStr);
        } else {
            exe.SetCancelTag(idStr);
        }
        outStream <<"REJECT TYPE "<<messType<<" CLIENT ID: "<<clientID<<" ";

        if( messType == '3') {
            int origID;
            stream.read((char*)&origID, sizeof(origID));
            origID = ntohl(origID);
            itoa(origID, arcaIDStr, 10);
            /*This is the original orderID so set it in the OrderTag field of the message.*/       
            exe.SetOrderTag(arcaIDStr);
            outStream<<" Original Order ID "<<origID<<" ";
        } else {
            stream.ignore(4);
        }
        char type;
        stream.read(&type, sizeof(char));
        stream.read(reason, REJECT_REASON_LEN);
        outStream<<"Reject Reason: "<<reason<<" ";
        stream.ignore(3);
        memset(outputString, '\0', OUTPUT_MESS_LEN);
        outStream.read(outputString, outStream.tellp());
        ((CMessageApp*)AfxGetApp())->ProcessTraffic(
            "ArcaDirect Order Rejected: ", outputString);
        outStream.seekg(0, std::ios::beg);
        outStream.seekp(0, std::ios::beg);
    }
    /**
     * Utility function for parsing order filled messages.
     * Order filled messages can either be for partially or
     * fully filled orders.
     * \return the Arca-assigned sequence number of the message.
     */
    inline int ProcessOrderFilledMessage(std::stringstream& stream,//const char* message,
                                         CExecution& exe,
                                         char* idStr,
                                         char* arcaIDStr) {
        /*We ignore the first 4 bytes since they are not used by 
          ROM. They are the MessageType, filler, Length of message.*/
        stream.ignore(4);
        int seq;
        stream.read((char*)&seq, sizeof(seq));
        seq = ntohl(seq);
        outStream <<" Sequence Num: "<<seq<<" ";
        /*Extract the time of the transaction.*/
        stream.ignore(4);
        /*The ID of the original order.*/
        int clientID;
        stream.read((char*)&clientID, sizeof(clientID));  
        clientID = ntohl(clientID);
        itoa(clientID, idStr, 10);
        /*Set the order ID in the OrderTag field of the message.*/
        exe.SetOrderTag(idStr);
        outStream<<" ClientID: "<<clientID<<" ";
         
        int arcaID;
        stream.read((char*)&arcaID, sizeof(arcaID));
        arcaID = ntohl(arcaID);
        itoa(arcaID, arcaIDStr, 10);
        /*Set the Exchange ID in the OrderTag field of the message.*/
        exe.SetExchangeTag(arcaIDStr);
        outStream<<" ARCAID: "<<arcaID<<" ";
        
        int execID;
        stream.read((char*)&execID, sizeof(execID));
        execID = ntohl(execID);
        /*We set the Execution ID from arca into the trade tag field.*/
        exe.SetTradeTag(execID);
        outStream<<" EXECUTION ID: "<<clientID<<" ";

        int lastShares;
        stream.read((char*)&lastShares, sizeof(lastShares));
        lastShares = ntohl(lastShares);
        /*Set the shares*/
        exe.SetLastShares(lastShares);
        outStream<<" last shares: "<<lastShares<<" ";

        int price;
        stream.read((char*)&price, sizeof(price));
        price = ntohl(price);

        char scale;
        stream.get(scale);

        double debugPrice;
        if(price <= 0) {
            debugPrice = 0;
        } else {
            debugPrice = GetPriceFromFields(price,scale);
        }
        exe.SetLastPrice(debugPrice);
        outStream<<" line price "<<price<<" scale "<<scale<<" PRICE: "<<debugPrice<<" ";
        
        char liq;
        stream.get(liq);
        exe.SetBillingCode(liq);
        
        char side;
        stream.get(side);
        exe.SetSide(IncomingTranslator::ToStaffordSide(side));

        stream.ignore(1);
        
        outStream<<" liq: "<<liq<<" side: "<<side<<" ";
        memset(outputString, '\0', OUTPUT_MESS_LEN);
        outStream.read(outputString, outStream.tellp());
        ((CMessageApp*)AfxGetApp())->ProcessTraffic(
            "ARCADIRECT Process Order Filled. ", outputString);
        outStream.seekg(0, std::ios::beg);
        outStream.seekp(0, std::ios::beg);

        return seq;
    }


    inline int ProcessBustOrCorrect(std::stringstream& stream,
                                     CString& finalMessage) {
        /*We ignore the first 4 bytes since they are not used by 
          ROM. They are the MessageType, filler, Length of message.*/
        stream.ignore(4);
        int seq;
        stream.read((char*)&seq, sizeof(seq));
        seq = ntohl(seq);
        outStream <<" Sequence Num: "<<seq<<" ";
        /*Extract the time of the transaction.*/
        stream.ignore(4);
        /*The ID of the original order.*/
        int clientID;
        stream.read((char*)&clientID, sizeof(clientID));  
        clientID = ntohl(clientID);
        outStream<<" ClientID: "<<clientID<<" ";
         
        int arcaID;
        stream.read((char*)&arcaID, sizeof(arcaID));
        arcaID = ntohl(arcaID);
        outStream<<" EXECUTION ID: "<<arcaID<<" ";
        
        int lastShares;
        stream.read((char*)&lastShares, sizeof(lastShares));
        lastShares = ntohl(lastShares);
        /*Set the shares*/
        outStream<<" ORDER QUANTITY: "<<lastShares<<" ";

        int price;
        stream.read((char*)&price, sizeof(price));
        price = ntohl(price);

        char scale;
        stream.get(scale);

        double debugPrice;
        if(price <= 0) {
            debugPrice = 0;
        } else {
            debugPrice = GetPriceFromFields(price,scale);
        }
        outStream<<" PRICE: "<<debugPrice<<" ";
        
        char side;
        stream.get(side);
        outStream<<" TYPE: "<<side<<" ";
        stream.ignore(2);
        memset(outputString, '\0', OUTPUT_MESS_LEN);
        outStream.read(outputString, outStream.tellp());
        finalMessage + outputString;
        outStream.seekg(0, std::ios::beg);
        outStream.seekp(0, std::ios::beg);
        return seq;

    }
} //namespace arcadirect


#endif //IncomingTranslator_H__
