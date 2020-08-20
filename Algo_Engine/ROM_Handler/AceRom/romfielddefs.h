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
#ifndef _ROMFIELDDEFS_H__
#define _ROMFIELDDEFS_H__

#include <string>

namespace ROMFIELDS {
    
    static const int	CSV_NUM_FIELDS		=	80;
    static const int CSV_NUM_HIST_FIELDS	=	16;
    // common to all messages;
    static const int CSV_COMMAND=0;
    static const int CSV_TIME=1;
    static const int CSV_TRADER=2;
    static const int CSV_TAG=3		;// client assigned tag to id order;
    static const int CSV_SIDE	=			4;
    static const int CSV_SYMBOL	=			5;
    static const int CSV_SHARES	=			6;
    static const int CSV_PRICE	=			7;
    static const int CSV_TYPE	=			8;
    static const int CSV_TIF	=				9;
    static const int CSV_CAPACITY		=	10		;// principal or agent      'A';
    static const int CSV_CLEARINGID		=	11		;// 4 digit nscc clearing code  0551;
    static const int CSV_CLEARINGACCOUNT=		12		;// account number a above clearing firm  'ISI table';
    static const int CSV_EXCHANGE		=	13		;// destination;
    static const int CSV_STATUS			=	14		;// current status;
    
    // just order entries;
    static const int CSV_INSTRUCTIONS	=	15		;// primarily used by instinet		 'customer Id in ISI table?';
    static const int CSV_PRICECHECK		=	16		;// tells exchange whether or not to do price check  'N';
    //static const int CSV_FLOOR				17		;// display size		'';
    static const int CSV_MINQTY			=	18		;// min exec size	'';
    static const int CSV_STOPPRICE		=	19		;
    static const int CSV_DISCRETIONARY	=	20		;//  'N';
    static const int CSV_SOLICITED		=	21		;//	'N';
    static const int CSV_LOCALACCOUNT	=	22		;//  'ISI table';
    static const int CSV_FIRM			=	23		;//  'ISI table';
    static const int CSV_TRADE_FOR		=	24		;// trader the trade is being done for (i.e. CSV_TRADER doing trade on CSV_TRADE_FOR behalf) 'trader acronym';
    static const int CSV_QUERY_TAG		=	25		;// tag used for communications between OM and the DB Server							  '';
    static const int CSV_DBSYMBOL		=	26		;// Database symbol to be used by recorder;
    
    // CSV_OWNER must not be overwritten (field 27);
    
    static const int CSV_MULTIPLIER		=	28		;// multiplier for futures;
    static const int CSV_EXDEST			=	29		;// ExDestination;
    static const int CSV_EXPDATE		=		30		;// expiration date for ORDERS;
	static const int CSV_EXPYEARMONTH	=		30		;// expiration date for ORDERS;
    static const int CSV_CALLPUT		=		31;
    static const int CSV_STRIKEPRICE	=		32;
    static const int CSV_UNDERLYING		=	33;
    
    static const int CSV_FLOOR			=	34		;// Max Floor Fix tag 111;
    
    static const int CSV_SECURITY_TYPE	=	35		;// option, future or equity;
    static const int CSV_MM_ACCOUNT		=	36		;// market maker account;
    static const int CSV_MM_EXCHANGE	=		37		;// market maker home exchange;
    static const int CSV_OPENCLOSE		=	38		;// Open a new position or close existing position, open is 1, close is 0;
    static const int CSV_TRADING_SESSION=		39		;// pre-open or primary;
    static const int CSV_GIVEUPACCOUNT	=	40		;
    static const int	CSV_ORDERDEST	=41		;// Where to send order;
    static const int CSV_ACCOUNT	=			45;
    static const int CSV_CMTAACCOUNT	=		46;
    static const int CSV_GTD_DATE		=	47;
    static const int CSV_CUM_SHARES		=	48;
    static const int CSV_LEAVE_SHARES	=	49;
    static const int CSV_EXEC_TIME		=	50		;// The time the transaction occurred(set by exchange);
    static const int CSV_AVG_PRICE		=	51;
    static const int CSV_OM_TIME		=		52		;// The time OM sends message to client;
    static const int CSV_CLIENT_TIME	=		53		;// The time client sends message to OM;
    static const int CSV_EXEC_TAG		=	54		;// Exchange(not our exchange server) execution tag;
    static const int CSV_SEC_DEF	=	55		;
    static const int CSV_CURRENCY		=	56		;// Currency;
    
