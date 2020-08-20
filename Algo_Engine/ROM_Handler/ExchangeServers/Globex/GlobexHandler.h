/*******************************************************************************
*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
*  : The Handler class takes the place of the RomMessage createtor.
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
*     Parnell     
*
*******************************************************************************/

#ifndef _GLOBEX_HANDLER_H__
#define _GLOBEX_HANDLER_H__
#include <winsock2.h>
#include <string>
#include <iostream>

#include "ROMMessage.h"
#include "FixMsg.h"
#include "GlobexMemTagSet.h"
#include "GlobexSequenceGenerator.h"
#include "GlobexRomLogin.h"
#include "Mailer.h"
#include "AuditTrail.h"

namespace GLOBEX {
    static const std::string CUSTOMER_TYPE_CODE = "9702";
    static const std::string FEE_BILLING = "9706";
    static const std::string GLOBEX_CORRELATIONORDID = "9717";
    static const std::string NEX_EXPECTED_MSG_SEQ_NUM = "789";
    static const std::string ORDER_QTY_FLAG = "9768";
    static const std::string SECURITY_EXCHANGE = "2";
    static const std::string YES = "Y";
    
    static const std::string DECIMAL = ".";
    static const std::string ZERO = "0";
    static const std::string GLOBEX_MULT = "00";
    static const std::string MARKET_TYPE = "1";
    
    static const int TAG_RETURN = 4;
    static const int MAX_TAG = 7;
    
}


template < class C, class P, class T >
class CGlobexHandler {
public:
    explicit CGlobexHandler(CGlobexMemTagSet* tags, SMTP::CMailer* mailer,
        C* c, void(C::*_func)(std::string&),
        P* p, void(P::*_pfunc)(RFIX::CRCsv&), RFIX::CMsgSequence* seq,
        std::string& sci, std::string& tci,std::string& fti,
        std::string& targetSubId, std::string& sessionId,
        std::string& senderSubId, std::string& senderLocId, std::string& fee,
        std::string& custormerOrFirm, std::string& customerTypeCode,
        CGlobexOrderIdGenerator* generator, std::string& firmId) 
        : m_pTags(tags), pC(c), func(_func),pP(p),pfunc(_pfunc), 
        m_seq(seq), m_T(), m_senderCompID(sci), m_targetCompID(tci),
        pMailer(mailer), m_fConnected(true),m_firmId(firmId),
        m_fti(fti), m_targetSubId(targetSubId), m_sessionId(sessionId),
        m_senderSubId(senderSubId), m_senderLocId(senderLocId), 
        m_feeBilling(fee), m_customerOrFirm(custormerOrFirm),
        m_customerTypeCode(customerTypeCode), m_auditSenderCompID(),m_globexOrderGen(generator) { 
        m_auditSenderCompID.append(m_senderCompID);
        m_auditSenderCompID.append(m_firmId);
        m_auditSenderCompID.append(m_fti);
        
    }
    ~CGlobexHandler(){
        
    }
    
    inline void SetConnected(bool isConnected) {
        m_fConnected = isConnected;
    }
    
    inline void FromGlobex(RFIX::CRCsv& csv) {
        std::string type = csv.GetAt(RFIX::CSV_ECHO);
        if(type == RFIX::FIX_MSG_TYPE_EXEC) {
            ExecutionReport(csv);
        } else if(type == RFIX::FIX_MSG_TYPE_CANCELREJ) {
            CancelReject(csv);
        } else {
            std::cerr<<"UNHANDLED!!! "<<csv.Message()<<std::endl;
        }
    }
    
    inline void FromRom(std::string& l_sub){
        char type = l_sub[0];
        switch(type) {
        case 'L':
            {
                RFIX::CRomMessage<CGlobexRomLogin> message;
                if(message.ShouldSend()) {
                    (pP->*pfunc)(message);
                }
                std::string noTag("");
                CAuditLogger::Instance()->Audit(message, "SERVER GLOBEX", "FROM SERVER OM", false, noTag,
                    m_auditSenderCompID);
                break;
            }
        case 'K':
            {
                //Logon Reject will have already been logged.
                break;
            }
        case 'Q':
            {
                //Getting the Q is purly informational
                break;
            }
        case 'S':
            {
                break;
            }
        case 'E':
            {
                EnterOrder(l_sub);
                break;
            }
        case 'R':
            {
                Replace(l_sub);
                break;
            }
        case 'C':
            {
                Cancel(l_sub);
                break;
            }
        case 'c':
            {
                //we do not ever receive a small c order as a Fix server or exchange.
                break;
            }
        }
    }
    
private:
    
