

#ifndef _ROMVALUEDEFS_H__
#define _ROMVALUEDEFS_H__



namespace ROMVALUE
{

    enum SECINDEX
    {
        EQUITY = 0,
        OPTION,
        OPTIONONFUTURE,
        FUTURE,
        SECNUM
    };

    enum SIDEINDEX
    {
        BUY = 0,
        SELL,
        SIDENUM
    };

static const std::string s_SecStr[SECNUM] = {"Equity", "Option", "OptionOnFuture", "Future"};
/*static bool SecStr(unsigned short secIndex, std::string& secStr)
{
    bool rc = false;
    if (secIndex < SECNUM)
    {
        secStr = s_SecStr[secIndex];
        rc = true;
    }
    return rc;
}*/

static const char MESSAGE_END = '\n';
static const char FIELDSEPARATOR = ',';
static const char VALUEDELIMITER = ';';
static const char ISO = 'f';

//static const short  MBTYPE_ORDERREQUEST			= 0;
//static const short	MBTYPE_ORDERREJECT			= 1;
//static const short  MBTYPE_ORDERAPPROVED		= 2;

// order status
static const short	STATUS_NEW 					= 0;
static const short	STATUS_PARTIALLYFILLED		= 1;
static const short	STATUS_FILLED				= 2;
static const short	STATUS_DONEFORDAY			= 3;
static const short	STATUS_CANCELED				= 4;
static const short	STATUS_REPLACED				= 5;
static const short	STATUS_PENDINGCANCEL		= 6;
static const short	STATUS_STOPPED				= 7;
static const short	STATUS_ORDERREJECTED		= 8;
static const short	STATUS_SUSPENDED			= 9;
static const short	STATUS_PENDINGNEW			= 10;
static const short	STATUS_CALCULATED			= 11;
static const short	STATUS_EXPIRED				= 12;
static const short	STATUS_OPEN					= 13;
static const short	STATUS_CANCELREJECTED		= 14;
static const short	STATUS_CORRECTED			= 15;
static const short	STATUS_BUSTED				= 16;
static const short	STATUS_PARTIALLYCANCELLED	= 17;
static const short	STATUS_PARTIALLYREJECTED	= 18;
static const short	STATUS_QUOTEREQUEST			= 19;
static const short	STATUS_REPLACEREJECTED		= 20;
static const short	STATUS_REPLACEPENDING		= 21;
static const short	STATUS_EXPOSED				= 22;
static const short	STATUS_DIRECTEDTHRUIML		= 23;
static const short	STATUS_DIRECTED				= 24;
static const short	STATUS_DIRECTEDREJECT		= 25;
static const short  STATUS_ROMCANCELPENDING     = 26;
static const short  STATUS_ROMREPLACEPENDING    = 27;
static const short  STATUS_ROMCANCELREJECTED    = 29;
static const short  STATUS_ROMREPLACEREJECTED   = 30;
static const short  STATUS_REJECT               = 28;
static const short  STATUS_FASTCANCELPENDING    = 66;
static const short  STATUS_FASTREPLACEPENDING   = 67;

//Command
static const char   COMMAND_LOGON				= 'L';
static const char   COMMAND_LOGOUT				= 'O';
static const char   COMMAND_LOGONREJECT			= 'K';
static const char   COMMAND_ORDER				= 'E';
static const char   COMMAND_CANCEL				= 'C';
static const char   COMMAND_CANCELS				= 'c';
static const char   COMMAND_REPLACE				= 'R';
static const char   COMMAND_HEARTBEAT			= 'H';
static const char   COMMAND_RESPONSETOADMIN		= 'J';
//static const char   COMMAND_DISCONNECTDEST		= 'q';
static const char   COMMAND_RESETDESTSEQUENCES  = 't';
static const char   COMMAND_RELOADDESTCONFIG	= 'P';
static const char   COMMAND_STATUS				= 'S';
static const char   COMMAND_CURRENT_STATUS      = 's';
static const char   COMMAND_ZAP					= 'Z';
static const char   COMMAND_ZAP_ALL             = 'z';
static const char   COMMAND_ADMIN				= 'N';
static const char   COMMAND_TEXT				= '@';
static const char   COMMAND_REJECT				= 'x';
static const char   COMMAND_ORDER_QUERY         = 'q';
static const char   COMMAND_ORDER_QUERY_END     = 'Q';
static const char   COMMAND_STRATEGY_QUERY      = '~';
static const char   COMMAND_STRATEGY_RESPONSE   = 'T';
static const char   COMMAND_TIMER               = 'G';
static const char   COMMAND_GAPFILL             = 'g';
static const char   COMMAND_DONEGAPFILL         = 'd';
static const char   COMMAND_DONEOPENHISTORYFILE = 'h';
static const char   COMMAND_FILEROLL            = 'r';
static const char   COMMAND_MESSAGE_QUERY       = 'F';
static const char   COMMAND_PASSWORD_UPDATE     = 'p';
static const char   COMMAND_PASSWORD_UPDATE_REJECT     = 'f';
static const char   COMMAND_POSITION_UPDATE     = 'P';
static const char   COMMAND_OPENPOSITION_QUERY  = 'U';


//Side
static const short  SIDE_MAX                    = 5;
static const short	SIDE_BUY					= 1;
static const short	SIDE_SELL					= 2;
static const short	SIDE_BUYMINUS				= 3;
static const short	SIDE_SELLPLUS				= 4;
static const short	SIDE_SHORT					= 5;
static const short	SIDE_EXEMPT					= 6;
static const short	SIDE_CROSS					= 8;
static const short	SIDE_CROSSBUY				= 9;
static const short	SIDE_CROSSSELL				= SIDE_MAX;
static std::string  SIDENAME[SIDE_MAX + 1] = { "Invalid", "Buy", "Sell", "BuyMinus",
                                                "SellPlus", "Short"};


//Type
static const short TYPE_MAX = 14;
static const short    TYPE_MARKET                 = 1;
static const short    TYPE_LIMIT                  = 2;
static const short    TYPE_STOP                   = 3;
static const short    TYPE_STOPLIMIT              = 4;
static const short    TYPE_MARKETONCLOSE          = 5;
static const short    TYPE_WITHORWITHOUT          = 6;
static const short    TYPE_LIMITORBETER           = 7;
static const short    TYPE_LIMITWITHORWITHOUT     = 8;
static const short    TYPE_ONBASIS                = 9;
static const short    TYPE_ONCLOSE                = 10;
static const short    TYPE_LIMITONCLOSE           = 11;
static const short    TYPE_MARKETATOPEN           = 12;
static const short    TYPE_MARKETLIMIT            = 13;
static const short    TYPE_VWAP                   = TYPE_MAX;

static const std::string TYPENAME[TYPE_MAX + 1] = { "Invalid", "Market", "Limit", "Stop", "StopLimit",
                                                    "MarketOnClose", "WithOrWithout", "LimitOrBetter",
                                                    "LimitWithOrWithout", "OnBasis", "OnClose",
                                                    "LimitOnClose", "MarketOnOpen", "MarketLimit",
                                                    "VWAP" };

//Admin Commands
static const short QUERY_ORDERDEST_STATUS		= 0;
static const short MODIFY_ORDERDEST_ACCESS		= 1;
static const short QUERY_USER_STATUS			= 2;
static const short DISCONNECT_USER				= 3;
static const short RELOAD_DATABASE				= 4;
static const short EXEMPT_TRADER_LIMITS			= 5;
static const short ADD_EXCEPTION_SYMBOLS		= 6;
static const short RELOAD_ORDER_DESTINATIONS	= 7;
static const short TRADER_LOGONSTATUS			= 8;
static const short QUERY_TRADER_LOGONS			= 9;
static const short DISABLE_OATS					= 10;
static const short DESTROUTE_UPDATE				= 11;
static const short QUERY_DESTROUTES				= 12;
static const short RELOAD_DEST_IPPORT			= 13;
static const short QUERY_OATS					= 14;
static const short ROM_INSTANCES_UPDATE			= 15;
static const short OATS_RECOVER					= 16;
static const short HOSTCONNECT_STATUS_QUERY		= 17;
static const short DISCONNECT_HOST				= 18;
static const short CLIENTCONNECT_STATUS_QUERY	= 19;
static const short BLOCK_USER					= 20;
static const short DISCONNECT_ROM               = 21;
static const short DROPCONNECATION_STATUS_QUERY = 22;
static const short ROM_DISCONNECT_FROM_OATS     = 23;
static const short TRADER_LIMITS_STATUS = 24;
static const short CLEARINGACT_SEC_LIMITS_STATUS = 25;
static const short RELOAD_CLEARINGACT_SEC_LIMITS = 26;
static const short EXCHANGEINTERFACE_STATUS_QUERY = 27;
static const short ADMIN_INSTANCES_UPDATE       = 28;
static const short DISCONNECT_ADMIN				= 29;
static const short DISCONNECT_DROP              = 30;
static const short RELOAD_DROPFIRMS             = 31;
static const short ADD_ROM                      = 32;
static const short DEST_RELOGON                 = 33;
static const short EXEMPT_TRADER_MSGRATE        = 35;
static const short EXEMPT_PRICE_CHECK           = 36;
static const short BILLINGGRP_LIMIT_STATUS      = 37;
static const short RELOAD_BILLINGGRP_LIMIT      = 38;
static const short POSITION_QUERY               = 39;
static const short LENDER_LIMIT_QUERY           = 40;
static const short RELOAD_LENDER_LIMIT          = 41;
static const short RELOAD_INIT_POSITION         = 42;
static const short GET_LENDER_GROUPS            = 43;
static const short UPDATE_POSITION              = 44;
static const short UPDATE_LENDING_LIMIT         = 45;
static const short EASYBORROW_QUERY             = 46;
static const short RELOAD_EASYBORROW            = 47;
static const short ADD_EASYSYMBOL               = 48;
static const short THRESHOLD_QUERY              = 49;
static const short RELOAD_THRESHOLD             = 50;
static const short ADD_THRESHOLD                = 51;
static const short EXEMPT_CLRACT_LIMITS         = 52;
static const short EXEMPT_BGP_LIMITS            = 53;
static const short RELOAD_FUTURE                = 54;
static const short QUERY_FUTURE                 = 55;
static const short CLR_LENDING_LIMIT_QUERY      = 56;

//Security Types
static const char	SECTYPE_EQUITY				= 'E';
static const char	SECTYPE_FUTURE				= 'F';
static const char	SECTYPE_OPTION				= 'O';
static const char	SECTYPE_CURRENCY			= 'X';
static const char	SECTYPE_OPTIONINDEX			= 'I';
static const char	SECTYPE_OPTIONFUTURE		= 'M';
static const char	SECTYPE_FUTUREEQUITY		= 'J';
static const char	SECTYPE_FUTUREINDEX			= 'K';
static const char	SECTYPE_FUTURECURRENCY		= 'L';
static const char	SECTYPE_COMPLEX				= 'C';

//TIF
static const short    TIF_MAX     = 7;
static const short    TIF_DAY     = 0;
static const short    TIF_GTC     = 1;
static const short    TIF_OPG     = 2;
static const short    TIF_IOC     = 3;
static const short    TIF_FOK     = 4;
static const short    TIF_GTX     = 5;
static const short    TIF_GTD     = 6;
static const short    TIF_EXDAY   = TIF_MAX;

static const std::string TIFNAME[TIF_MAX + 1] = { "DAY", "GTC", "OPG", "IOC", "FOK", "GTX", "GTD", "EXDAY" };

//DropCopy Type
static const short			DROPCOPY_ALL		= 2;
static const short			DROPCOPY_FILLONLY	= 1;

//Complex order type
static const short    CPLXTYPE_SINGLE		 = 0;
static const short    CPLXTYPE_BASKET		 = 1;
static const short    CPLXTYPE_CROSS		 = 2;
static const short    CPLXTYPE_SPREAD		 = 3;
static const short    CPLXTYPE_CBOELEG		 = 4;
static const short	  CPLXTYPE_FUTURE_SPREAD = 5;
static const short	  CPLXTYPE_PRODUCT_SPREAD = 5;
static const short	  CPLXTYPE_LEG			 = 6;

//Algo Order Types
static const short		ALGO_NON		= 0;
static const short		ALGO_TWAP		= 1;
static const short		ALGO_VWAP		= 2;

//Client Type
static const short    CLIENTTYPE_ROMDIRECT = 0;
static const short    CLIENTTYPE_FIX       = 1;

//Rom internal
static const short	  OATS_DEST_ID	= -1;

//empty command value
static const short	  CMD_EMPTY = 44;

//leg separator for user defined complex order
static const std::string LEG_SEPRATOR = "!#!";
static const std::string DELIMITER = ",";
};

#endif //_ROMVALUEDEFS_H__