    static const int CSV_EXEC_INSTRUCTION=	57		;// Fix ExecInst;
    static const int CSV_SHORT_LENDER	=	58;
    static const int CSV_ACTTAG			=	59;
    static const int CSV_NEW_TAG		=		60;
    static const int CSV_ORDER_STATUS	=	61;
    static const int CSV_TRADE_ID		=	62;
	static const int CSV_MATURITYDAY	=	62;
    static const int CSV_REJECT_BY		=	63;
    static const int CSV_PROGRAM_TRADING	=	64;
	static const int CSV_PEGPRICE_OFFSET	=	64;
    static const int CSV_CPLXORDER_TYPE	=	66;
    static const int CSV_PARENT_TAG		=	65;
    static const int CSV_ECHO			=	67;
    static const int CSV_EFFECTIVE_TIME	=	68;
    static const int CSV_BILLING_CODE	=	69;
    static const int CSV_PARENT_STAFFORD_TAG =70;
	static const int CSV_RESERVE = 73;
    static const int CSV_OATSMULTIPLIER =	79;
    // For Cross order;
    static const int CSV_OUTGOING_SEQ =	71;
    static const int CSV_LASTRECEIVED_SEQ	=	72;
	static const int CSV_NEXTEXPECTED_SEQ	=	72;
    static const int CSV_MGF			=		73;
	static const int CSV_ALGOTYPE		=		73;
    static const int CSV_REGULATION_ID	=	74;
	static const int CSV_CLIENT_ECHO	=	74;
	static const int CSV_OATSMSG_SEQ	=	74;
    static const int CSV_PASSTHROUGH	=		75;

	static const int CSV_START_TIME = 76;
	static const int CSV_END_TIME = 77;
	static const int CSV_SEC_ID = 78;
    
    // just status messages;
    static const int CSV_ORDER_TAG		=	15		;// tag assigned by lowest client;
    static const int CSV_EXCHANGE_TAG	=	16		;// tag assigned by exchange;
    static const int CSV_STAFFORD_TAG	=	17		;// tag assigned by order manager;
    static const int CSV_CANCEL_TAG		=	18		;// tag assigned to cancel by server connected to exchange;
    static const int CSV_EXEC_CONFIRM	=	20		;// confirm from exchane;
    static const int CSV_REFERENCE		=	21		;// execution lookup tag from exchange;
    static const int CSV_REPORT			=	22		;// do act report?;
    static const int CSV_CONTRA			=	23		;// contra;
    static const int CSV_TEXT			=	26		;// reject or status text returned by exchange;
    static const int CSV_OWNER			=	27		;// owner of the message;
    static const int CSV_OMEXECUTION_TAG	=	28;
    static const int CSV_EXEC_SHARES		=	42		;// Executed shares;
    static const int CSV_LAST_SHARES		=	42		;// Executed shares;
    static const int CSV_ORIGINAL_SHARES	=	43		;
    static const int CSV_EXEC_PRICE		=	44		;// Execution price;
    static const int CSV_LAST_PRICE		=	44;
    static const int CSV_RPT_FIRM		=	45		;// Reporting firm  ;//WXU, C1, 09/19/05;
    
    static const int CSV_OWNERCANCEL	=		28		;// Indicate owner canceled this order;
    
    // login messages;
    static const int CSV_CANCEL_BOOL	=		3;
    static const int CSV_CANCEL_MIN		=	4;
    static const int CSV_APP			=		5;
    static const int CSV_APP_VERSION	=		6;
    static const int CSV_PASSWORD		=	7;
    static const int CSV_GAPFILL_ENALBED = 8;
	static const int CSV_NEWPASSWORD	= 9;

    // Modify Fields;
    static const int CSV_MOD_MAXSHARES	=	3;
    static const int CSV_MOD_BUYINGPOWER=		4;
    static const int CSV_MOD_MAXORDERS	=	5;
    static const int CSV_MOD_MAXDOLLARS	=	6;
    static const int CSV_MOD_MAXSHARESORDER	=7;
    
    // Email Fields;
    static const int CSV_EMAIL_MESSAGE	=	3;
    static const int CSV_EMAIL_LIST		=	4;
    