    inline void EnterOrder(std::string& l_sub) {
        if(!m_fConnected) {
            
            RFIX::CRomMessage<RFIX::CRomStatus>csv(l_sub);
            csv.SetAt(RFIX::CSV_COMMAND, RFIX::ROM_STATUS);
            csv.SetAt(RFIX::CSV_STATUS, RFIX::ROM_REJECT);
            (pP->*pfunc)(csv);
            return;
        }
        RFIX::CRomMessage<RFIX::CRomOrder> message(l_sub);
        //create unique orderid
        std::string orderId;
        CreateOrderId(orderId);
        std::string stafford = message.GetAt(RFIX::CSV_STAFFORD_TAG);
        m_pTags->CreateNewMap(stafford.c_str(), orderId.c_str(), m_seq->NextMsgSeqNum(),
            message.GetAt(RFIX::CSV_TRADER).c_str());
        m_pTags->SetTotalShares(message.GetAt(RFIX::CSV_SHARES));
        m_pTags->SetLeavesShares(message.GetAt(RFIX::CSV_SHARES));
        m_pTags->SetShares(message.GetAt(RFIX::CSV_SHARES));
        m_pTags->SetOriginalShares(message.GetAt(RFIX::CSV_SHARES));
        m_pTags->SetICumShares(0);
        m_pTags->SetILeavesShares(0);
        m_pTags->SetCancelTag(orderId.c_str());
        m_pTags->SetStaffordTag(stafford.c_str());
        m_pTags->SetOrderTag(orderId.c_str());
        m_pTags->SetCancelCount(0);
        m_pTags->SetTrader(message.GetAt(RFIX::CSV_TRADER));
        m_pTags->SetAccount(message.GetAt(RFIX::CSV_CLEARINGACCOUNT).c_str());
        m_pTags->SetAccountId(message.GetAt(RFIX::CSV_CLEARINGID).c_str());
        m_pTags->SetTradeFor(message.GetAt(RFIX::CSV_TRADE_FOR).c_str());
        message.SetAt(RFIX::CSV_TAG, orderId);
        
        if(message.GetAt(RFIX::CSV_TYPE) != GLOBEX::MARKET_TYPE) {
            std::string newPrice(message.GetAt(RFIX::CSV_PRICE));
            RomToGlobexPrice(newPrice);
            message.SetAt(RFIX::CSV_PRICE, newPrice);
        } else {
            message.Erase(RFIX::CSV_PRICE);
        }
        
        RFIX::CTypedFixMsg<RFIX::CFixFourTwo, RFIX::CFixOrder> msg;
        //msg.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SecurityExchange], GLOBEX::SECURITY_EXCHANGE);
        
        Send(message, msg);
        m_pTags->SetSequenceTag(atol(msg.GetValueForTag(RFIX::FIX_TAG_STRINGS[RFIX::FIX_MsgSeqNum]).c_str()));
    }
    
