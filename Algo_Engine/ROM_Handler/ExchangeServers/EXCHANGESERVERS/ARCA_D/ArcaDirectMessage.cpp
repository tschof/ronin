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
#include "stdafx.h"
#include "ArcaDirect.h"
#include "ArcaDirectMessage.h"
#include "Message.h"
#include "Order.h"


void ArcaDirectMessage::Init();
CTranslator ArcaDirectMessage::m_Translator;
CString ArcaDirectMessage::m_companyGroupID;

const int ArcaDirectMessage::ToStaffordSide(char ExchangeCode);
const char ArcaDirectMessage::ToExchangeSide(int StaffordCode);

const int ArcaDirectMessage::ToStaffordType(char ExchangeCode);
const char ArcaDirectMessage::ToExchangeType(int StaffordCode);
const int ArcaDirectMessage::ToStaffordTIF(char ExchangeCode);
const char ArcaDirectMessage::ToExchangeTIF(int StaffordCode);
const char* ArcaDirectMessage::GetCompanyGroupID();

const char* ArcaDirectMessage::GetCompanyGroupID() {
    return (LPCTSTR)m_companyGroupID;
}

const int ArcaDirectMessage::ToStaffordSide(char ExchangeCode)
{
    return (m_Translator.Lookup(m_Translator.m_ToStaffordSide,
        ExchangeCode));
}

const char ArcaDirectMessage::ToExchangeSide(int StaffordCode)
{
    return (m_Translator.Lookup(m_Translator.m_ToExchangeSide,
        StaffordCode));
}

const int ArcaDirectMessage::ToStaffordType(char ExchangeCode)
{
    return (m_Translator.Lookup(m_Translator.m_ToStaffordType,
        ExchangeCode));
}

const char ArcaDirectMessage::ToExchangeType(int StaffordCode)
{
    return (m_Translator.Lookup(m_Translator.m_ToExchangeType,
        StaffordCode));
}


const int ArcaDirectMessage::ToStaffordTIF(char ExchangeCode)
{
    return (m_Translator.Lookup(m_Translator.m_ToStaffordTIF,
        ExchangeCode));
}

const char ArcaDirectMessage::ToExchangeTIF(int StaffordCode)
{
    return (m_Translator.Lookup(m_Translator.m_ToExchangeTIF,
        StaffordCode));
}


void ArcaDirectMessage::Init()
{
    m_Translator.m_ToExchangeSide[COrder::FYI]	= ArcaDirectMessage::Buy;
    m_Translator.m_ToExchangeSide[COrder::Buy]	= ArcaDirectMessage::Buy;
    m_Translator.m_ToExchangeSide[COrder::Sell]	= ArcaDirectMessage::Sell;
    m_Translator.m_ToExchangeSide[COrder::BuyMinus]	= ArcaDirectMessage::Buy;
    m_Translator.m_ToExchangeSide[COrder::SellPlus]	= ArcaDirectMessage::Sell;
    m_Translator.m_ToExchangeSide[COrder::Short]  = ArcaDirectMessage::SellShort;
    m_Translator.m_ToExchangeSide[COrder::Exempt] = ArcaDirectMessage::SellShortEx;
    m_Translator.m_ToExchangeSide[COrder::Undisclosed] = ArcaDirectMessage::Buy;
    m_Translator.m_ToExchangeSide[COrder::Cross] = ArcaDirectMessage::Cross;
    m_Translator.m_ToExchangeSide[COrder::CrossSell] = ArcaDirectMessage::CrossShort;
    
    m_Translator.m_ToExchangeType[COrder::Market] = ArcaDirectMessage::Market;
    m_Translator.m_ToExchangeType[COrder::Limit] = ArcaDirectMessage::Limit;
    m_Translator.m_ToExchangeType[COrder::LimitOrBetter] = ArcaDirectMessage::InsideLimit;
    
    
    m_Translator.m_ToExchangeTIF[COrder::Day] = ArcaDirectMessage::DayOrder;
    m_Translator.m_ToExchangeTIF[COrder::IC] = ArcaDirectMessage::IOC;
    m_Translator.m_ToExchangeTIF[COrder::ExtendedDay] = ArcaDirectMessage::LateAuctionOnly;
    m_Translator.m_ToExchangeTIF[COrder::OPG] = ArcaDirectMessage::EarlyAuctionOnly;
    
    
    m_Translator.m_ToStaffordSide[ArcaDirectMessage::Buy] = COrder::Buy;
    m_Translator.m_ToStaffordSide[ArcaDirectMessage::Sell] = COrder::Sell;
    m_Translator.m_ToStaffordSide[ArcaDirectMessage::SellShort] = COrder::Short;
    m_Translator.m_ToStaffordSide[ArcaDirectMessage::SellShortEx] = COrder::Exempt;
    m_Translator.m_ToStaffordSide[ArcaDirectMessage::Cross] = COrder::Cross;
    m_Translator.m_ToStaffordSide[ArcaDirectMessage::CrossShort] = COrder::CrossSell;
    
    m_Translator.m_ToStaffordType[ArcaDirectMessage::Market] = COrder::Market;
    m_Translator.m_ToStaffordType[ArcaDirectMessage::Limit] = COrder::Limit;
    m_Translator.m_ToStaffordType[ArcaDirectMessage::InsideLimit] = COrder::LimitOrBetter;
    
    m_Translator.m_ToStaffordTIF[ArcaDirectMessage::DayOrder] = COrder::Day;
    m_Translator.m_ToStaffordTIF[ArcaDirectMessage::IOC] = COrder::IC;
    m_Translator.m_ToStaffordTIF[ArcaDirectMessage::LateAuctionOnly] = COrder::ExtendedDay;
    m_Translator.m_ToStaffordTIF[ArcaDirectMessage::EarlyAuctionOnly] = COrder::OPG;
    CMessageApp	*pApp = (CMessageApp *)AfxGetApp();
    GetPrivateProfileString("Session Information",	
                            "Company Group ID",		
                            "RONIN",
                            m_companyGroupID.GetBuffer(128),
                            128, 
                            pApp->Path() + pApp->Name() + ".DAT");
    m_companyGroupID.ReleaseBuffer();

}

ArcaDirectMessage::ArcaDirectMessage()
{
}

