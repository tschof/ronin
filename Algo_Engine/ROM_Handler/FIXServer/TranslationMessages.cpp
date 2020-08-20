#include "stdafx.h"
#include "TranslationMessages.h"
#include "Values.h"
#include "UserDefinedFields.h"
#include "FIXServer.h"
#include "Log.h"

EnterOrderFromFIX::EnterOrderFromFIX( const FIX42::NewOrderSingle& single )
{
    std::string trader = const_cast<FIX42::NewOrderSingle&>(single).getSessionID().getSenderCompID().getString();
    SetTrader( trader );
    
    FIX::ClOrdID tag;
    single.get( tag );
    SetTag( tag );
    
    FIX::Side side;
    single.get(side);
    SetSide( side - FIX::Side_BUY + 1 );
    
    FIX::ExecInst execInst;
    if( single.isSetField( execInst ) )
    {
        single.get( execInst );
        SetExecInst( execInst );
    }
    
    std::string szSymbol;
    szSymbol = single.get(FIX::Symbol());
    SetSymbol( szSymbol );
    SetShares( single.get(FIX::OrderQty()) );
    
    FIX::OrdType ordType;
    single.get(ordType);
    if(ordType == FIX::OrdType_MARKETLIMIT) {
        SetType(13);
    } else {
        SetType( ordType - FIX::OrdType_MARKET + 1 );
    }
    if( ordType == FIX::OrdType_LIMIT )
        SetPrice( single.get(FIX::Price()) );
    else if( ordType == FIX::OrdType_STOP )
    {
        if( !single.isSetField(FIX::StopPx()) )
            throw FIX::RequiredTagMissing(1);
        SetStopPrice( single.get(FIX::StopPx()) );
    }
    else if( ordType == FIX::OrdType_STOP_LIMIT )
    {
        if( !single.isSetField(FIX::StopPx()) )
            throw FIX::RequiredTagMissing(1);
        SetStopPrice( single.get(FIX::StopPx()) );
        SetPrice( single.get(FIX::Price()) );
    }
    else
        SetPrice( 0.0 );
    
    FIX::TimeInForce tif;
    if( single.isSetField( tif ) )
    {
        single.get( tif );
        SetTIF( tif );
        if( tif == FIX::TimeInForce_GOOD_TILL_DATE )
        {
            FIX::ExpireDate expire;
            if( single.isSetField( expire ) )
            {
                single.get( expire );
                SetGTDDate( expire.getString() ); 
            }
            else
                throw FIX::RequiredTagMissing(1);
        }
    }
    else
        SetTIF( 0 );
    
    FIX::Rule80A rule80A;
    
    if( single.isSetField(rule80A) )
    {
        single.get(rule80A);
        SetCapacity( rule80A );
    }
    else
        SetCapacity( OM::Capacity_Agent );
    
    
    FIX::Account account;
    if( !single.isSetField(account) )
    {
        Log::Write( LOG_ERROR, "Missing Account Field" );
        throw FIX::RequiredTagMissing(1);
    }
    
    std::stringstream accountStream(single.get(FIX::Account()));
    
    std::string traderAcr; // Account
    std::string clearingId;
    std::string clearingAccount;
    std::string firm;
    std::string localAccount;
    std::string acctType = "CS"; // default acct type;
    
    accountStream >> traderAcr >> clearingId >> clearingAccount >> firm >> localAccount;
    if( accountStream.fail() )
    {
        Log::Write( LOG_ERROR, "Invalid Account Field" );
        throw FIX::IncorrectDataFormat(1);
    }
    
    SetTradeFor( traderAcr );
    SetClearingID(clearingId);
    SetClearingAccount(clearingAccount);
    SetFirm(firm);
    SetLocalAccount(localAccount);
    
    FIX::ExDestination dest;
    if( single.isSetField(dest) )
    {
        single.get(dest);
        std::string szDest = dest;
        SetExecutionDestination( szDest );	// WXU
        // SetExecutionDestination( szDest[0] );
    }
    
    FIX::OMExchange omExch;
    
    if( !single.isSetField(omExch) )
    {
        if( single.isSetField(dest) )
        {
            single.get(dest);
            std::string szDest = dest;
            SetExchange( atoi(szDest.c_str()) );
        }
        else
        {
            try
            {
                int nDest = FIXServer::getInst()->GetOrderRoute( single );	
                SetExchange( nDest ); 
            }
            catch(...)
            {
                std::cout << "Invalid Message: Missing OMExchange tag ..." << std::endl;
                Log::Write( LOG_MESSAGE, "Invalid Message: Missing OMExchange tag ..." );
                throw FIX::RequiredTagMissing(100);
            }
        }
    }
    else
    {
        single.getField(omExch);
        SetExchange( omExch );
    }
    
    FIX::MaxFloor max_floor;
    if( single.isSetField(max_floor) )
    {
        single.getField(max_floor);
        SetMaxFloor( max_floor );
    }
    
    FIX::SecurityType type;
    if( single.isSetField(type) )
    {
        single.get(type);
        if( type.getValue() == FIX::SecurityType_OPTION )
        {
            acctType = "OPT";
            SetSecurityType( 'O' ); 
            std::string szUnder = FIXServer::getInst()->FindOptionUnderlying( single.get(FIX::Symbol()) );
            SetUnderlying( szUnder );
            // no support for index on option 'I' ?
            FIX::PutOrCall option_type;
            if( !single.isSetField(option_type) )
            {
                Log::Write( LOG_ERROR, "Missing PutOrCall" );
                throw FIX::RequiredTagMissing(1);
            }
            single.get(option_type);
            if( option_type == 0 )
                SetOptionType( 'P' );
            else if( option_type == 1 )
                SetOptionType( 'C' );
            else
            {
                Log::Write( LOG_ERROR, "Invalid option type ..." );
                throw FIX::RequiredTagMissing(1);
            }
            
            FIX::MaturityMonthYear exp_date;
            if( !single.isSetField(exp_date) )
            {
                Log::Write( LOG_ERROR, "Missing MaturityMonthYear" );
                throw FIX::RequiredTagMissing(1);
            }
            single.get(exp_date);
            SetExpirationDate( exp_date );
            
            FIX::StrikePrice strike;
            if( !single.isSetField(strike) )
            {
                Log::Write( LOG_ERROR, "Missing StrikePrice" );
                throw FIX::RequiredTagMissing(1);
            }
            single.get(strike);
            SetStrikePrice( strike );
            
            FIX::OpenClose open_close;
            if( !single.isSetField(open_close) )
            {
                Log::Write( LOG_ERROR, "Missing OpenClose" );
                throw FIX::RequiredTagMissing(1);
            }
            single.get(open_close);
            if( open_close == 'C' )
                SetOpenClose( 0 );
            else if( open_close == 'O' )
                SetOpenClose( 1 );
            else
            {
                Log::Write( LOG_ERROR, "Invalid open close value ..." );
                throw FIX::IncorrectDataFormat(1);
            }
            
            
            FIX::ClearingFirm cmta;
            if( single.isSetField(cmta) )
            {
                single.get(cmta);
                SetCMTA(cmta);
            }
            
            FIX::ClearingAccount mmaccount;
            if( single.isSetField(mmaccount) )
            {
                single.get(mmaccount);
                std::string acct = mmaccount;
                int nIndex = acct.find(":");
                if( nIndex != -1 )
                {
                    SetMMAccount( acct.substr(nIndex+1,-1) );
                    SetMMHomeExchange( acct.substr( 0, nIndex ) );
                }
                else
                    SetMMAccount( acct );
                
            }
            
            FIX::ExecBroker giveup;
            if( single.isSetField(giveup) )
            {
                single.get(giveup);
                SetGiveup(giveup);
            }
            
        }
        else if( type.getValue() == FIX::SecurityType_FUTURE )
        {
            acctType = "FUT";
            SetSecurityType( 'F' );
            try
            {
                std::string symbol = single.get(FIX::Symbol());
                std::string onechg;
                if ( symbol.length() > 2)
                    onechg = symbol.substr(symbol.length()-2);
                
                if ( onechg.compare("1C") == 0)
                {
                    SetSecurityType('J');
                    // SetUnderlying( symbol);
                }
                else
                {
                    onechg = symbol.substr(symbol.length()-4, 2);
                    if ( onechg.compare("1C") == 0)
                        SetSecurityType('J');
                    else
                    {
                        //std::string szUnder = FIXServer::getInst()->FindFutureUnderlying( symbol);
                        //SetUnderlying( szUnder );
                    }
                }
                FIX::MaturityMonthYear exp_date;
                if( !single.isSetField(exp_date) )
                {
                    Log::Write( LOG_ERROR, "Missing MaturityMonthYear" );
                    throw FIX::RequiredTagMissing(1);
                }
                single.get(exp_date);
                SetExpirationDate( exp_date );
            }
            catch(...)
            {
                Log::Write( LOG_ERROR, "FindFutureUnderlying failed" );
            }
        }
        else
        {
            acctType = "CS";
            SetSecurityType( 'E' );
            
            if( side == FIX::Side_SELL_SHORT )
            {
                FIX::LocateReqd reqd;
                if( !single.isSetField( reqd ) ) // got to set LocateReqd
                    throw FIX::RequiredTagMissing(1);
                single.get( reqd );
                if( reqd == true )
                {
                    FIX::SSFirm ssfirm;
                    if( single.isSetField( ssfirm ) )
                    {
                        single.getField( ssfirm );
                        SetSSFirm( ssfirm );
                    }
                    else // got to have SSFirm if LocateReqd is YES
                    {
                        Log::Write( LOG_ERROR, "Missing SSFirm when LocateReqd = YES" );
                        throw FIX::RequiredTagMissing(1);
                    }
                }
            }
        }
        
    }
    else
    {
        SetSecurityType( 'E' );
        if( side == FIX::Side_SELL_SHORT )
        {
            FIX::LocateReqd reqd;
            if( !single.isSetField( reqd ) ) // got to set LocateReqd
                throw FIX::RequiredTagMissing(1);
            single.get( reqd );
            if( reqd == true )
            {
                FIX::SSFirm ssfirm;
                if( single.isSetField( ssfirm ) )
                {
                    single.getField( ssfirm );
                    SetSSFirm( ssfirm );
                }
                else // got to have SSFirm if LocateReqd is YES
                {
                    Log::Write( LOG_ERROR, "Missing SSFirm when LocateReqd = YES" );
                    throw FIX::RequiredTagMissing(1);
                }
            }
        }
    }
    
    FIX::Text text;
    if( single.isSetField(text) )
    {
        single.get(text);
        SetInstructions(text);
    }
    //Parnell TODO remove this else clause!
    /* 	else
    {
    AccountID id( traderAcr, localAccount, clearingAccount, firm, acctType );
    try
    {
    std::string szOMAccount = FIXServer::getInst()->GetOMAccount( id );
    SetInstructions( szOMAccount );
    }
    catch(...)
    {
    std::cout << "Cannot find OMAccount\n";
    Log::Write( LOG_ERROR, "Cannot find OM Account" );
    }
    }
    */
    
    SetStatus(OM::Status_PendingNew);
}