    inline void Replace(std::string& l_sub) {
        RFIX::CRomMessage<RFIX::CRomReplace> message(l_sub);
        
        std::string staffordTag = message.GetAt(RFIX::CSV_STAFFORD_TAG);
        if(m_pTags->GetTags(MTAG::STAFFORD_TAG_QUERY, staffordTag)) {
            
            std::string trader(message.GetAt(RFIX::CSV_OWNER));
            if(trader.size() > 0 && trader[0] != '_' && m_pTags->CancelCount() >=1) {
                return;
            }
            
            std::string exchangeTag(m_pTags->ExchangeTag());
            message.SetAt(RFIX::CSV_EXCHANGE_TAG, exchangeTag);
            std::string orderId;
            CreateOrderId(orderId);
            message.SetAt(RFIX::CSV_NEW_TAG, orderId); 
            m_pTags->SetCancelTag(orderId.c_str());
            m_pTags->SetOrderTag(message.GetAt(RFIX::CSV_TAG).c_str());
            m_pTags->IncrementCancelCount();
            
            int replaceShares = atoi(message.GetAt(RFIX::CSV_SHARES).c_str());
            //int cumShares = atoi(m_pTags->GetCumShares());
            int revisedShares = m_pTags->GetOriginalShares() - m_pTags->GetShares() + replaceShares ;//+ cumShares;
            char shareBuff[30];
            memset(shareBuff, '\0', 30);
            itoa(revisedShares, shareBuff, 10);
            std::string newShares(shareBuff);
            message.SetAt(RFIX::CSV_SHARES, newShares);
            std::string account(m_pTags->Account());
            std::string accountId(m_pTags->AccountId());
            message.SetAt(RFIX::CSV_CLEARINGACCOUNT, account);
            message.SetAt(RFIX::CSV_CLEARINGID, accountId);
            message.SetAt(RFIX::CSV_TAG, orderId);
            if(message.GetAt(RFIX::CSV_TYPE) != GLOBEX::MARKET_TYPE) {
                std::string newPrice(message.GetAt(RFIX::CSV_PRICE));
                RomToGlobexPrice(newPrice);
                message.SetAt(RFIX::CSV_PRICE, newPrice);               
            } else {
                message.Erase(RFIX::CSV_PRICE);
            }
            std::string tradeFor(m_pTags->TradeFor());
            message.SetAt(RFIX::CSV_TRADE_FOR, tradeFor);
            RFIX::CTypedFixMsg<RFIX::CFixFourTwo, RFIX::CFixReplace> msg;
            msg.SetAt(GLOBEX::ORDER_QTY_FLAG, GLOBEX::YES);
            
            Send(message, msg);//(pC->*func)(message, msg);
        } else {
            std::cerr<<"COULD NOT FIND THIS Replace IN MAP!!! "<<l_sub<<std::endl;
        }
        
    }
    inline void Cancel(std::string& l_sub){
        RFIX::CRomMessage<RFIX::CRomCancel> message(l_sub);
        std::string staffordTag = message.GetAt(RFIX::CSV_STAFFORD_TAG);
        if(m_pTags->GetTags(MTAG::STAFFORD_TAG_QUERY, staffordTag)) {           
            std::string trader(message.GetAt(RFIX::CSV_OWNER));
            if(trader.size() > 0 && trader[0] != '_' && m_pTags->CancelCount() >=1) {
                return;
            }
            std::string exchangeTag(m_pTags->ExchangeTag());
            message.SetAt(RFIX::CSV_EXCHANGE_TAG, exchangeTag);
            std::string orderId;
            CreateOrderId(orderId);
            message.SetAt(RFIX::CSV_NEW_TAG, orderId);
            //m_pTags->SetOrderTag(message.GetAt(RFIX::CSV_TAG).c_str());
            message.SetAt(RFIX::CSV_EXCHANGE_TAG, m_pTags->ExchangeTag());
            message.SetAt(RFIX::CSV_TAG, m_pTags->OrderTag());
            m_pTags->SetCancelTag(orderId.c_str());
            m_pTags->IncrementCancelCount();
            std::string account(m_pTags->Account());
            std::string accountId(m_pTags->AccountId());
            message.SetAt(RFIX::CSV_CLEARINGACCOUNT, account);
            message.SetAt(RFIX::CSV_CLEARINGID, accountId);
            std::string tradeFor(m_pTags->TradeFor());
            message.SetAt(RFIX::CSV_TRADE_FOR, tradeFor);
            
            RFIX::CTypedFixMsg<RFIX::CFixFourTwo, RFIX::CFixCancel> msg;
            Send(message, msg);
        } else {
            std::cerr<<"COULD NOT FIND THIS CANCEL IN MAP!!! "<<l_sub<<std::endl;
        }       
    }
    
