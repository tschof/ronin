#ifndef _ROMVALUEDEFS_H__
#define _ROMVALUEDEFS_H__



namespace ROMVALUE
{
static const char MESSAGE_END = '\n';
static const char DELIMITER = ',';

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


//Command
static const char	COMMAND_LOGON				= 'L';
static const char	COMMAND_LOGOUT				= 'O';
static const char	COMMAND_LOGONREJECT			= 'K';
static const char	COMMAND_ORDER				= 'E';
static const char   COMMAND_CPLXORDER           = 'G';
static const char	COMMAND_CANCEL				= 'C';
static const char	COMMAND_CANCELS				= 'c';
static const char	COMMAND_REPLACE				= 'R';
static const char	COMMAND_HEARTBEAT			= 'H';
static const char	COMMAND_RESPONSETOADMIN		= 'J';
static const char	COMMAND_DISCONNECTDEST		= 'q';
static const char	COMMAND_RESETDESTSEQUENCES  = 't';
static const char	COMMAND_RELOADDESTCONFIG	= 'P';
static const char	COMMAND_STATUS				= 'S';
static const char	COMMAND_ZAP					= 'Z';
static const char	COMMAND_ADMIN				= 'N';
static const char	COMMAND_TEXT				= '@';
static const char	COMMAND_REJECT				= 'x';
static const char   COMMAND_QUERY               = 'Q';
static const char   COMMAND_STRATEGY_QUERY      = '~';
static const char   COMMAND_STRATEGY_RESPONSE   = 'T';

//Side
static const short	SIDE_BUY					= 1;
static const short	SIDE_SELL					= 2;
static const short	SIDE_BUYMINUS				= 3;
static const short	SIDE_SELLPLUS				= 4;
static const short	SIDE_SHORT					= 5;
static const short	SIDE_EXEMPT					= 6;
static const short	SIDE_CROSS					= 8;
static const short	SIDE_CROSSBUY				= 9;
static const short	SIDE_CROSSSELL				= 10;

//Type
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
static const short    TYPE_VWAP                   = 14;

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
static const short OATS_REVOVER					= 16;
static const short HOSTCONNECT_STATUS_QUERY		= 17;
static const short DISCONNECT_HOST				= 18;
static const short CLIENTCONNECT_STATUS_QUERY	= 19;
static const short BLOCK_USER					= 20;

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
static const short    TIF_DAY     = 0;
static const short    TIF_GTC     = 1;
static const short    TIF_OPG     = 2;
static const short    TIF_IOC     = 3;
static const short    TIF_FOK     = 4;
static const short    TIF_GTX     = 5;
static const short    TIF_GTD     = 6;

//DropCopy Type
static const DWORD			DROPCOPY_ALL		= 2;
static const DWORD			DROPCOPY_FILLONLY	= 1;

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
};

#endif //_ROMVALUEDEFS_H__