    // Option Fields;
    static const int	CSV_CONTRA_MEMBER_ACR	=	15;
    static const int	CSV_CONTRA_CLEARING_ACCOUNT	=16;
    static const int	CSV_EXPIRATION_DATE		=	17;
    static const int	CSV_STRIKE				=	18;
    static const int	CSV_CALL_PUT			=	19;
    static const int	CSV_IS_RESEND			=	20;

    static const int CSV_POS_DUP = 78;
    static const int CSV_ORD_TYPE =77;
    static const int CSV_ROM_CLIENT = 79;
    
	//ControlFreak
	static const int CSV_COMMAND_FREAK			= 2;
	static const int CSV_EXCHANGE_FREAK			= 4;
	static const int CSV_ACTION_FREAK			= 5;
	static const int CSV_TRADER_FREAK			= 3;
	static const int CSV_EXCEPTIONSYMBOL_FREAK	= 3;
	static const int CSV_SERVER_FREAK			= 6;
	static const int CSV_CLIENT_FREAK			= 7;
	static const int CSV_ORDERDEST_FREAK		= 8;
	static const int CSV_EXCLUDED_OWNERS		= 9;
	static const int CSV_ROM_INSTANCES			= 10;
	static const int CSV_INCLUDED_DESTS			= 11;
	static const int CSV_RECOVER_OPTION			= 12;

	//SecurityDefinition Responses
	static const int CSV_NUM_OF_DECIMAL_PRICE = 70;
	static const int CSV_NUM_OF_DECIMAL_QTY = 71;
	static const int CSV_BASE_NUM_LOTS = 72;
	static const int CSV_NUM_BLOCKS = 73;
	static const int CSV_TICK_VALUE = 74;
	static const int CSV_PRODUCT_ID = 75;
	static const int CSV_CLEARED_ALIAS = 76;
	static const int CSV_DENOMINATOR = 77;
	static const int CSV_MAX_STRIKE = 69;
	static const int CSV_MIN_STRIKE = 79;
	static const int CSV_INCREMENT_STRIKE = 15;
	static const int CSV_SECONDARY_LEG_SYMB = 16;
	static const int CSV_PRIMARY_LEG_SYMB = 17;
	static const int CSV_IMPLIED_TYPE = 18;
	static const int CSV_PRODUCT_TYPE = 19;
	static const int CSV_LOT_MULTIPLIER = 20;
	static const int CSV_INITIAL_MARGIN = 21;
	static const int CSV_NUM_DECIMAL_PRICE = 22;
	static const int CSV_NUM_DECIMAL_QTY = 23;
	static const int CSV_LOT_SIZE = 24;
	static const int CSV_SECURITY_TRADING_STATUS = 25;
	static const int CSV_UNDERLYING_SECURITY_DES = 26;
	static const int CSV_UNDERLYING_MATURITY_DAY = 28;
	static const int CSV_SECURITY_REQ_ID = 29;

	//User Response Messages
	//FIX - 336
	static const int CSV_TRADING_SESSION_ID = 70;
	//FIX - 923
	static const int CSV_USER_REQUEST_ID = 71;
	//FIX - 553
	static const int CSV_USERNAME = 72;
	//FIX - 926
	static const int CSV_USER_STATUS = 73;
	//FIX - 927
	static const int USER_STATUS_TEXT = 74;

	///Trade Capture Report Fields
	//FIX - 573
	static const int CSV_MATCH_STATUS = 63;

	//Trade Session Status Fields
	//FIX - 340
	static const int CSV_SESSION_STATUS = 73;





	

    // Seperators;
    static const int MAIL_LIST_SEPARATOR		=	':';
    static const int HIST_LIST_SEPARATOR		=	'!';
    static const int SPREAD_ORDER_SEPARATOR	=	'#';
    
    static const std::string ROM_EQUITY = "E";
    static const std::string ROM_FUT = "F";
    static const std::string ROM_OPT = "O";

	static const std::string ROM_AGENT = "A";
	static const std::string ROM_PRINCIPAL = "P";
	static const std::string ROM_MARKET_MAKER = "S";
	static const std::string ROM_MARKET_MAKER_FIRM = "M";
	static const std::string ROM_BROKER_DEALER = "R";
	static const std::string ROM_BROKER_DEALER_FIRM = "B";
	static const std::string ROM_CUSTOMER = "C";
}

#endif