    inline void ExecutionReport(RFIX::CRCsv& cvs) {
        std::string transType = cvs.GetAt(RFIX::CSV_PASSTHROUGH);
        if(transType == "1" || transType == "2") {
            std::string lsSubject = "ATTENTION! ";//
            lsSubject += transType == "1" ? "Bust From Globex " : "Correction From Globex";
            std::string lsBust("ALERT FROM Globex!");
            MailErrorReport(lsSubject, lsBust, cvs);
            return;
        }
        std::string orderTag = cvs.GetAt(RFIX::CSV_TAG);
        std::string cancelTag = cvs.GetAt(RFIX::CSV_NEW_TAG);
        std::string status = cvs.GetAt(RFIX::CSV_STATUS);
        std::string origOrderId;
        if(status == RFIX::FIX_MSG_TYPE_CANCELREJ ||
            status == RFIX::FIX_ORD_STATUS_CANCELLED ||
            status == RFIX::FIX_ORD_STATUS_PENDING_REPLACE ||
            status == RFIX::ROM_PENDING_CANCEL) {
            GetOrigOrderId(origOrderId, orderTag);
            orderTag = origOrderId;
        } 
        
        if(m_pTags->GetTags(MTAG::ORDER_TAG_QUERY, orderTag) ||
            m_pTags->GetTags(MTAG::CANCEL_TAG_QUERY, orderTag) ||
            m_pTags->GetTags(MTAG::ORDER_TAG_QUERY, cancelTag) ||
            m_pTags->GetTags(MTAG::CANCEL_TAG_QUERY, cancelTag)) {
            std::string stafford(m_pTags->StaffordTag());
            cvs.SetAt(RFIX::CSV_STAFFORD_TAG, stafford);           
            
            //std::string status = cvs.GetAt(RFIX::CSV_STATUS);
            m_pTags->SetExchangeTag(cvs.GetAt(RFIX::CSV_EXCHANGE_TAG).c_str());
            std::string trader(m_pTags->Trader());
            cvs.SetAt(RFIX::CSV_TRADER, trader);
            if(status == RFIX::FIX_ORD_STATUS_PENDING_NEW) {
                status = RFIX::OM_PENDING_NEW;
                cvs.SetAt(RFIX::CSV_STATUS, status);
            } else if(status == RFIX::FIX_ORD_STATUS_CALCULATED) {
                status = RFIX::OM_CALCULATED;
                cvs.SetAt(RFIX::CSV_STATUS, status);
            } else if(status == RFIX::FIX_ORD_STATUS_EXPIRED) {
                status = RFIX::OM_EXPIRED;
                cvs.SetAt(RFIX::CSV_STATUS, status);
            } else if(status == RFIX::FIX_ORD_STATUS_PENDING_REPLACE) {
                status = RFIX::OM_REPLACE_PENDING;
                cvs.SetAt(RFIX::CSV_STATUS, status);
            } else if(status == RFIX::FIX_EXEC_TRANS_TYPE_NEW) {
                status = RFIX::OM_NEW;
                cvs.SetAt(RFIX::CSV_STATUS, status);
            }
            if(status == RFIX::FIX_ORD_STATUS_FILLED ||
                status == RFIX::FIX_ORD_STATUS_CANCELLED ||
                status == RFIX::FIX_ORD_STATUS_EXPIRED ||
                status == RFIX::FIX_ORD_STATUS_REJECTED) {
                
                m_pTags->RemoveTags();
            } else if(status == RFIX::FIX_ORD_STATUS_PARTIAL_FILL) {
                std::string lastShares(cvs.GetAt(RFIX::CSV_LAST_SHARES));
                m_pTags->SetShares(m_pTags->GetShares() - atoi(lastShares.c_str()));
                
            } else if(status == RFIX::FIX_ORD_STATUS_REPLACED) {
                m_pTags->SetOrderTag(orderTag.c_str());
                m_pTags->SetCancelCount(0);
                m_pTags->SetLeavesShares(cvs.GetAt(RFIX::CSV_LEAVE_SHARES));
                
                std::string shares(cvs.GetAt(RFIX::CSV_SHARES));
                int l_exeShares = atoi(shares.c_str());
                
                int l_iShares = m_pTags->GetShares() - m_pTags->GetOriginalShares() + l_exeShares;
                m_pTags->SetShares(l_iShares);
                m_pTags->SetOriginalShares(l_exeShares);
                m_pTags->SetTotalShares(l_iShares);
                
                cvs.Set(RFIX::CSV_SHARES, m_pTags->GetTotalShares());//shares);
            }
            std::string newExePrice(cvs.GetAt(RFIX::CSV_LAST_PRICE));
            GlobexToRomPrice(newExePrice);
            cvs.SetAt(RFIX::CSV_LAST_PRICE, newExePrice);
            std::string newPrice(cvs.GetAt(RFIX::CSV_PRICE));
            GlobexToRomPrice(newPrice);
            cvs.SetAt(RFIX::CSV_PRICE, newPrice);
            
            (pP->*pfunc)(cvs);
            CAuditLogger::Instance()->Audit(cvs, "SERVER GLOBEX", "FROM CME", false, cvs.GetAt(RFIX::CSV_TAG),
                m_auditSenderCompID);
            
        } else {
            std::string lsSubject("Could not find Execution (GLOBEX)");
            std::string lsBust("We received this execution from Globex but we had no record of the order in the Tag map.");
            MailErrorReport(lsSubject, lsBust, cvs);    
        }
    }
    
    
    inline void MailErrorReport(std::string& subject, std::string& lsBust, RFIX::CRCsv& cvs) {
        //std::string lsBust();  
        lsBust += "\n\nDetails: ";
        lsBust +="\n";
        lsBust +="Symbol ";
        lsBust += cvs.GetAt(RFIX::CSV_SYMBOL);
        lsBust += "\nSHARES ";
        lsBust += cvs.GetAt(RFIX::CSV_SHARES);
        lsBust += "\nPRICE ";
        lsBust += cvs.GetAt(RFIX::CSV_PRICE);
        lsBust += "\nType ";
        lsBust += cvs.GetAt(RFIX::CSV_TYPE);
        
        lsBust += "\nExchange Tag: ";
        lsBust += cvs.GetAt(RFIX::CSV_EXCHANGE_TAG);
        lsBust += "\nOrder ID: ";
        lsBust += cvs.GetAt(RFIX::CSV_TAG);
        lsBust += "\n";
        lsBust += "          \n";
        lsBust += cvs.Message();
        lsBust += "          \n";
        pMailer->MailMessage(subject, lsBust);
    }
    
