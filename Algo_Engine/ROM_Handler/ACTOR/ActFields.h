#define CSV_NUM_ACT_FIELDS			25

#define CSV_ACT_COMMAND_ENTER		'E'
#define CSV_ACT_COMMAND_STATUS		'S'

// First Generation Obsolete
//#define CSV_ACT_MMENTRY_TYPE		'M'
//#define CSV_ACT_OEENTRY_TYPE		'O'

#define CSV_ACT_MMENTRY_TYPE		'V'
#define CSV_ACT_OEENTRY_TYPE		'W'
#define CSV_ACT_TCEN_TYPE			"TCEN"
#define CSV_ACT_TCAL_TYPE			"TCAL"
#define CSV_ACT_TCLK_TYPE			"TCLK"
#define CSV_ACT_REJ_TYPE			"REJ"
#define CSV_ACT_TTEN_TYPE			"TTEN"
#define CSV_ACT_CTEN_TYPE			"CTEN"
#define CSV_ACT_TTAL_TYPE			"TTAL"
#define CSV_ACT_TCAL_TYPE			"TCAL"
#define CSV_ACT_CTAL_TYPE			"CTAL"

// common to all messages
#define CSV_ACT_COMMAND				0
#define CSV_ACT_TIME				1
#define CSV_ACT_TYPE				2
#define CSV_ACT_TRADER				3		// use for branch
#define CSV_ACT_TAG					4		// om assigned tag (use in reference & branch )
#define CSV_ACT_ORDER_TAG			5		// use for debug only (maybe put in memo field)
#define CSV_ACT_SIDE				6		// act side (B,S,X,Z,E,C,K,P,A) see act spec for mm entry
#define CSV_ACT_SYMBOL				7
#define CSV_ACT_SHARES				8
#define CSV_ACT_PRICE				9
#define CSV_ACT_MODIFIER			10
#define CSV_ACT_OVERRIDE			11
#define CSV_ACT_OEID				12
#define CSV_ACT_OEGU				13
#define CSV_ACT_OECLEAR				14
#define CSV_ACT_MMID				15
#define CSV_ACT_MMGU				16
#define CSV_ACT_MMCLEAR				17
#define CSV_ACT_CAPACITY			18
#define CSV_ACT_REPORT				19
#define CSV_ACT_CLEAR				20
#define CSV_ACT_EXEC_TIME			21
#define CSV_ACT_REFERENCE_NUM		22
#define CSV_ACT_BUY_REF_NUM			22 // use this instead of above define for TCLK
#define CSV_ACT_SELL_REF_NUM		23
#define CSV_ACT_REJECT_REASON		24
#define CSV_ACT_EXECUTION_TAG		25