CancelOrderFromFIX::CancelOrderFromFIX( const FIX42::OrderCancelRequest& cancel )
{
    // FIX41 required
    std::string trader = const_cast<FIX42::OrderCancelRequest&>(cancel).getSessionID().getSenderCompID().getString();
    SetTrader( trader );
    
    FIX::OrigClOrdID tag;
    cancel.get( tag );
    SetTag( tag );
    
    FIX::ClOrdID action;
    cancel.get( action );
    SetActionID( action );
    
    SetSide( cancel.get(FIX::Side()) - FIX::Side_BUY + 1 );
    SetSymbol( cancel.get(FIX::Symbol()) );
    
    //try {
    if(cancel.isSetField(FIX::Account())) {
        std::stringstream accountStream(cancel.get(FIX::Account()));
        
        std::string traderAcr; // Account
        std::string clearingId;
        std::string clearingAccount;
        std::string firm;
        std::string localAccount;
        
        accountStream >> traderAcr >> clearingId >> clearingAccount >> firm >> localAccount;
        if( accountStream.fail() ) {
            Log::Write( LOG_ERROR, "Invalid Account Field" );
        } else {
            SetTradeFor( traderAcr );
            SetClearingID(clearingId);
            SetClearingAccount(clearingAccount);
        }
    }
    //} catch (...) {
    //     Log::Write( LOG_ERROR, "Invalid Account Field" );
    // }
    
}

