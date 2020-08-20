#if !defined(DEF_CMS)
#define DEF_CMS


typedef long TMsgSeqNum;

#define CMS_MSG_SEQ_NUM_MAX			99999
#define CMS_MSG_LINE_DELIMITOR		'\n'
#define CMS_MSG_FIELD_DELIMITOR		' '

enum ECMSMsgType
{
	//send to CMS
	CMS_Order						= '1',
	CMS_Cancel						= '2',
	CMS_Replace						= '3',

	//CMS back
	CMS_Status						= 'S',
	CMS_Admin						= 'A',
	CMS_Report						= 'R',

//connection related:
	CMS_Heartbeat					= 'B',
	CMS_ResendRequest				= 'C',
	CMS_SequenceReset				= 'D',
	CMS_Logout						= 'E',
	CMS_LogonReq					= 'F',
	CMS_LogonReply					= 'G',

	//admin messages
	CMS_Admin_CORRECTED_PRICE		= 'b',
	CMS_Admin_UR_OUT				= 'c',
	CMS_Admin_UR_STPD				= 'd',
	CMS_Admin_UR_GUAR				= 'e',
	CMS_Admin_TLTC					= 'f',
	CMS_Admin_ND,

	CMS_Status_Order_ACK			= 'm',
	CMS_Status_Admin_ACK			= 'n',
	CMS_Status_NumberGap			= 'o',
	CMS_Status_FreeFormat			= 'p',
//	CMS_Status_GoodNight,
//	CMS_Status_CrossingSession,
	CMS_Status_CMSRestart			= 'q',
//	CMS_Status_GoodMorning,
	CMS_Status_PossLoss				= 'r',
	CMS_Status_OrderRej				= 's',
	CMS_Status_Rej					= 't',
	CMS_Status_CancelRej			= 'v',
	CMS_Status_ReplaceRej			= 'u',

	CMS_Report_Partial				= 'P',
	CMS_Report_Fill					= 'Q',		

	CMS_Special						= 'X',
	CMS_UNKNOWN						= 'U'
};


enum ECMSOrdStatus
{
	CMS_New	= '0',
	CMS_PartiallyFilled	= '1',
	CMS_Filled = '2',
	CMS_DoneForDay = '3',	
	CMS_Canceled = '4',
	CMS_Replaced = '5',
	CMS_PendingCancel = '6',
	CMS_Stopped = '7',
	CMS_Rejected = '8',
	CMS_Suspended = '9',
	CMS_PendingNew = 'A',
	CMS_Calculated = 'B',
	CMS_Expired = 'C'
};

#endif