    inline void CancelReject(RFIX::CRCsv& cvs) {
        std::string cancelTag = cvs.GetAt(RFIX::CSV_TAG);
        std::string orderTag = cvs.GetAt(RFIX::CSV_NEW_TAG);
        std::string origOrderId;
        GetOrigOrderId(origOrderId, orderTag);
        orderTag = origOrderId;
        if(m_pTags->GetTags(MTAG::CANCEL_TAG_QUERY, cancelTag) ||
            m_pTags->GetTags(MTAG::ORDER_TAG_QUERY, orderTag)) {
            
            m_pTags->SetCancelTag(m_pTags->OrderTag());
            std::string stafford(m_pTags->StaffordTag());
            cvs.SetAt(RFIX::CSV_STAFFORD_TAG, stafford);
            m_pTags->SetCancelCount(0);
            std::string trader(m_pTags->Trader());
            cvs.SetAt(RFIX::CSV_TRADER, trader);
            
            std::string rejectType(cvs.GetAt(RFIX::CSV_REJECT_BY));
            if(rejectType == RFIX::DEFAULT_ONE) {
                std::string status(RFIX::OM_CANCEL_REJ);
                cvs.SetAt(RFIX::CSV_STATUS, status);
            } else {
                std::string status(RFIX::OM_REPLACE_REJ);
                cvs.SetAt(RFIX::CSV_STATUS, status);
            }
            cvs.SetAt(RFIX::CSV_TAG, orderTag);
            cvs.SetAt(RFIX::CSV_ORDER_TAG, orderTag);
            (pP->*pfunc)(cvs);
            CAuditLogger::Instance()->Audit(cvs, "SERVER GLOBEX", "FROM CME", false, cvs.GetAt(RFIX::CSV_TAG),
                m_auditSenderCompID);
            
        } else {
            std::cerr<<"COULD NOT FIND THIS CANCELREJECT IN MAP!!! "<<cvs.Message()<<std::endl;
        }
    }    
    
    inline void CreateOrderId(std::string& orderId) {
        m_globexOrderGen->NextOrderId(orderId);
    }
    