ReplaceOrderFromFIX::ReplaceOrderFromFIX( const FIX42::OrderCancelReplaceRequest& replace )
{
    std::string trader = const_cast<FIX42::OrderCancelReplaceRequest&>(replace).getSessionID().getSenderCompID().getString();
    SetTrader( trader );
    
    FIX::OrigClOrdID tag;
    replace.get( tag );
    
    SetTag( tag );
    
    FIX::ClOrdID action;
    replace.get( action );
    
    SetActionID( action );
    SetNewOrderID( action );
    
    if( replace.isSetField( FIX::Side() ) )
    {
        //{ WXU, to handle SSF in replace
        //- SetSide( replace.get(FIX::Side()) - FIX::Side_BUY + 1 );
        FIX::Side side;
        replace.get(side);
        SetSide( side - FIX::Side_BUY + 1 );
        
        if( side == FIX::Side_SELL_SHORT || side == FIX::Side_SELL_SHORT_EXEMPT)
        {
            FIX::LocateReqd reqd;
            if( ! replace.isSetField( reqd ) ) // got to set LocateReqd
                throw FIX::RequiredTagMissing(1);
            replace.get( reqd );
            if( reqd == true )
            {
                FIX::SSFirm ssfirm;
                if( replace.isSetField( ssfirm ) )
                {
                    replace.getField( ssfirm );
                    SetSSFirm( ssfirm );
                }
                else // got to have SSFirm if LocateReqd is YES
                {
                    Log::Write( LOG_ERROR, "Missing SSFirm when LocateReqd = YES" );
                    throw FIX::RequiredTagMissing(1);
                }
            }
        }
        //}
    }
    
    if( replace.isSetField( FIX::Symbol() ) )
        SetSymbol( replace.get(FIX::Symbol()) );
    if( replace.isSetField( FIX::OrdType() ) )
        SetType( replace.get(FIX::OrdType()) - FIX::OrdType_MARKET + 1 );
    
    FIX::Price price;
    if( replace.isSetField( price ) )
        SetPrice( replace.get(FIX::Price()) );
    
    FIX::TimeInForce tif;
    if( replace.isSetField( tif ) )
    {
        replace.get( tif );
        SetTIF( tif );
        if( tif == FIX::TimeInForce_GOOD_TILL_DATE )
        {
            FIX::ExpireDate expire;
            if( replace.isSetField( expire ) )
            {
                replace.get( expire );
                SetGTDDate( replace.getString() ); 
            }
        }
    }
    else
        SetTIF( 0 );
    
    FIX::ExecInst execInst;
    if( replace.isSetField( execInst ) )
    {
        replace.get( execInst );
        SetExecInst( execInst );
    }
    
    FIX::OrderQty qty;
    if( replace.isSetField( qty ) )
    {
        replace.get( qty );
        SetShares( qty );
    }
    
    FIX::Text text;
    if( replace.isSetField(text) )
    {
        replace.get(text);
        SetOrderTag(text);
    }
    
    if(replace.isSetField(FIX::Account())) {
        std::stringstream accountStream(replace.get(FIX::Account()));
        
        std::string traderAcr; // Account
        std::string clearingId;
        std::string clearingAccount;
        std::string firm;
        std::string localAccount;
        
        accountStream >> traderAcr >> clearingId >> clearingAccount >> firm >> localAccount;
        if( accountStream.fail() ) {
            Log::Write( LOG_ERROR, "Invalid Account Field" );
        } else {
            SetTradeFor( traderAcr );
            SetClearingID(clearingId);
            SetClearingAccount(clearingAccount);
        }
    } 
    
    // tell om shares is the new order qty = cum+leave
    SetLeaveQty( -1 );
}

