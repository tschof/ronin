#if !defined(Fields_H)
#define Fields_H

#define	CSV_NUM_FIELDS			58
#define CSV_NUM_HIST_FIELDS		16

// common to all messages
#define CSV_COMMAND				0
#define CSV_TIME				1
#define CSV_TRADER				2
#define CSV_TAG					3		// client assigned tag to id order
#define CSV_SIDE				4
#define CSV_SYMBOL				5
#define CSV_SHARES				6
#define CSV_PRICE				7
#define CSV_TYPE				8
#define CSV_TIF					9
#define CSV_CAPACITY			10		// principal or agent      'A'
#define CSV_CLEARINGID			11		// 4 digit nscc clearing code  0551
#define CSV_CLEARINGACCOUNT		12		// account number a above clearing firm  'ISI table'
#define CSV_EXCHANGE			13		// destination
#define CSV_STATUS				14		// current status

// just order entries
#define CSV_INSTRUCTIONS		15		// primarily used by instinet		 'customer Id in ISI table?'
#define CSV_PRICECHECK			16		// tells exchange whether or not to do price check  'N'
//#define CSV_FLOOR				17		// display size		''
#define CSV_MINQTY				18		// min exec size	''
#define CSV_STOPPRICE			19		
#define CSV_DISCRETIONARY		20		//  'N'
#define CSV_SOLICITED			21		//	'N'
#define CSV_LOCALACCOUNT		22		//  'ISI table'
#define CSV_FIRM				23		//  'ISI table'
#define CSV_TRADE_FOR			24		// trader the trade is being done for (i.e. CSV_TRADER doing trade on CSV_TRADE_FOR behalf) 'trader acronym'
#define CSV_QUERY_TAG			25		// tag used for communications between OM and the DB Server							  ''
#define CSV_DBSYMBOL			26		// Database symbol to be used by recorder
#define CSV_FLOOR				34		// Max Floor Fix tag 111

// CSV_OWNER must not be overwritten (field 27)

#define CSV_MULTIPLIER			28		// multiplier for futures
#define CSV_EXDEST				29		// ExDestination
#define CSV_EXPDATE				30		// expiration date for ORDERS
#define CSV_CALLPUT				31
#define CSV_STRIKEPRICE			32
#define CSV_UNDERLYING			33

#define CSV_SECURITY_TYPE		35		// option, future or equity
#define CSV_MM_ACCOUNT			36		// market maker account
#define CSV_MM_EXCHANGE			37		// market maker home exchange
#define CSV_OPENCLOSE			38		// Open a new position or close existing position, open is 1, close is 0
#define CSV_TRADING_SESSION		39		// pre-open or primary
#define CSV_GIVEUPACCOUNT		40		
#define	CSV_ORDERDEST			41		// Where to send order
#define CSV_ACCOUNT				45
#define CSV_CMTAACCOUNT			46
#define CSV_GTD_DATE			47
#define CSV_CUM_SHARES			48
#define CSV_LEAVE_SHARES		49
#define CSV_EXEC_TIME			50		// The time the transaction occurred(set by exchange)
#define CSV_AVG_PRICE			51
#define CSV_OM_TIME				52		// The time OM sends message to client
#define CSV_CLIENT_TIME			53		// The time client sends message to OM
#define CSV_EXEC_TAG			54		// Exchange(not our exchange server) execution tag
#define CSV_DBSYMBOL_NEW		55		// New field for DBSymbol to resolve the conflict with TEXT field
#define CSV_CURRENCY			56		// Currency

#define CSV_EXEC_INSTRUCTION	57		// Fix ExecInst
#define CSV_SHORT_LENDER		58
#define CSV_ACTTAG				59
#define CSV_NEW_TAG				60
#define CSV_ORDER_STATUS		61

// just status messages
#define CSV_ORDER_TAG			15		// tag assigned by lowest client
#define CSV_EXCHANGE_TAG		16		// tag assigned by exchange
#define CSV_STAFFORD_TAG		17		// tag assigned by order manager
#define CSV_CANCEL_TAG			18		// tag assigned to cancel by server connected to exchange
#define CSV_EXEC_CONFIRM		20		// confirm from exchane
#define CSV_REFERENCE			21		// execution lookup tag from exchange
#define CSV_REPORT				22		// do act report?
#define CSV_CONTRA				23		// contra
#define CSV_TEXT				26		// reject or status text returned by exchange
#define CSV_OWNER				27		// owner of the message
#define CSV_OMEXECUTION_TAG		28
#define CSV_EXEC_SHARES			42		// Executed shares
#define CSV_LAST_SHARES			42		// Executed shares
#define CSV_ORIGINAL_SHARES		43		
#define CSV_EXEC_PRICE			44		// Execution price
#define CSV_LAST_PRICE			44
#define CSV_OWNERCANCEL			28		// Indicate owner canceled this order

// login messages
#define CSV_CANCEL_BOOL			3
#define CSV_CANCEL_MIN			4
#define CSV_APP					5
#define CSV_APP_VERSION			6
#define CSV_PASSWORD			7

// Modify Fields
#define CSV_MOD_MAXSHARES		3
#define CSV_MOD_BUYINGPOWER		4
#define CSV_MOD_MAXORDERS		5
#define CSV_MOD_MAXDOLLARS		6
#define CSV_MOD_MAXSHARESORDER	7

// Email Fields
#define CSV_EMAIL_MESSAGE		3
#define CSV_EMAIL_LIST			4

// Option Fields
#define	CSV_CONTRA_MEMBER_ACR		15
#define	CSV_CONTRA_CLEARING_ACCOUNT	16
#define	CSV_EXPIRATION_DATE			17
#define	CSV_STRIKE					18
#define	CSV_CALL_PUT				19
#define	CSV_IS_RESEND				20

// Error Fields
#define	CSV_ERROR1					15
#define	CSV_ERROR2					16
#define	CSV_ERROR3					17

// Historical Query
#define CSV_HIST_TYPE				3
#define CSV_HIST_SYMBOL				4
#define CSV_HIST_SECONDS			5
#define CSV_HIST_BIDTIMES			6
#define CSV_HIST_BIDPRICES			7
#define CSV_HIST_BIDSIZES			8
#define CSV_HIST_ASKTIMES			9
#define CSV_HIST_ASKPRICES			10
#define CSV_HIST_ASKSIZES			11
#define CSV_HIST_LASTTIMES			12
#define CSV_HIST_LASTPRICES			13
#define CSV_HIST_LASTSIZES			14
#define CSV_HIST_REPLYSUBJECT		15

// Seperators
#define MAIL_LIST_SEPARATOR			':'
#define HIST_LIST_SEPARATOR			'!'
#define SPREAD_ORDER_SEPARATOR		'#'

#endif