    inline void SetGlobexCustomFields(RFIX::CFixMsg& msg) {
        msg.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_CustomerOrFirm], m_customerOrFirm);
        msg.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SecurityExchange], "2");
        msg.SetAt(GLOBEX::CUSTOMER_TYPE_CODE,m_customerTypeCode);
        msg.SetAt(GLOBEX::FEE_BILLING, m_feeBilling);
    }
    
    inline void SetSenderCompId(RFIX::CFixMsg& msg, RFIX::CRCsv& romMsg) {
        std::string tempSenderCompId(m_senderCompID);
        
        std::string clearingId = romMsg.GetAt(RFIX::CSV_CLEARINGID);
        if(clearingId == RFIX::EMPTY_STRING) {
            tempSenderCompId += m_firmId;           
        } else {
            tempSenderCompId += clearingId;
        }
        tempSenderCompId += m_fti;
        msg.SetAt(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderCompID], tempSenderCompId);
    }
    
    inline void SetGlobexHeaderFields(RFIX::CFixMsg& fixMsg) {
        fixMsg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetCompID], m_targetCompID);
        //fixMsg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderSubID], senderSubId);
        fixMsg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_TargetSubID], m_targetSubId);
        fixMsg.Replace(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderLocationID], m_senderLocId);
        
    }
    
    inline void Send(RFIX::CRCsv& romMsg, RFIX::CFixMsg& fixMsg) {
        
        m_T.RomToFix(romMsg, fixMsg);
        SetSenderCompId(fixMsg, romMsg); 
        SetGlobexHeaderFields(fixMsg);
        SetGlobexCustomFields(fixMsg);
        long seq = m_seq->NextMsgSeqNum();
        fixMsg.SetMsgSeqNum(seq);
        std::string str;
        str = fixMsg.GetFormattedMsg();       
        m_seq->AddMsg(str, seq);
        
        (pC->*func)(str);
        CAuditLogger::Instance()->Audit(romMsg, "SERVER GLOBEX", "FROM SERVER OM", false, romMsg.GetAt(RFIX::CSV_TAG),
            fixMsg.GetValueForTag(RFIX::FIX_TAG_STRINGS[RFIX::FIX_SenderCompID]));
    }
    
    //**************************************************************************
    // We need this function because the CME only sends back the last 5 digits
    // of an CIOrderID (FIX tag 11) on a cancel or cancel replace. We "know" that 
    // we only have orders open for today so we add on the the current date.
    //****************************************************************************
    inline void GetOrigOrderId(std::string& origOrderId, const std::string& orderTag) {
        SYSTEMTIME systime;
        GetSystemTime(&systime);
        char currUTCTime[50];
        memset(currUTCTime, '\0', 50);
        wsprintf(currUTCTime, "%02u%02u",                
            systime.wMonth, systime.wDay);
        origOrderId += currUTCTime;
        int tagSize = orderTag.size();
        if(tagSize >= GLOBEX::TAG_RETURN && tagSize < GLOBEX::MAX_TAG) {
            int begin = tagSize - GLOBEX::TAG_RETURN;
            origOrderId += orderTag.substr(begin, GLOBEX::TAG_RETURN);//+= orderTag.Right(4);
        }
    }
    
    inline void RomToGlobexPrice(std::string& price) {
        if(price.size() > 0) {
            std::string::size_type loc = price.find(GLOBEX::DECIMAL, 0);
            if(loc != std::string::npos) {
                price.erase(loc, 1);
                if((loc +2) <= price.size()) {
                    std::string::size_type newLoc = loc + 2;
                    price.insert(newLoc, GLOBEX::DECIMAL);
                } else {
                    std::string::size_type newLoc = loc + 1;
                    price.append("0");
                    //price.insert(newLoc, GLOBEX::DECIMAL);
                }
            } else {
                price.append(GLOBEX::GLOBEX_MULT);
            }
        }
    }
    
    inline void GlobexToRomPrice(std::string& price) {
        int size = price.size();
        if(size > 0) {
            std::string::size_type loc = price.find(GLOBEX::DECIMAL, 0);
            if(loc != std::string::npos) {
                price.erase(loc, 1);
                int jumpLen = size > 1 ? (loc - 2) : 1;
                std::string::size_type newLoc = loc - jumpLen;
                newLoc = newLoc < 0 ? 0 : newLoc;
                price.insert(newLoc, GLOBEX::DECIMAL);
            } else if(price.size() >=2){
                std::string::size_type priceSize = price.size();
                std::string::size_type decLoc = priceSize - 2;
                decLoc = decLoc < 0 ? 0 : decLoc;
                price.insert(decLoc, GLOBEX::DECIMAL);
            } else {
                price.insert(0, GLOBEX::ZERO);
                price.insert(0, GLOBEX::DECIMAL);
            }
        }
    }
    
    
    
    CGlobexMemTagSet* m_pTags;
    
    void (C::*func)(std::string&);
    C* pC;
    void (P::*pfunc)(RFIX::CRCsv&);
    P* pP;
    
    T m_T;
    RFIX::CMsgSequence* m_seq;
    std::string m_senderCompID;
    std::string m_targetCompID;
    SMTP::CMailer* pMailer;
    bool m_fConnected;
    
    std::string m_feeBilling;
    std::string m_customerOrFirm;
    std::string m_firmId;
    std::string m_fti;
    std::string m_senderSubId;
    std::string m_targetSubId;
    std::string m_senderLocId;
    std::string m_sessionId;
    std::string m_customerTypeCode;
    std::string m_auditSenderCompID;
    CGlobexOrderIdGenerator* m_globexOrderGen;
    char cumShares_[20];
    
};


#endif //_GLOBEX_HANDLER_H__