std::string ExecutionReportFromOM::NextExecID()
{
    UUID uid;
    UuidCreate( &uid );
    unsigned char *szID = NULL;
    UuidToString( &uid, &szID );
    std::string id = (char *)szID;
    RpcStringFree( &szID );
    return id;
}

ExecutionReportFromOM::ExecutionReportFromOM( const OM::Status& status )
{
    setSessionID( FIXServer::clientSessionID() );
    if( status.GetOMExecTag().size() > 0 )
        set( FIX::ExecID(status.GetOMExecTag()) );
    else
        set( FIX::ExecID("0") );
    
    set( FIX::OrderID(status.GetFirmTag()) );
    set( FIX::Side(status.GetSide() + FIX::Side_BUY - 1) );
    set( FIX::Symbol(status.GetSymbol()) );
    set( FIX::ExecTransType(FIX::ExecTransType_NEW) );
    
    
    set( FIX::LastPx(0.0f) );						// FIX4.1 Rquired
    set( FIX::LastShares(0) );						// FIX4.1 Rquired
    
    set( FIX::ClOrdID(status.GetTag()) );	
    
    int nStatus = status.GetStatus();
    std::string liq;
    switch( nStatus )
    {
    case OM::Status_PartiallyFilled:
    case OM::Status_Filled:
        set( FIX::LastPx(status.GetPrice()) );
        set( FIX::LastShares(status.GetShares()) );
        try { 
            liq = status.GetField(69);
        } catch(std::out_of_range e) {
            Log::Write(LOG_ERROR, "Translate Error: Liq Flag out of Range");
        }
        if(liq.size() <= 0) {
            liq = "NA";
        }
        setField(9730, liq);
        break;
    case OM::Status_Replaced:
        {
            set( FIX::Price(status.GetPrice()) );
            if( status.GetNewOrderID().length() > 0 )
                set( FIX::ClOrdID(status.GetNewOrderID()) );
            else
            {
                Log::Write( LOG_ERROR, "Missing NewOrderID in OM Replaced Status Message" );
                set( FIX::ClOrdID(status.GetTag()) );			
            }
            set( FIX::OrigClOrdID(status.GetTag()) );			
            // new to allow change stop price, limit price, order time, tif etc
            char cTIF = status.GetTIF();
            set( FIX::TimeInForce( cTIF ) );
            
            int nType = status.GetType() + (int)FIX::OrdType_MARKET-1;
            set( FIX::OrdType( (char)nType ) );
            
        }
        break;
    case OM::Status_CancelRejected:
    case OM::Status_ReplaceRejected:
        throw exception("Should not have happened");
        break;
    case OM::Status_Rejected:
        if( status.GetFirmTag().size() <= 0 ) {
            set ( FIX::OrderID("REJECT"));   
        }
        break;
    case OM::Status_PendingCancel:
    case OM::Status_ReplacePending:
        if( status.GetActionID().length() > 0 )
            set( FIX::ClOrdID(status.GetActionID()) );
        else
        {
            Log::Write( LOG_ERROR, "Missing ActionID in OM CancelPending/ReplacePending Status Message" );
            set( FIX::ClOrdID(status.GetTag()) );			
        }
        set( FIX::OrigClOrdID(status.GetTag()) );			
        break;
    default:
        break;
    }
    
    set( FIX::ExecType( TranslationManager::TranslateStatus( status.GetStatus() ) ) );
    try
    {
        set( FIX::OrdStatus( TranslationManager::TranslateStatus( status.GetOrderStatus() ) ) );
    }
    catch(...)
    {
        set( FIX::OrdStatus( TranslationManager::TranslateStatus( status.GetStatus() ) ) );
    }
    
    set( FIX::AvgPx( status.GetAvgPx()) );
    set( FIX::LeavesQty( status.GetLeaveQty()) );
    set( FIX::CumQty( status.GetCumQty()) );
    set( FIX::OrderQty(status.GetLeaveQty()+status.GetCumQty()) );
    
    std::string szTemp;
    szTemp = status.GetAccount();
    if( !szTemp.empty() )
    {
        try
        {
            std::string szType = "CS";
            switch( status.GetSecurityType() )
            {
            case 'E':
                szType = "CS";
                break;
            case 'O':
            case 'I':
                szType = "OPT";
                break;
            case 'J':
            case 'K':
            case 'F':
                szType = "FUT";
                break;
            default:
                szType = "CS";
                break;
            }
            std::string szRetailAcct = FIXServer::getInst()->GetRetailAccount( AccountID(szTemp,szType) );
            szTemp += " "+szRetailAcct;
        }
        catch(...)
        {
            Log::Write( LOG_ERROR, "Account not found in the account map.");
        }
        set( FIX::Account( szTemp ) );
        szTemp = status.GetTrader();
        set( FIX::ClientID( szTemp ) );
    }
    switch( status.GetSecurityType() )
    {
    case 'E':
        {
            set( FIX::SecurityType( FIX::SecurityType_COMMON_STOCK )  );
        }
        break;
    case 'O':
    case 'I':
        {
            try
            {
                set( FIX::SecurityType( FIX::SecurityType_OPTION ) );
                char cType = status.GetOptionType();
                if( cType == 'C' )
                    set( FIX::PutOrCall( FIX::PutOrCall_CALL ) );
                else if( cType == 'P' )
                    set( FIX::PutOrCall( FIX::PutOrCall_PUT ) );
                
                szTemp = status.GetExpirationDate();
                if( szTemp.length() > 0 )
                    set( FIX::MaturityMonthYear( szTemp ) );
                set( FIX::StrikePrice( status.GetStrikePrice() ) );
                
                if( status.GetOpenClose() == 1 )
                    set( FIX::OpenClose( FIX::OpenClose_OPEN ) );
                else if( status.GetOpenClose() == 0 )
                    set( FIX::OpenClose( FIX::OpenClose_CLOSE ) );
                if( status.GetUnderlying().length() > 0 )
                    set( FIX::SymbolSfx(status.GetUnderlying()) );
            }
            catch(...)
            {
                Log::Write( LOG_ERROR, "Missing required fields.");
            }
        }
        break;
    case 'F':
    case 'J':
        {
            set( FIX::SecurityType( FIX::SecurityType_FUTURE )  );
            try
            {
                szTemp = status.GetExpirationDate();
                if( szTemp.length() > 0 )
                    set( FIX::MaturityMonthYear( szTemp ) );
                if( status.GetUnderlying().length() > 0 )
                    set( FIX::SymbolSfx(status.GetUnderlying() ) );
            }
            catch(...)
            {
                Log::Write( LOG_ERROR, "Missing required fields.");
            }
            
        }
        break;
    default:
        break;
    }
    
    std::stringstream stm;
    stm << status.GetExchange();
    set( FIX::LastMkt( stm.str() ) );
    int nYear, nHour, nMinute, nSecond;
    char szBuf[128];
    strcpy( szBuf, status.GetExecutionTime().c_str() );
    if( !status.GetExecutionTime().empty() )
    {
        sscanf( szBuf, "%d-%d:%d:%d", &nYear, &nHour, &nMinute, &nSecond ); 
        set( FIX::TransactTime( UtcTimeStamp( nHour,nMinute,nSecond ) ) );
    }
    std::string text = status.GetText();
    if( text.size() > 0 && text != "()" )
        set( FIX::Text(status.GetText()) );
}

OrderCancelRejectFromOM::OrderCancelRejectFromOM( const OM::Status& status )
{
    setSessionID( FIXServer::clientSessionID() );
    
    if( status.GetActionID().size() > 0 )
        set( FIX::ClOrdID(status.GetActionID()) );
    
    if( status.GetTag().size() > 0 )
        set( FIX::OrigClOrdID(status.GetTag()) );
    
    if( status.GetFirmTag().size() > 0 ) {
        set( FIX::OrderID(status.GetFirmTag()) );
    } else {
        set ( FIX::OrderID("NONE"));
        set (FIX::CxlRejReason(1));
    }
    
    
    set( TranslationManager::TranslateStatus( status.GetOrderStatus() ) );
    
    if( status.GetStatus() == OM::Status_CancelRejected )
        set( FIX::CxlRejResponseTo('1') );
    else
        set( FIX::CxlRejResponseTo('2') );
    
    std::string text = status.GetText();
    if( text.size() > 0 && text != "()" )
        set( FIX::Text(status.GetText()) );
    
}
