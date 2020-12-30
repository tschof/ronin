/*******************************************************************************
 * Permission to use this/these file(s) is subject to the Terms of Use set
 * forth in the file Terms_of_Use.txt accompanying this file.
 *******************************************************************************
 *
 *  FILE NAME: ETILayoutsNS_Derivatives.h
 *
 *  INTERFACE VERSION:   8.0
 *
 *  SUBVERSION:          D0002
 *
 *  BUILD NUMBER:        80.250.0.ga-80003010-227
 *
 *  DESCRIPTION:
 *
 *    This header file documents the binary message format of ETI.
 *    - All integers are in little endian byte order.
 *    - Padding bytes in following structures (char PadX[...]) are not required to be initialized.
 *
 *    DISCLAIMER:
 *
 *      Supported on Linux/x64 platforms with GNU C/C++ version 4.1 and 4.4.
 *
 *      This header file is meant to be compatible (but not supported) with any C/C++
 *      compiler/architecture that defines C99 compliant integer types in stdint.h and
 *      corresponds with the following alignment and padding requirements:
 *
 *    Padding:
 *      The compiler does not add implicit padding bytes between any of the following
 *      structure members. All padding bytes required for the alignment rules below are
 *      already explicitly contained in the structures.
 *
 *    Alignment rules:
 *      1 byte alignment for  int8_t and  uint8_t types
 *      2 byte alignment for int16_t and uint16_t types
 *      4 byte alignment for int32_t and uint32_t types
 *      8 byte alignment for int64_t and uint64_t types
 *
 *******************************************************************************/

#ifndef __ETI_DERIVATIVES_LAYOUTS_WITH_NAMESPACE__
#define __ETI_DERIVATIVES_LAYOUTS_WITH_NAMESPACE__

#include <stdint.h>

#if defined(__cplusplus) || defined(c_plusplus)
namespace Derivatives
{
extern "C"
{
#endif

#define ETI_INTERFACE_VERSION "8.0"
#define ETI_BUILD_NUMBER      "80.250.0.ga-80003010-227"

/*
 * No Value defines
 */
#define NO_VALUE_SLONG                                   ((int64_t) 0x8000000000000000L)
#define NO_VALUE_ULONG                                   ((uint64_t) 0xffffffffffffffffUL)
#define NO_VALUE_SINT                                    ((int32_t) 0x80000000)
#define NO_VALUE_UINT                                    ((uint32_t) 0xffffffff)
#define NO_VALUE_SSHORT                                  ((int16_t) 0x8000)
#define NO_VALUE_USHORT                                  ((uint16_t) 0xffff)
#define NO_VALUE_SCHAR                                   ((int8_t) 0x80)
#define NO_VALUE_UCHAR                                   ((uint8_t) 0xff)
#define NO_VALUE_STR                                     0
#define NO_VALUE_DATA_16                                 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

/*
 * Template IDs defines
 */
#define	TID_ADD_COMPLEX_INSTRUMENT_REQUEST               10301		// < AddComplexInstrumentRequest (Create Strategy)
#define	TID_ADD_COMPLEX_INSTRUMENT_RESPONSE              10302		// < AddComplexInstrumentResponse (Create Strategy Response)
#define	TID_ADD_FLEXIBLE_INSTRUMENT_REQUEST              10309		// < AddFlexibleInstrumentRequest (Create Flexible Instrument Request)
#define	TID_ADD_FLEXIBLE_INSTRUMENT_RESPONSE             10310		// < AddFlexibleInstrumentResponse (Create Flexible Instrument Response)
#define	TID_AMEND_BASKET_TRADE_REQUEST                   10629		// < AmendBasketTradeRequest (Amend Basket Trade Request)
#define	TID_APPROVE_BASKET_TRADE_REQUEST                 10623		// < ApproveBasketTradeRequest (Approve Basket Trade Request)
#define	TID_APPROVE_TES_TRADE_REQUEST                    10603		// < ApproveTESTradeRequest (Approve TES Trade Request)
#define	TID_BASKET_APPROVE_BROADCAST                     10627		// < BasketApproveBroadcast (Approve Basket Broadcast)
#define	TID_BASKET_BROADCAST                             10625		// < BasketBroadcast (Basket Trading Broadcast)
#define	TID_BASKET_DELETE_BROADCAST                      10626		// < BasketDeleteBroadcast (Delete Basket Broadcast)
#define	TID_BASKET_EXECUTION_BROADCAST                   10628		// < BasketExecutionBroadcast (Basket Execution Broadcast)
#define	TID_BASKET_RESPONSE                              10624		// < BasketResponse (Basket Trading Response)
#define	TID_BROADCAST_ERROR_NOTIFICATION                 10032		// < BroadcastErrorNotification (Gap Fill)
#define	TID_CLIP_DELETION_NOTIFICATION                   10134		// < CLIPDeletionNotification (CLIP Deletion Notification)
#define	TID_CLIP_EXECUTION_NOTIFICATION                  10135		// < CLIPExecutionNotification (CLIP Execution Notification)
#define	TID_CLIP_RESPONSE                                10133		// < CLIPResponse (Response to a CLIP Request)
#define	TID_CROSS_REQUEST                                10118		// < CrossRequest (Cross Request)
#define	TID_CROSS_REQUEST_RESPONSE                       10119		// < CrossRequestResponse (Cross Request Response)
#define	TID_DELETE_ALL_ORDER_BROADCAST                   10122		// < DeleteAllOrderBroadcast (Order Mass Cancellation Notification)
#define	TID_DELETE_ALL_ORDER_NR_RESPONSE                 10124		// < DeleteAllOrderNRResponse (Order Mass Cancellation Response No Hits)
#define	TID_DELETE_ALL_ORDER_QUOTE_EVENT_BROADCAST       10308		// < DeleteAllOrderQuoteEventBroadcast (Mass Cancellation Event)
#define	TID_DELETE_ALL_ORDER_REQUEST                     10120		// < DeleteAllOrderRequest (Order Mass Cancellation Request)
#define	TID_DELETE_ALL_ORDER_RESPONSE                    10121		// < DeleteAllOrderResponse (Order Mass Cancellation Response)
#define	TID_DELETE_ALL_QUOTE_BROADCAST                   10410		// < DeleteAllQuoteBroadcast (Quote Mass Cancellation Notification)
#define	TID_DELETE_ALL_QUOTE_REQUEST                     10408		// < DeleteAllQuoteRequest (Quote Mass Cancellation Request)
#define	TID_DELETE_ALL_QUOTE_RESPONSE                    10409		// < DeleteAllQuoteResponse (Quote Mass Cancellation Response)
#define	TID_DELETE_BASKET_TRADE_REQUEST                  10622		// < DeleteBasketTradeRequest (Delete Basket Trade Request)
#define	TID_DELETE_CLIP_REQUEST                          10132		// < DeleteCLIPRequest (CLIP Delete Request)
#define	TID_DELETE_ORDER_BROADCAST                       10112		// < DeleteOrderBroadcast (Cancel Order Notification)
#define	TID_DELETE_ORDER_COMPLEX_REQUEST                 10123		// < DeleteOrderComplexRequest (Cancel Order Multi Leg)
#define	TID_DELETE_ORDER_NR_RESPONSE                     10111		// < DeleteOrderNRResponse (Cancel Order Response (Lean Order))
#define	TID_DELETE_ORDER_RESPONSE                        10110		// < DeleteOrderResponse (Cancel Order Response (Standard Order))
#define	TID_DELETE_ORDER_SINGLE_REQUEST                  10109		// < DeleteOrderSingleRequest (Cancel Order Single)
#define	TID_DELETE_TES_TRADE_REQUEST                     10602		// < DeleteTESTradeRequest (Delete TES Trade Request)
#define	TID_ENTER_BASKET_TRADE_REQUEST                   10620		// < EnterBasketTradeRequest (Enter Basket Trade Request)
#define	TID_ENTER_CLIP_REQUEST                           10131		// < EnterCLIPRequest (CLIP Enter Request)
#define	TID_ENTER_TES_TRADE_REQUEST                      10600		// < EnterTESTradeRequest (Enter TES Trade Request)
#define	TID_FORCED_LOGOUT_NOTIFICATION                   10012		// < ForcedLogoutNotification (Session Logout Notification)
#define	TID_FORCED_USER_LOGOUT_NOTIFICATION              10043		// < ForcedUserLogoutNotification (User Logout Notification)
#define	TID_HEARTBEAT                                    10011		// < Heartbeat (Heartbeat)
#define	TID_HEARTBEAT_NOTIFICATION                       10023		// < HeartbeatNotification (Heartbeat Notification)
#define	TID_INQUIRE_ENRICHMENT_RULE_ID_LIST_REQUEST      10040		// < InquireEnrichmentRuleIDListRequest (Trade Enrichment List Inquire)
#define	TID_INQUIRE_ENRICHMENT_RULE_ID_LIST_RESPONSE     10041		// < InquireEnrichmentRuleIDListResponse (Trade Enrichment List Inquire Response)
#define	TID_INQUIRE_MM_PARAMETER_REQUEST                 10305		// < InquireMMParameterRequest (Inquire Market Maker Parameters)
#define	TID_INQUIRE_MM_PARAMETER_RESPONSE                10306		// < InquireMMParameterResponse (Inquire Market Maker Parameters Response)
#define	TID_INQUIRE_PRE_TRADE_RISK_LIMITS_REQUEST        10311		// < InquirePreTradeRiskLimitsRequest (Inquire Pre-Trade Risk Limits Request)
#define	TID_INQUIRE_SESSION_LIST_REQUEST                 10035		// < InquireSessionListRequest (Session List Inquire)
#define	TID_INQUIRE_SESSION_LIST_RESPONSE                10036		// < InquireSessionListResponse (Session List Inquire Response)
#define	TID_INQUIRE_USER_REQUEST                         10038		// < InquireUserRequest (User List Inquire)
#define	TID_INQUIRE_USER_RESPONSE                        10039		// < InquireUserResponse (User List Inquire Response)
#define	TID_LEGAL_NOTIFICATION_BROADCAST                 10037		// < LegalNotificationBroadcast (Legal Notification)
#define	TID_LOGON_REQUEST                                10000		// < LogonRequest (Session Logon)
#define	TID_LOGON_RESPONSE                               10001		// < LogonResponse (Session Logon Response)
#define	TID_LOGOUT_REQUEST                               10002		// < LogoutRequest (Session Logout)
#define	TID_LOGOUT_RESPONSE                              10003		// < LogoutResponse (Session Logout Response)
#define	TID_MM_PARAMETER_DEFINITION_REQUEST              10303		// < MMParameterDefinitionRequest (Market Maker Parameter Definition)
#define	TID_MM_PARAMETER_DEFINITION_RESPONSE             10304		// < MMParameterDefinitionResponse (Market Maker Parameter Definition Response)
#define	TID_MASS_QUOTE_REQUEST                           10405		// < MassQuoteRequest (Mass Quote)
#define	TID_MASS_QUOTE_RESPONSE                          10406		// < MassQuoteResponse (Mass Quote Response)
#define	TID_MODIFY_BASKET_TRADE_REQUEST                  10621		// < ModifyBasketTradeRequest (Modify Basket Trade Request)
#define	TID_MODIFY_ORDER_COMPLEX_REQUEST                 10114		// < ModifyOrderComplexRequest (Replace Order Multi Leg)
#define	TID_MODIFY_ORDER_COMPLEX_SHORT_REQUEST           10130		// < ModifyOrderComplexShortRequest (Replace Order Multi Leg (short layout))
#define	TID_MODIFY_ORDER_NR_RESPONSE                     10108		// < ModifyOrderNRResponse (Replace Order Response (Lean Order))
#define	TID_MODIFY_ORDER_RESPONSE                        10107		// < ModifyOrderResponse (Replace Order Response (Standard Order))
#define	TID_MODIFY_ORDER_SINGLE_REQUEST                  10106		// < ModifyOrderSingleRequest (Replace Order Single)
#define	TID_MODIFY_ORDER_SINGLE_SHORT_REQUEST            10126		// < ModifyOrderSingleShortRequest (Replace Order Single (short layout))
#define	TID_MODIFY_TES_TRADE_REQUEST                     10601		// < ModifyTESTradeRequest (Modify TES Trade Request)
#define	TID_NEW_ORDER_COMPLEX_REQUEST                    10113		// < NewOrderComplexRequest (New Order Multi Leg)
#define	TID_NEW_ORDER_COMPLEX_SHORT_REQUEST              10129		// < NewOrderComplexShortRequest (New Order Multi Leg (short layout))
#define	TID_NEW_ORDER_NR_RESPONSE                        10102		// < NewOrderNRResponse (New Order Response (Lean Order))
#define	TID_NEW_ORDER_RESPONSE                           10101		// < NewOrderResponse (New Order Response (Standard Order))
#define	TID_NEW_ORDER_SINGLE_REQUEST                     10100		// < NewOrderSingleRequest (New Order Single)
#define	TID_NEW_ORDER_SINGLE_SHORT_REQUEST               10125		// < NewOrderSingleShortRequest (New Order Single (short layout))
#define	TID_NEWS_BROADCAST                               10031		// < NewsBroadcast (News)
#define	TID_ORDER_EXEC_NOTIFICATION                      10104		// < OrderExecNotification (Book Order Execution)
#define	TID_ORDER_EXEC_REPORT_BROADCAST                  10117		// < OrderExecReportBroadcast (Extended Order Information)
#define	TID_ORDER_EXEC_RESPONSE                          10103		// < OrderExecResponse (Immediate Execution Response)
#define	TID_PARTY_ACTION_REPORT                          10042		// < PartyActionReport (Party Action Report)
#define	TID_PARTY_ENTITLEMENTS_UPDATE_REPORT             10034		// < PartyEntitlementsUpdateReport (Entitlement Notification)
#define	TID_PRE_TRADE_RISK_LIMIT_RESPONSE                10313		// < PreTradeRiskLimitResponse (Pre-Trade Risk Limit Response)
#define	TID_PRE_TRADE_RISK_LIMITS_DEFINITION_REQUEST     10312		// < PreTradeRiskLimitsDefinitionRequest (Pre-Trade Risk Limits Definition Request)
#define	TID_QUOTE_ACTIVATION_NOTIFICATION                10411		// < QuoteActivationNotification (Quote Activation Notification)
#define	TID_QUOTE_ACTIVATION_REQUEST                     10403		// < QuoteActivationRequest (Quote Activation Request)
#define	TID_QUOTE_ACTIVATION_RESPONSE                    10404		// < QuoteActivationResponse (Quote Activation Response)
#define	TID_QUOTE_EXECUTION_REPORT                       10407		// < QuoteExecutionReport (Quote Execution Notification)
#define	TID_RFQ_REQUEST                                  10401		// < RFQRequest (Quote Request)
#define	TID_RFQ_RESPONSE                                 10402		// < RFQResponse (Quote Request Response)
#define	TID_REJECT                                       10010		// < Reject (Reject)
#define	TID_RETRANSMIT_ME_MESSAGE_REQUEST                10026		// < RetransmitMEMessageRequest (Retransmit (Order/Quote Event))
#define	TID_RETRANSMIT_ME_MESSAGE_RESPONSE               10027		// < RetransmitMEMessageResponse (Retransmit Response (Order/Quote Event))
#define	TID_RETRANSMIT_REQUEST                           10008		// < RetransmitRequest (Retransmit)
#define	TID_RETRANSMIT_RESPONSE                          10009		// < RetransmitResponse (Retransmit Response)
#define	TID_RISK_NOTIFICATION_BROADCAST                  10033		// < RiskNotificationBroadcast (Risk Notification)
#define	TID_SRQS_CREATE_DEAL_NOTIFICATION                10708		// < SRQSCreateDealNotification (SRQS Create Deal Notification)
#define	TID_SRQS_DEAL_NOTIFICATION                       10709		// < SRQSDealNotification (SRQS Deal Notification)
#define	TID_SRQS_DEAL_RESPONSE                           10705		// < SRQSDealResponse (SRQS Deal Response)
#define	TID_SRQS_ENTER_QUOTE_REQUEST                     10702		// < SRQSEnterQuoteRequest (SRQS Enter Quote Request)
#define	TID_SRQS_HIT_QUOTE_REQUEST                       10704		// < SRQSHitQuoteRequest (SRQS Hit Quote Request)
#define	TID_SRQS_NEGOTIATION_NOTIFICATION                10713		// < SRQSNegotiationNotification (SRQS Negotiation Notification for Respondent)
#define	TID_SRQS_NEGOTIATION_REQUESTER_NOTIFICATION      10712		// < SRQSNegotiationRequesterNotification (SRQS Negotiation Notification for Requester)
#define	TID_SRQS_NEGOTIATION_STATUS_NOTIFICATION         10715		// < SRQSNegotiationStatusNotification (SRQS Negotiation Status Notification)
#define	TID_SRQS_OPEN_NEGOTIATION_NOTIFICATION           10711		// < SRQSOpenNegotiationNotification (SRQS Open Negotiation Notification for Respondent)
#define	TID_SRQS_OPEN_NEGOTIATION_REQUEST                10700		// < SRQSOpenNegotiationRequest (SRQS Open Negotiation Request)
#define	TID_SRQS_OPEN_NEGOTIATION_REQUESTER_NOTIFICATION 10710		// < SRQSOpenNegotiationRequesterNotification (SRQS Open Negotiation Notification for Requester)
#define	TID_SRQS_QUOTE_NOTIFICATION                      10707		// < SRQSQuoteNotification (SRQS Quote Notification for Responder)
#define	TID_SRQS_QUOTE_RESPONSE                          10703		// < SRQSQuoteResponse (SRQS Quote Response)
#define	TID_SRQS_QUOTING_STATUS_REQUEST                  10717		// < SRQSQuotingStatusRequest (SRQS Enter Quoting Status Request)
#define	TID_SRQS_STATUS_BROADCAST                        10714		// < SRQSStatusBroadcast (SRQS Status Notification)
#define	TID_SRQS_UPDATE_DEAL_STATUS_REQUEST              10706		// < SRQSUpdateDealStatusRequest (SRQS Update Deal Request)
#define	TID_SRQS_UPDATE_NEGOTIATION_REQUEST              10701		// < SRQSUpdateNegotiationRequest (SRQS Update Negotiation Request)
#define	TID_SERVICE_AVAILABILITY_BROADCAST               10030		// < ServiceAvailabilityBroadcast (Service Availability)
#define	TID_SERVICE_AVAILABILITY_MARKET_BROADCAST        10044		// < ServiceAvailabilityMarketBroadcast (Service Availability Market)
#define	TID_SUBSCRIBE_REQUEST                            10025		// < SubscribeRequest (Subscribe)
#define	TID_SUBSCRIBE_RESPONSE                           10005		// < SubscribeResponse (Subscribe Response)
#define	TID_TES_APPROVE_BROADCAST                        10607		// < TESApproveBroadcast (Approve TES Trade Broadcast)
#define	TID_TES_BROADCAST                                10604		// < TESBroadcast (TES Broadcast)
#define	TID_TES_DELETE_BROADCAST                         10606		// < TESDeleteBroadcast (Delete TES Trade Broadcast)
#define	TID_TES_EXECUTION_BROADCAST                      10610		// < TESExecutionBroadcast (TES Execution Broadcast)
#define	TID_TES_RESPONSE                                 10611		// < TESResponse (TES Response)
#define	TID_TES_TRADE_BROADCAST                          10614		// < TESTradeBroadcast (TES Trade Broadcast)
#define	TID_TES_TRADING_SESSION_STATUS_BROADCAST         10615		// < TESTradingSessionStatusBroadcast (TES Status Broadcast )
#define	TID_TES_UPLOAD_BROADCAST                         10613		// < TESUploadBroadcast (TES Trade Upload Broadcast )
#define	TID_TM_TRADING_SESSION_STATUS_BROADCAST          10501		// < TMTradingSessionStatusBroadcast (Trade Notification Status)
#define	TID_THROTTLE_UPDATE_NOTIFICATION                 10028		// < ThrottleUpdateNotification (Throttle Update Notification)
#define	TID_TRADE_BROADCAST                              10500		// < TradeBroadcast (Trade Notification)
#define	TID_TRADING_SESSION_STATUS_BROADCAST             10307		// < TradingSessionStatusBroadcast (Trading Session Event)
#define	TID_UNSUBSCRIBE_REQUEST                          10006		// < UnsubscribeRequest (Unsubscribe)
#define	TID_UNSUBSCRIBE_RESPONSE                         10007		// < UnsubscribeResponse (Unsubscribe Response)
#define	TID_UPLOAD_TES_TRADE_REQUEST                     10612		// < UploadTESTradeRequest (Upload TES Trade Request)
#define	TID_USER_LOGIN_REQUEST                           10018		// < UserLoginRequest (User Logon)
#define	TID_USER_LOGIN_RESPONSE                          10019		// < UserLoginResponse (User Logon Response)
#define	TID_USER_LOGOUT_REQUEST                          10029		// < UserLogoutRequest (User Logout)
#define	TID_USER_LOGOUT_RESPONSE                         10024		// < UserLogoutResponse (User Logout Response)

const int ETI_DERIVATIVES_TID_MIN = 10000;  // lowest assigned template ID
const int ETI_DERIVATIVES_TID_MAX = 10717;  // highest assigned template ID

/*
 * Max defines for sequences defines
 */
#define MAX_ADD_COMPLEX_INSTRUMENT_REQUEST_INSTRMT_LEG_GRP	20
#define MAX_ADD_COMPLEX_INSTRUMENT_RESPONSE_INSTRMT_LEG_GRP	20
#define MAX_AMEND_BASKET_TRADE_REQUEST_BASKET_ROOT_PARTY_GRP	2
#define MAX_AMEND_BASKET_TRADE_REQUEST_INSTRMT_MATCH_SIDE_GRP	99
#define MAX_AMEND_BASKET_TRADE_REQUEST_BASKET_SIDE_ALLOC_GRP	198
#define MAX_APPROVE_BASKET_TRADE_REQUEST_BASKET_SIDE_ALLOC_EXT_GRP	198
#define MAX_BASKET_APPROVE_BROADCAST_BASKET_ROOT_PARTY_GRP	2
#define MAX_BASKET_APPROVE_BROADCAST_BASKET_SIDE_ALLOC_EXT_BC_GRP	198
#define MAX_BASKET_BROADCAST_BASKET_ROOT_PARTY_GRP      	3
#define MAX_BASKET_BROADCAST_INSTRMT_MATCH_SIDE_GRP     	99
#define MAX_BASKET_BROADCAST_BASKET_SIDE_ALLOC_GRP      	198
#define MAX_BASKET_EXECUTION_BROADCAST_BASKET_EXEC_GRP  	99
#define MAX_CLIP_EXECUTION_NOTIFICATION_FILLS_GRP       	100
#define MAX_CLIP_EXECUTION_NOTIFICATION_INSTRMNT_LEG_EXEC_GRP	600
#define MAX_CLIP_RESPONSE_CROSS_REQUEST_ACK_SIDE_GRP    	2
#define MAX_DELETE_ALL_ORDER_BROADCAST_NOT_AFFECTED_ORDERS_GRP	500
#define MAX_DELETE_ALL_ORDER_BROADCAST_AFFECTED_ORD_GRP 	500
#define MAX_DELETE_ALL_ORDER_BROADCAST_AFFECTED_ORDER_REQUESTS_GRP	500
#define MAX_DELETE_ALL_ORDER_RESPONSE_NOT_AFFECTED_ORDERS_GRP	500
#define MAX_DELETE_ALL_ORDER_RESPONSE_AFFECTED_ORD_GRP  	500
#define MAX_DELETE_ALL_ORDER_RESPONSE_AFFECTED_ORDER_REQUESTS_GRP	500
#define MAX_DELETE_ALL_QUOTE_BROADCAST_NOT_AFFECTED_SECURITIES_GRP	500
#define MAX_DELETE_ALL_QUOTE_RESPONSE_NOT_AFFECTED_SECURITIES_GRP	500
#define MAX_ENTER_BASKET_TRADE_REQUEST_BASKET_ROOT_PARTY_GRP	2
#define MAX_ENTER_BASKET_TRADE_REQUEST_INSTRMT_MATCH_SIDE_GRP	99
#define MAX_ENTER_BASKET_TRADE_REQUEST_BASKET_SIDE_ALLOC_GRP	198
#define MAX_ENTER_CLIP_REQUEST_CROSS_REQUEST_SIDE_GRP   	2
#define MAX_ENTER_CLIP_REQUEST_SIDE_CROSS_LEG_GRP       	40
#define MAX_ENTER_TES_TRADE_REQUEST_SIDE_ALLOC_GRP      	30
#define MAX_ENTER_TES_TRADE_REQUEST_TRD_INSTRMNT_LEG_GRP	20
#define MAX_ENTER_TES_TRADE_REQUEST_INSTRUMENT_EVENT_GRP	2
#define MAX_ENTER_TES_TRADE_REQUEST_INSTRUMENT_ATTRIBUTE_GRP	6
#define MAX_ENTER_TES_TRADE_REQUEST_UNDERLYING_STIP_GRP 	1
#define MAX_ENTER_TES_TRADE_REQUEST_SRQS_RELATED_TRADE_ID_GRP	12
#define MAX_INQUIRE_ENRICHMENT_RULE_ID_LIST_RESPONSE_ENRICHMENT_RULES_GRP	400
#define MAX_INQUIRE_MM_PARAMETER_RESPONSE_MM_PARAMETER_GRP	9
#define MAX_INQUIRE_SESSION_LIST_RESPONSE_SESSIONS_GRP  	1000
#define MAX_INQUIRE_USER_RESPONSE_PARTY_DETAILS_GRP     	1000
#define MAX_MASS_QUOTE_REQUEST_QUOTE_ENTRY_GRP          	100
#define MAX_MASS_QUOTE_RESPONSE_QUOTE_ENTRY_ACK_GRP     	200
#define MAX_MODIFY_BASKET_TRADE_REQUEST_BASKET_ROOT_PARTY_GRP	2
#define MAX_MODIFY_BASKET_TRADE_REQUEST_INSTRMT_MATCH_SIDE_GRP	99
#define MAX_MODIFY_BASKET_TRADE_REQUEST_BASKET_SIDE_ALLOC_GRP	198
#define MAX_MODIFY_ORDER_COMPLEX_REQUEST_LEG_ORD_GRP    	20
#define MAX_MODIFY_TES_TRADE_REQUEST_SIDE_ALLOC_GRP     	30
#define MAX_MODIFY_TES_TRADE_REQUEST_TRD_INSTRMNT_LEG_GRP	20
#define MAX_MODIFY_TES_TRADE_REQUEST_SRQS_RELATED_TRADE_ID_GRP	12
#define MAX_NEW_ORDER_COMPLEX_REQUEST_LEG_ORD_GRP       	20
#define MAX_ORDER_EXEC_NOTIFICATION_FILLS_GRP           	100
#define MAX_ORDER_EXEC_NOTIFICATION_INSTRMNT_LEG_EXEC_GRP	600
#define MAX_ORDER_EXEC_REPORT_BROADCAST_LEG_ORD_GRP     	20
#define MAX_ORDER_EXEC_REPORT_BROADCAST_FILLS_GRP       	100
#define MAX_ORDER_EXEC_REPORT_BROADCAST_INSTRMNT_LEG_EXEC_GRP	600
#define MAX_ORDER_EXEC_RESPONSE_FILLS_GRP               	100
#define MAX_ORDER_EXEC_RESPONSE_INSTRMNT_LEG_EXEC_GRP   	600
#define MAX_PRE_TRADE_RISK_LIMIT_RESPONSE_RISK_LIMITS_RPT_GRP	64
#define MAX_PRE_TRADE_RISK_LIMITS_DEFINITION_REQUEST_RISK_LIMIT_QTY_GRP	2
#define MAX_QUOTE_ACTIVATION_NOTIFICATION_NOT_AFFECTED_SECURITIES_GRP	500
#define MAX_QUOTE_ACTIVATION_RESPONSE_NOT_AFFECTED_SECURITIES_GRP	500
#define MAX_QUOTE_EXECUTION_REPORT_QUOTE_EVENT_GRP      	100
#define MAX_QUOTE_EXECUTION_REPORT_QUOTE_LEG_EXEC_GRP   	600
#define MAX_SRQS_CREATE_DEAL_NOTIFICATION_ORDER_BOOK_ITEM_GRP	26
#define MAX_SRQS_NEGOTIATION_REQUESTER_NOTIFICATION_TARGET_PARTIES	50
#define MAX_SRQS_OPEN_NEGOTIATION_NOTIFICATION_QUOT_REQ_LEGS_GRP	20
#define MAX_SRQS_OPEN_NEGOTIATION_REQUEST_QUOT_REQ_LEGS_GRP	20
#define MAX_SRQS_OPEN_NEGOTIATION_REQUEST_TARGET_PARTIES	50
#define MAX_SRQS_OPEN_NEGOTIATION_REQUESTER_NOTIFICATION_QUOT_REQ_LEGS_GRP	20
#define MAX_SRQS_OPEN_NEGOTIATION_REQUESTER_NOTIFICATION_TARGET_PARTIES	50
#define MAX_SRQS_UPDATE_NEGOTIATION_REQUEST_TARGET_PARTIES	50
#define MAX_TES_APPROVE_BROADCAST_TRD_INSTRMNT_LEG_GRP  	20
#define MAX_TES_APPROVE_BROADCAST_INSTRUMENT_EVENT_GRP  	2
#define MAX_TES_APPROVE_BROADCAST_INSTRUMENT_ATTRIBUTE_GRP	6
#define MAX_TES_APPROVE_BROADCAST_UNDERLYING_STIP_GRP   	1
#define MAX_TES_APPROVE_BROADCAST_SRQS_RELATED_TRADE_ID_GRP	12
#define MAX_TES_BROADCAST_SIDE_ALLOC_GRPBC              	30
#define MAX_TES_BROADCAST_TRD_INSTRMNT_LEG_GRP          	20
#define MAX_TES_BROADCAST_INSTRUMENT_EVENT_GRP          	2
#define MAX_TES_BROADCAST_INSTRUMENT_ATTRIBUTE_GRP      	6
#define MAX_TES_BROADCAST_UNDERLYING_STIP_GRP           	1
#define MAX_TES_BROADCAST_SRQS_RELATED_TRADE_ID_GRP     	12
#define MAX_TES_TRADE_BROADCAST_SRQS_RELATED_TRADE_ID_GRP	12
#define MAX_TES_UPLOAD_BROADCAST_SIDE_ALLOC_EXT_GRP     	30
#define MAX_TES_UPLOAD_BROADCAST_TRD_INSTRMNT_LEG_GRP   	20
#define MAX_TES_UPLOAD_BROADCAST_INSTRUMENT_EVENT_GRP   	2
#define MAX_TES_UPLOAD_BROADCAST_INSTRUMENT_ATTRIBUTE_GRP	6
#define MAX_TES_UPLOAD_BROADCAST_UNDERLYING_STIP_GRP    	1
#define MAX_TES_UPLOAD_BROADCAST_SRQS_RELATED_TRADE_ID_GRP	12
#define MAX_UPLOAD_TES_TRADE_REQUEST_SIDE_ALLOC_EXT_GRP 	30
#define MAX_UPLOAD_TES_TRADE_REQUEST_TRD_INSTRMNT_LEG_GRP	20
#define MAX_UPLOAD_TES_TRADE_REQUEST_INSTRUMENT_EVENT_GRP	2
#define MAX_UPLOAD_TES_TRADE_REQUEST_INSTRUMENT_ATTRIBUTE_GRP	6
#define MAX_UPLOAD_TES_TRADE_REQUEST_UNDERLYING_STIP_GRP	1
#define MAX_UPLOAD_TES_TRADE_REQUEST_SRQS_RELATED_TRADE_ID_GRP	12

/*
 * Data Type defines
 */

// DataType Account
#define LEN_ACCOUNT                                      2

// DataType ApplBegMsgID
#define LEN_APPL_BEG_MSGID                               16

// DataType ApplEndMsgID
#define LEN_APPL_END_MSGID                               16

// DataType ApplID
#define ENUM_APPLID_TRADE                                1
#define ENUM_APPLID_NEWS                                 2
#define ENUM_APPLID_SERVICE_AVAILABILITY                 3
#define ENUM_APPLID_SESSION_DATA                         4
#define ENUM_APPLID_LISTENER_DATA                        5
#define ENUM_APPLID_RISK_CONTROL                         6
#define ENUM_APPLID_TES_MAINTENANCE                      7
#define ENUM_APPLID_TES_TRADE                            8
#define ENUM_APPLID_SRQS_MAINTENANCE                     9
#define ENUM_APPLID_SERVICE_AVAILABILITY_MARKET          10

// DataType ApplIDStatus
#define ENUM_APPL_ID_STATUS_OUTBOUND_CONVERSION_ERROR    105

// DataType ApplMsgID
#define LEN_APPL_MSGID                                   16

// DataType ApplResendFlag
#define ENUM_APPL_RESEND_FLAG_FALSE                      0
#define ENUM_APPL_RESEND_FLAG_TRUE                       1

// DataType ApplSeqIndicator
#define ENUM_APPL_SEQ_INDICATOR_NO_RECOVERY_REQUIRED     0
#define ENUM_APPL_SEQ_INDICATOR_RECOVERY_REQUIRED        1

// DataType ApplSeqStatus
#define ENUM_APPL_SEQ_STATUS_UNAVAILABLE                 0
#define ENUM_APPL_SEQ_STATUS_AVAILABLE                   1

// DataType ApplUsageOrders
#define LEN_APPL_USAGE_ORDERS                            1
#define ENUM_APPL_USAGE_ORDERS_AUTOMATED                 "A"
#define ENUM_APPL_USAGE_ORDERS_AUTOMATED_CHAR            'A'
#define ENUM_APPL_USAGE_ORDERS_MANUAL                    "M"
#define ENUM_APPL_USAGE_ORDERS_MANUAL_CHAR               'M'
#define ENUM_APPL_USAGE_ORDERS_AUTO_SELECT               "B"
#define ENUM_APPL_USAGE_ORDERS_AUTO_SELECT_CHAR          'B'
#define ENUM_APPL_USAGE_ORDERS_NONE                      "N"
#define ENUM_APPL_USAGE_ORDERS_NONE_CHAR                 'N'

// DataType ApplUsageQuotes
#define LEN_APPL_USAGE_QUOTES                            1
#define ENUM_APPL_USAGE_QUOTES_AUTOMATED                 "A"
#define ENUM_APPL_USAGE_QUOTES_AUTOMATED_CHAR            'A'
#define ENUM_APPL_USAGE_QUOTES_MANUAL                    "M"
#define ENUM_APPL_USAGE_QUOTES_MANUAL_CHAR               'M'
#define ENUM_APPL_USAGE_QUOTES_AUTO_SELECT               "B"
#define ENUM_APPL_USAGE_QUOTES_AUTO_SELECT_CHAR          'B'
#define ENUM_APPL_USAGE_QUOTES_NONE                      "N"
#define ENUM_APPL_USAGE_QUOTES_NONE_CHAR                 'N'

// DataType ApplicationSystemName
#define LEN_APPLICATION_SYSTEM_NAME                      30

// DataType ApplicationSystemVendor
#define LEN_APPLICATION_SYSTEM_VENDOR                    30

// DataType ApplicationSystemVersion
#define LEN_APPLICATION_SYSTEM_VERSION                   30

// DataType BasketSideTradeReportID
#define LEN_BASKET_SIDE_TRADE_REPORTID                   20

// DataType BasketTradeReportText
#define LEN_BASKET_TRADE_REPORT_TEXT                     20

// DataType BasketTradeReportType
#define ENUM_BASKET_TRADE_REPORT_TYPE_SUBMIT             0
#define ENUM_BASKET_TRADE_REPORT_TYPE_ADDENDUM           4
#define ENUM_BASKET_TRADE_REPORT_TYPE_NO_WAS_SUBSTITUE   5

// DataType ComplianceText
#define LEN_COMPLIANCE_TEXT                              20

// DataType CoreSrvc
#define LEN_CORE_SRVC                                    100
#define ENUM_CORE_SRVC_TRADING                           "Order and Quote Management                                                                          "
#define ENUM_CORE_SRVC_ORDER_QUOTE_RETRANSMISSION        "Retransmission of Order and Quote Events                                                            "
#define ENUM_CORE_SRVC_TRADE_RETRANSMISSION              "Trades                                                                                              "
#define ENUM_CORE_SRVC_TES                               "T7 Entry                                                                                            "
#define ENUM_CORE_SRVC_SRQS                              "Selective Request for Quote Service                                                                 "
#define ENUM_CORE_SRVC_NONE                              "None                                                                                                "

// DataType CrossedIndicator
#define ENUM_CROSSED_INDICATOR_NO_CROSSING               0
#define ENUM_CROSSED_INDICATOR_CROSS_REJECTED            1

// DataType CustOrderHandlingInst
#define LEN_CUST_ORDER_HANDLING_INST                     1

// DataType DefaultCstmApplVerID
#define LEN_DEFAULT_CSTM_APPL_VERID                      30

// DataType DefaultCstmApplVerSubID
#define LEN_DEFAULT_CSTM_APPL_VER_SUBID                  5
#define ENUM_DEFAULT_CSTM_APPL_VER_SUBID_DERIVATIVES     "D0002"

// DataType DeleteReason
#define ENUM_DELETE_REASON_NO_SPECIAL_REASON             100
#define ENUM_DELETE_REASON_TAS_CHANGE                    101
#define ENUM_DELETE_REASON_INTRADAY_EXPIRATION           102
#define ENUM_DELETE_REASON_RISK_EVENT                    103
#define ENUM_DELETE_REASON_STOP_TRADING                  104
#define ENUM_DELETE_REASON_INSTRUMENT_DELETION           105
#define ENUM_DELETE_REASON_INSTRUMENT_SUSPENSION         106
#define ENUM_DELETE_REASON_PRE_TRADE_RISK_EVENT          107

// DataType EffectOnBasket
#define ENUM_EFFECT_ON_BASKET_ADD_VOLUME                 1
#define ENUM_EFFECT_ON_BASKET_REMOVE_VOLUME              2

// DataType EventType
#define ENUM_EVENT_TYPE_SWAP_START_DATE                  8
#define ENUM_EVENT_TYPE_SWAP_END_DATE                    9

// DataType ExecInst
#define ENUM_EXEC_INST_H                                 1
#define ENUM_EXEC_INST_Q                                 2
#define ENUM_EXEC_INST_H_Q                               3
#define ENUM_EXEC_INST_H_6                               5
#define ENUM_EXEC_INST_Q_6                               6

// DataType ExecRestatementReason
#define ENUM_EXEC_RESTATEMENT_REASON_ORDER_BOOK_RESTATEMENT 1
#define ENUM_EXEC_RESTATEMENT_REASON_ORDER_ADDED         101
#define ENUM_EXEC_RESTATEMENT_REASON_ORDER_MODIFIED      102
#define ENUM_EXEC_RESTATEMENT_REASON_ORDER_CANCELLED     103
#define ENUM_EXEC_RESTATEMENT_REASON_IOC_ORDER_CANCELLED 105
#define ENUM_EXEC_RESTATEMENT_REASON_BOOK_ORDER_EXECUTED 108
#define ENUM_EXEC_RESTATEMENT_REASON_CHANGED_TO_IOC      114
#define ENUM_EXEC_RESTATEMENT_REASON_INSTRUMENT_STATE_CHANGE 122
#define ENUM_EXEC_RESTATEMENT_REASON_MARKET_ORDER_TRIGGERED 135
#define ENUM_EXEC_RESTATEMENT_REASON_CAO_ORDER_ACTIVATED 149
#define ENUM_EXEC_RESTATEMENT_REASON_CAO_ORDER_INACTIVATED 150
#define ENUM_EXEC_RESTATEMENT_REASON_OCO_ORDER_TRIGGERED 164
#define ENUM_EXEC_RESTATEMENT_REASON_STOP_ORDER_TRIGGERED 172
#define ENUM_EXEC_RESTATEMENT_REASON_OWNERSHIP_CHANGED   181
#define ENUM_EXEC_RESTATEMENT_REASON_ORDER_CANCELLATION_PENDING 197
#define ENUM_EXEC_RESTATEMENT_REASON_PENDING_CANCELLATION_EXECUTED 199
#define ENUM_EXEC_RESTATEMENT_REASON_BOC_ORDER_CANCELLED 212
#define ENUM_EXEC_RESTATEMENT_REASON_PANIC_CANCEL        261
#define ENUM_EXEC_RESTATEMENT_REASON_CLIP_EXECUTION      340
#define ENUM_EXEC_RESTATEMENT_REASON_CLIP_ARRANGEMENT_TIME_OUT 343
#define ENUM_EXEC_RESTATEMENT_REASON_CLIP_ARRANGEMENT_VALIDATION 344

// DataType ExecType
#define LEN_EXEC_TYPE                                    1
#define ENUM_EXEC_TYPE_NEW                               "0"
#define ENUM_EXEC_TYPE_NEW_CHAR                          '0'
#define ENUM_EXEC_TYPE_CANCELED                          "4"
#define ENUM_EXEC_TYPE_CANCELED_CHAR                     '4'
#define ENUM_EXEC_TYPE_REPLACED                          "5"
#define ENUM_EXEC_TYPE_REPLACED_CHAR                     '5'
#define ENUM_EXEC_TYPE_PENDING_CANCEL_E                  "6"
#define ENUM_EXEC_TYPE_PENDING_CANCEL_E_CHAR             '6'
#define ENUM_EXEC_TYPE_SUSPENDED                         "9"
#define ENUM_EXEC_TYPE_SUSPENDED_CHAR                    '9'
#define ENUM_EXEC_TYPE_RESTATED                          "D"
#define ENUM_EXEC_TYPE_RESTATED_CHAR                     'D'
#define ENUM_EXEC_TYPE_TRIGGERED                         "L"
#define ENUM_EXEC_TYPE_TRIGGERED_CHAR                    'L'
#define ENUM_EXEC_TYPE_TRADE                             "F"
#define ENUM_EXEC_TYPE_TRADE_CHAR                        'F'

// DataType ExecutingTraderQualifier
#define ENUM_EXECUTING_TRADER_QUALIFIER_ALGO             22
#define ENUM_EXECUTING_TRADER_QUALIFIER_HUMAN            24

// DataType ExerciseStyle
#define ENUM_EXERCISE_STYLE_EUROPEAN                     0
#define ENUM_EXERCISE_STYLE_AMERICAN                     1

// DataType FIXClOrdID
#define LEN_FIX_CL_ORDID                                 20

// DataType FIXEngineName
#define LEN_FIX_ENGINE_NAME                              30

// DataType FIXEngineVendor
#define LEN_FIX_ENGINE_VENDOR                            30

// DataType FIXEngineVersion
#define LEN_FIX_ENGINE_VERSION                           30

// DataType FillLiquidityInd
#define ENUM_FILL_LIQUIDITY_IND_ADDED_LIQUIDITY          1
#define ENUM_FILL_LIQUIDITY_IND_REMOVED_LIQUIDITY        2
#define ENUM_FILL_LIQUIDITY_IND_AUCTION                  4
#define ENUM_FILL_LIQUIDITY_IND_TRIGGERED_STOP_ORDER     5
#define ENUM_FILL_LIQUIDITY_IND_TRIGGERED_OCO_ORDER      6
#define ENUM_FILL_LIQUIDITY_IND_TRIGGERED_MARKET_ORDER   7

// DataType FirmNegotiationID
#define LEN_FIRM_NEGOTIATIONID                           20

// DataType FirmTradeID
#define LEN_FIRM_TRADEID                                 20

// DataType FreeText1
#define LEN_FREE_TEXT1                                   12

// DataType FreeText2
#define LEN_FREE_TEXT2                                   12

// DataType FreeText3
#define LEN_FREE_TEXT3                                   12

// DataType FreeText5
#define LEN_FREE_TEXT5                                   132

// DataType FreeText5DisclosureInstruction
#define ENUM_FREE_TEXT5_DISCLOSURE_INSTRUCTION_NO        0
#define ENUM_FREE_TEXT5_DISCLOSURE_INSTRUCTION_YES       1

// DataType FuncCategory
#define LEN_FUNC_CATEGORY                                100
#define ENUM_FUNC_CATEGORY_ORDER_HANDLING                "Order Handling                                                                                      "
#define ENUM_FUNC_CATEGORY_SESSION_LAYER                 "Session Layer                                                                                       "
#define ENUM_FUNC_CATEGORY_QUOTE_HANDLING                "Quote Handling                                                                                      "
#define ENUM_FUNC_CATEGORY_BEST_QUOTE_HANDLING           "Best Quote Handling                                                                                 "
#define ENUM_FUNC_CATEGORY_QUOTE_AND_CROSS_REQUEST       "Quote and Cross Request                                                                             "
#define ENUM_FUNC_CATEGORY_STRATEGY_CREATION             "Strategy Creation                                                                                   "
#define ENUM_FUNC_CATEGORY_FLEXIBLE_INSTRUMENT_CREATION  "Flexible Instrument Creation                                                                        "
#define ENUM_FUNC_CATEGORY_TES_TRADING                   "TES Trading                                                                                         "
#define ENUM_FUNC_CATEGORY_SRQS                          "Selective Request for Quote Service                                                                 "
#define ENUM_FUNC_CATEGORY_OTHER                         "Other                                                                                               "
#define ENUM_FUNC_CATEGORY_BROADCAST                     "Broadcast                                                                                           "
#define ENUM_FUNC_CATEGORY_CLIP_TRADING                  "CLIP Trading                                                                                        "
#define ENUM_FUNC_CATEGORY_ISSUER                        "Issuer Model                                                                                        "
#define ENUM_FUNC_CATEGORY_BASKET_TRADING                "Basket Trading                                                                                      "
#define ENUM_FUNC_CATEGORY_SPECIALIST                    "Specialist Model                                                                                    "

// DataType GatewayStatus
#define ENUM_GATEWAY_STATUS_STANDBY                      0
#define ENUM_GATEWAY_STATUS_ACTIVE                       1

// DataType Headline
#define LEN_HEADLINE                                     256

// DataType HedgeType
#define ENUM_HEDGE_TYPE_DURATION_HEDGE                   0
#define ENUM_HEDGE_TYPE_NOMINAL_HEDGE                    1
#define ENUM_HEDGE_TYPE_PRICE_FACTOR_HEDGE               2

// DataType ImpliedMarketIndicator
#define ENUM_IMPLIED_MARKET_INDICATOR_NOT_IMPLIED        0
#define ENUM_IMPLIED_MARKET_INDICATOR_IMPLIED_IN_OUT     3

// DataType InputSource
#define ENUM_INPUT_SOURCE_CLIENT_BROKER                  1
#define ENUM_INPUT_SOURCE_PROPRIETARY_BROKER             2

// DataType InstrAttribType
#define ENUM_INSTR_ATTRIB_TYPE_VARIABLE_RATE             5
#define ENUM_INSTR_ATTRIB_TYPE_COUPON_RATE               100
#define ENUM_INSTR_ATTRIB_TYPE_OFFSET_TO_THE_VARIABLE_COUPON_RATE 101
#define ENUM_INSTR_ATTRIB_TYPE_SWAP_CUSTOMER_1           102
#define ENUM_INSTR_ATTRIB_TYPE_SWAP_CUSTOMER_2           103
#define ENUM_INSTR_ATTRIB_TYPE_CASH_BASKET_REFERENCE     104

// DataType InstrAttribValue
#define LEN_INSTR_ATTRIB_VALUE                           32

// DataType LastEntityProcessed
#define LEN_LAST_ENTITY_PROCESSED                        16

// DataType LastFragment
#define ENUM_LAST_FRAGMENT_NOT_LAST_MESSAGE              0
#define ENUM_LAST_FRAGMENT_LAST_MESSAGE                  1

// DataType LastMkt
#define ENUM_LAST_MKT_XEUR                               1
#define ENUM_LAST_MKT_XEEE                               2
#define ENUM_LAST_MKT_NODX                               12

// DataType LastPxDisclosureInstruction
#define ENUM_LAST_PX_DISCLOSURE_INSTRUCTION_NO           0
#define ENUM_LAST_PX_DISCLOSURE_INSTRUCTION_YES          1

// DataType LastQtyDisclosureInstruction
#define ENUM_LAST_QTY_DISCLOSURE_INSTRUCTION_NO          0
#define ENUM_LAST_QTY_DISCLOSURE_INSTRUCTION_YES         1

// DataType LeavesQtyDisclosureInstruction
#define ENUM_LEAVES_QTY_DISCLOSURE_INSTRUCTION_NO        0
#define ENUM_LEAVES_QTY_DISCLOSURE_INSTRUCTION_YES       1

// DataType LegAccount
#define LEN_LEG_ACCOUNT                                  2

// DataType LegInputSource
#define ENUM_LEG_INPUT_SOURCE_CLIENT_BROKER              1
#define ENUM_LEG_INPUT_SOURCE_PROPRIETARY_BROKER         2

// DataType LegPositionEffect
#define LEN_LEG_POSITION_EFFECT                          1
#define ENUM_LEG_POSITION_EFFECT_CLOSE                   "C"
#define ENUM_LEG_POSITION_EFFECT_CLOSE_CHAR              'C'
#define ENUM_LEG_POSITION_EFFECT_OPEN                    "O"
#define ENUM_LEG_POSITION_EFFECT_OPEN_CHAR               'O'

// DataType LegSecurityType
#define ENUM_LEG_SECURITY_TYPE_MULTILEG_INSTRUMENT       1
#define ENUM_LEG_SECURITY_TYPE_UNDERLYING_LEG            2

// DataType LegSide
#define ENUM_LEG_SIDE_BUY                                1
#define ENUM_LEG_SIDE_SELL                               2

// DataType ListUpdateAction
#define LEN_LIST_UPDATE_ACTION                           1
#define ENUM_LIST_UPDATE_ACTION_ADD                      "A"
#define ENUM_LIST_UPDATE_ACTION_ADD_CHAR                 'A'
#define ENUM_LIST_UPDATE_ACTION_DELETE                   "D"
#define ENUM_LIST_UPDATE_ACTION_DELETE_CHAR              'D'

// DataType MDBookType
#define ENUM_MD_BOOK_TYPE_TOP_OF_BOOK                    1
#define ENUM_MD_BOOK_TYPE_PRICE_DEPTH                    2

// DataType MDSubBookType
#define ENUM_MD_SUB_BOOK_TYPE_IMPLIED                    1
#define ENUM_MD_SUB_BOOK_TYPE_VOLUME_WEIGHTED_AVERAGE    2

// DataType MarketID
#define ENUM_MARKETID_XEUR                               1
#define ENUM_MARKETID_XEEE                               2
#define ENUM_MARKETID_NODX                               12

// DataType MassActionReason
#define ENUM_MASS_ACTION_REASON_NO_SPECIAL_REASON        0
#define ENUM_MASS_ACTION_REASON_STOP_TRADING             1
#define ENUM_MASS_ACTION_REASON_EMERGENCY                2
#define ENUM_MASS_ACTION_REASON_MARKET_MAKER_PROTECTION  3
#define ENUM_MASS_ACTION_REASON_SESSION_LOSS             6
#define ENUM_MASS_ACTION_REASON_DUPLICATE_SESSION_LOGIN  7
#define ENUM_MASS_ACTION_REASON_CLEARING_RISK_CONTROL    8
#define ENUM_MASS_ACTION_REASON_INTERNAL_CONNECTION_LOSS 100
#define ENUM_MASS_ACTION_REASON_PRODUCT_STATE_HALT       105
#define ENUM_MASS_ACTION_REASON_PRODUCT_STATE_HOLIDAY    106
#define ENUM_MASS_ACTION_REASON_INSTRUMENT_SUSPENDED     107
#define ENUM_MASS_ACTION_REASON_COMPLEX_INSTRUMENT_DELETION 109
#define ENUM_MASS_ACTION_REASON_VOLATILITY_INTERRUPTION  110
#define ENUM_MASS_ACTION_REASON_PRODUCT_TEMPORARILY_NOT_TRADEABLE 111
#define ENUM_MASS_ACTION_REASON_MEMBER_DISABLE           117

// DataType MassActionSubType
#define ENUM_MASS_ACTION_SUB_TYPE_BOOK_OR_CANCEL         1

// DataType MassActionType
#define ENUM_MASS_ACTION_TYPE_SUSPEND_QUOTES             1
#define ENUM_MASS_ACTION_TYPE_RELEASE_QUOTES             2

// DataType MatchSubType
#define ENUM_MATCH_SUB_TYPE_OPENING_AUCTION              1
#define ENUM_MATCH_SUB_TYPE_CLOSING_AUCTION              2
#define ENUM_MATCH_SUB_TYPE_INTRADAY_AUCTION             3
#define ENUM_MATCH_SUB_TYPE_CIRCUIT_BREAKER_AUCTION      4
#define ENUM_MATCH_SUB_TYPE_OUTSIDE_BBO                  6

// DataType MatchType
#define ENUM_MATCH_TYPE_CONFIRMED_TRADE_REPORT           3
#define ENUM_MATCH_TYPE_AUTO_MATCH_INCOMING              4
#define ENUM_MATCH_TYPE_CROSS_AUCTION                    5
#define ENUM_MATCH_TYPE_CALL_AUCTION                     7
#define ENUM_MATCH_TYPE_AUTO_MATCH_RESTING               11
#define ENUM_MATCH_TYPE_LIQUIDITY_IMPROVEMENT_CROSS      13

// DataType MatchingEngineStatus
#define ENUM_MATCHING_ENGINE_STATUS_UNAVAILABLE          0
#define ENUM_MATCHING_ENGINE_STATUS_AVAILABLE            1

// DataType MessageEventSource
#define LEN_MESSAGE_EVENT_SOURCE                         1
#define ENUM_MESSAGE_EVENT_SOURCE_BROADCAST_TO_INITIATOR "I"
#define ENUM_MESSAGE_EVENT_SOURCE_BROADCAST_TO_INITIATOR_CHAR 'I'
#define ENUM_MESSAGE_EVENT_SOURCE_BROADCAST_TO_APPROVER  "A"
#define ENUM_MESSAGE_EVENT_SOURCE_BROADCAST_TO_APPROVER_CHAR 'A'
#define ENUM_MESSAGE_EVENT_SOURCE_BROADCAST_TO_REQUESTER "R"
#define ENUM_MESSAGE_EVENT_SOURCE_BROADCAST_TO_REQUESTER_CHAR 'R'
#define ENUM_MESSAGE_EVENT_SOURCE_BROADCAST_TO_QUOTE_SUBMITTER "Q"
#define ENUM_MESSAGE_EVENT_SOURCE_BROADCAST_TO_QUOTE_SUBMITTER_CHAR 'Q'

// DataType MsgType
#define LEN_MSG_TYPE                                     3
#define ENUM_MSG_TYPE_HEARTBEAT                          "0  "
#define ENUM_MSG_TYPE_REJECT                             "3  "
#define ENUM_MSG_TYPE_LOGOUT                             "5  "
#define ENUM_MSG_TYPE_EXECUTION_REPORT                   "8  "
#define ENUM_MSG_TYPE_LOGON                              "A  "
#define ENUM_MSG_TYPE_NEW_ORDER_MULTILEG                 "AB "
#define ENUM_MSG_TYPE_MULTILEG_ORDER_CANCEL_REPLACE      "AC "
#define ENUM_MSG_TYPE_TRADE_CAPTURE_REPORT               "AE "
#define ENUM_MSG_TYPE_TRADE_CAPTURE_REPORT_ACK           "AR "
#define ENUM_MSG_TYPE_QUOTE_REQUEST_REJECT               "AG "
#define ENUM_MSG_TYPE_TRADE_MATCH_REPORT                 "DC "
#define ENUM_MSG_TYPE_TRADE_MATCH_REPORT_ACK             "DD "
#define ENUM_MSG_TYPE_NEWS                               "B  "
#define ENUM_MSG_TYPE_USER_REQUEST                       "BE "
#define ENUM_MSG_TYPE_USER_RESPONSE                      "BF "
#define ENUM_MSG_TYPE_APPLICATION_MESSAGE_REQUEST        "BW "
#define ENUM_MSG_TYPE_APPLICATION_MESSAGE_REQUEST_ACK    "BX "
#define ENUM_MSG_TYPE_APPLICATION_MESSAGE_REPORT         "BY "
#define ENUM_MSG_TYPE_ORDER_MASS_ACTION_REPORT           "BZ "
#define ENUM_MSG_TYPE_ORDER_MASS_ACTION_REQUEST          "CA "
#define ENUM_MSG_TYPE_USER_NOTIFICATION                  "CB "
#define ENUM_MSG_TYPE_PARTY_RISK_LIMITS_UPDATE_REPORT    "CR "
#define ENUM_MSG_TYPE_PARTY_RISK_LIMITS_REQUEST          "CL "
#define ENUM_MSG_TYPE_PARTY_RISK_LIMITS_DEFINITION_REQUEST "CS "
#define ENUM_MSG_TYPE_PARTY_RISK_LIMITS_REPORT           "CM "
#define ENUM_MSG_TYPE_PARTY_ENTITLEMENTS_UPDATE_REPORT   "CZ "
#define ENUM_MSG_TYPE_NEW_ORDER_SINGLE                   "D  "
#define ENUM_MSG_TYPE_ORDER_CANCEL_REQUEST               "F  "
#define ENUM_MSG_TYPE_ORDER_CANCEL_REPLACE_REQUEST       "G  "
#define ENUM_MSG_TYPE_QUOTE_REQUEST                      "R  "
#define ENUM_MSG_TYPE_MARKET_DATA_SNAPSHOT_FULL_REFRESH  "W  "
#define ENUM_MSG_TYPE_MASS_QUOTE_ACKNOWLEDGEMENT         "b  "
#define ENUM_MSG_TYPE_SECURITY_DEFINITION_REQUEST        "c  "
#define ENUM_MSG_TYPE_SECURITY_DEFINITION                "d  "
#define ENUM_MSG_TYPE_TRADING_SESSION_STATUS             "h  "
#define ENUM_MSG_TYPE_MASS_QUOTE                         "i  "
#define ENUM_MSG_TYPE_QUOTE                              "S  "
#define ENUM_MSG_TYPE_QUOTE_ACK                          "CW "
#define ENUM_MSG_TYPE_QUOTE_STATUS_REQUEST               "a  "
#define ENUM_MSG_TYPE_QUOTE_STATUS_REPORT                "AI "
#define ENUM_MSG_TYPE_QUOTE_RESPONSE                     "AJ "
#define ENUM_MSG_TYPE_REQUEST_ACKNOWLEDGE                "U1 "
#define ENUM_MSG_TYPE_SESSION_DETAILS_LIST_REQUEST       "U5 "
#define ENUM_MSG_TYPE_SESSION_DETAILS_LIST_RESPONSE      "U6 "
#define ENUM_MSG_TYPE_QUOTE_EXECUTION_REPORT             "U8 "
#define ENUM_MSG_TYPE_MM_PARAMETER_DEFINITION_REQUEST    "U14"
#define ENUM_MSG_TYPE_CROSS_REQUEST                      "DS "
#define ENUM_MSG_TYPE_CROSS_REQUEST_ACK                  "DT "
#define ENUM_MSG_TYPE_MM_PARAMETER_REQUEST               "U17"
#define ENUM_MSG_TYPE_MM_PARAMETER_RESPONSE              "U18"
#define ENUM_MSG_TYPE_SECURITY_STATUS_DEFINITION_REQUEST "U27"
#define ENUM_MSG_TYPE_SECURITY_STATUS                    "f  "
#define ENUM_MSG_TYPE_PARTY_DETAIL_LIST_REQUEST          "CF "
#define ENUM_MSG_TYPE_PARTY_DETAIL_LIST_REPORT           "CG "
#define ENUM_MSG_TYPE_TRADE_ENRICHMENT_LIST_REQUEST      "U7 "
#define ENUM_MSG_TYPE_TRADE_ENRICHMENT_LIST_REPORT       "U9 "
#define ENUM_MSG_TYPE_PARTY_ACTION_REPORT                "DI "
#define ENUM_MSG_TYPE_MARKET_DATA_INSTRUMENT             "U23"

// DataType MultiLegReportingType
#define ENUM_MULTI_LEG_REPORTING_TYPE_SINGLE_SECURITY    1
#define ENUM_MULTI_LEG_REPORTING_TYPE_INDIVIDUAL_LEG_OF_A_MULTILEG_SECURITY 2

// DataType MultilegModel
#define ENUM_MULTILEG_MODEL_PREDEFINED_MULTILEG_SECURITY 0
#define ENUM_MULTILEG_MODEL_USER_DEFINED_MULTLEG         1

// DataType MultilegPriceModel
#define ENUM_MULTILEG_PRICE_MODEL_STANDARD               0
#define ENUM_MULTILEG_PRICE_MODEL_USER_DEFINED           1

// DataType NegotiateUnderlying
#define ENUM_NEGOTIATE_UNDERLYING_NO                     0
#define ENUM_NEGOTIATE_UNDERLYING_YES                    1

// DataType NetworkMsgID
#define LEN_NETWORK_MSGID                                8

// DataType NumberOfRespDisclosureInstruction
#define ENUM_NUMBER_OF_RESP_DISCLOSURE_INSTRUCTION_NO    0
#define ENUM_NUMBER_OF_RESP_DISCLOSURE_INSTRUCTION_YES   1

// DataType OrdStatus
#define LEN_ORD_STATUS                                   1
#define ENUM_ORD_STATUS_NEW                              "0"
#define ENUM_ORD_STATUS_NEW_CHAR                         '0'
#define ENUM_ORD_STATUS_PARTIALLY_FILLED                 "1"
#define ENUM_ORD_STATUS_PARTIALLY_FILLED_CHAR            '1'
#define ENUM_ORD_STATUS_FILLED                           "2"
#define ENUM_ORD_STATUS_FILLED_CHAR                      '2'
#define ENUM_ORD_STATUS_CANCELED                         "4"
#define ENUM_ORD_STATUS_CANCELED_CHAR                    '4'
#define ENUM_ORD_STATUS_PENDING_CANCEL                   "6"
#define ENUM_ORD_STATUS_PENDING_CANCEL_CHAR              '6'
#define ENUM_ORD_STATUS_SUSPENDED                        "9"
#define ENUM_ORD_STATUS_SUSPENDED_CHAR                   '9'

// DataType OrdType
#define ENUM_ORD_TYPE_MARKET                             1
#define ENUM_ORD_TYPE_LIMIT                              2
#define ENUM_ORD_TYPE_STOP                               3
#define ENUM_ORD_TYPE_STOP_LIMIT                         4

// DataType OrderAttributeLiquidityProvision
#define ENUM_ORDER_ATTRIBUTE_LIQUIDITY_PROVISION_Y       1
#define ENUM_ORDER_ATTRIBUTE_LIQUIDITY_PROVISION_N       0

// DataType OrderAttributeRiskReduction
#define ENUM_ORDER_ATTRIBUTE_RISK_REDUCTION_Y            1
#define ENUM_ORDER_ATTRIBUTE_RISK_REDUCTION_N            0

// DataType OrderCategory
#define LEN_ORDER_CATEGORY                               1
#define ENUM_ORDER_CATEGORY_ORDER                        "1"
#define ENUM_ORDER_CATEGORY_ORDER_CHAR                   '1'
#define ENUM_ORDER_CATEGORY_QUOTE                        "2"
#define ENUM_ORDER_CATEGORY_QUOTE_CHAR                   '2'

// DataType OrderOrigination
#define ENUM_ORDER_ORIGINATION_DIRECT_ACCESS_OR_SPONSORED_ACCESS_CUSTOMER 5

// DataType OrderQtyDisclosureInstruction
#define ENUM_ORDER_QTY_DISCLOSURE_INSTRUCTION_NO         0
#define ENUM_ORDER_QTY_DISCLOSURE_INSTRUCTION_YES        1

// DataType OrderRoutingIndicator
#define LEN_ORDER_ROUTING_INDICATOR                      1
#define ENUM_ORDER_ROUTING_INDICATOR_YES                 "Y"
#define ENUM_ORDER_ROUTING_INDICATOR_YES_CHAR            'Y'
#define ENUM_ORDER_ROUTING_INDICATOR_NO                  "N"
#define ENUM_ORDER_ROUTING_INDICATOR_NO_CHAR             'N'

// DataType OrderSide
#define ENUM_ORDER_SIDE_BUY                              1
#define ENUM_ORDER_SIDE_SELL                             2

// DataType OwnershipIndicator
#define ENUM_OWNERSHIP_INDICATOR_NO_CHANGE_OF_OWNERSHIP  0
#define ENUM_OWNERSHIP_INDICATOR_CHANGE_TO_EXECUTING_TRADER 1

// DataType Pad1
#define LEN_PAD1                                         1

// DataType Pad1_1
#define LEN_PAD1_1                                       1

// DataType Pad1_3
#define LEN_PAD1_3                                       1

// DataType Pad2
#define LEN_PAD2                                         2

// DataType Pad2_1
#define LEN_PAD2_1                                       2

// DataType Pad2_2
#define LEN_PAD2_2                                       2

// DataType Pad3
#define LEN_PAD3                                         3

// DataType Pad3_2
#define LEN_PAD3_2                                       3

// DataType Pad4
#define LEN_PAD4                                         4

// DataType Pad4_2
#define LEN_PAD4_2                                       4

// DataType Pad5
#define LEN_PAD5                                         5

// DataType Pad5_1
#define LEN_PAD5_1                                       5

// DataType Pad6
#define LEN_PAD6                                         6

// DataType Pad6_1
#define LEN_PAD6_1                                       6

// DataType Pad6_2
#define LEN_PAD6_2                                       6

// DataType Pad7
#define LEN_PAD7                                         7

// DataType Pad7_1
#define LEN_PAD7_1                                       7

// DataType PartyActionType
#define ENUM_PARTY_ACTION_TYPE_HALT_TRADING              1
#define ENUM_PARTY_ACTION_TYPE_REINSTATE                 2

// DataType PartyDetailDeskID
#define LEN_PARTY_DETAIL_DESKID                          3

// DataType PartyDetailExecutingTrader
#define LEN_PARTY_DETAIL_EXECUTING_TRADER                6

// DataType PartyDetailExecutingUnit
#define LEN_PARTY_DETAIL_EXECUTING_UNIT                  5

// DataType PartyDetailRoleQualifier
#define ENUM_PARTY_DETAIL_ROLE_QUALIFIER_TRADER          10
#define ENUM_PARTY_DETAIL_ROLE_QUALIFIER_HEAD_TRADER     11
#define ENUM_PARTY_DETAIL_ROLE_QUALIFIER_SUPERVISOR      12

// DataType PartyDetailStatus
#define ENUM_PARTY_DETAIL_STATUS_ACTIVE                  0
#define ENUM_PARTY_DETAIL_STATUS_SUSPEND                 1

// DataType PartyEnteringTrader
#define LEN_PARTY_ENTERING_TRADER                        6

// DataType PartyExecutingFirm
#define LEN_PARTY_EXECUTING_FIRM                         5

// DataType PartyExecutingTrader
#define LEN_PARTY_EXECUTING_TRADER                       6

// DataType PartyExecutingUnit
#define LEN_PARTY_EXECUTING_UNIT                         5

// DataType PartyIDBeneficiary
#define LEN_PARTY_ID_BENEFICIARY                         9

// DataType PartyIDEnteringFirm
#define ENUM_PARTY_ID_ENTERING_FIRM_PARTICIPANT          1
#define ENUM_PARTY_ID_ENTERING_FIRM_MARKET_SUPERVISION   2

// DataType PartyIDLocationID
#define LEN_PARTY_ID_LOCATIONID                          2

// DataType PartyIDOrderOriginationFirm
#define LEN_PARTY_ID_ORDER_ORIGINATION_FIRM              7

// DataType PartyIDOriginationMarket
#define ENUM_PARTY_ID_ORIGINATION_MARKET_XKFE            1

// DataType PartyIDPositionAccount
#define LEN_PARTY_ID_POSITION_ACCOUNT                    32

// DataType PartyIDSettlementLocation
#define ENUM_PARTY_ID_SETTLEMENT_LOCATION_CLEARSTREM_BANKING_FRANKFURT 1
#define ENUM_PARTY_ID_SETTLEMENT_LOCATION_CLEARSTREM_BANKING_LUXEMBURG 2
#define ENUM_PARTY_ID_SETTLEMENT_LOCATION_CLS_GROUP      3
#define ENUM_PARTY_ID_SETTLEMENT_LOCATION_EUROCLEAR      4

// DataType PartyIDTakeUpTradingFirm
#define LEN_PARTY_ID_TAKE_UP_TRADING_FIRM                5

// DataType PartyIdInvestmentDecisionMakerQualifier
#define ENUM_PARTY_ID_INVESTMENT_DECISION_MAKER_QUALIFIER_ALGO 22
#define ENUM_PARTY_ID_INVESTMENT_DECISION_MAKER_QUALIFIER_HUMAN 24

// DataType PartySubIDType
#define ENUM_PARTY_SUB_ID_TYPE_BUYER                     14001
#define ENUM_PARTY_SUB_ID_TYPE_SELLER                    14002

// DataType Password
#define LEN_PASSWORD                                     32

// DataType PositionEffect
#define LEN_POSITION_EFFECT                              1
#define ENUM_POSITION_EFFECT_CLOSE                       "C"
#define ENUM_POSITION_EFFECT_CLOSE_CHAR                  'C'
#define ENUM_POSITION_EFFECT_OPEN                        "O"
#define ENUM_POSITION_EFFECT_OPEN_CHAR                   'O'

// DataType PriceDisclosureInstruction
#define ENUM_PRICE_DISCLOSURE_INSTRUCTION_NO             0
#define ENUM_PRICE_DISCLOSURE_INSTRUCTION_YES            1

// DataType PriceValidityCheckType
#define ENUM_PRICE_VALIDITY_CHECK_TYPE_NONE              0
#define ENUM_PRICE_VALIDITY_CHECK_TYPE_OPTIONAL          1
#define ENUM_PRICE_VALIDITY_CHECK_TYPE_MANDATORY         2

// DataType ProductComplex
#define ENUM_PRODUCT_COMPLEX_SIMPLE_INSTRUMENT           1
#define ENUM_PRODUCT_COMPLEX_STANDARD_OPTION_STRATEGY    2
#define ENUM_PRODUCT_COMPLEX_NON_STANDARD_OPTION_STRATEGY 3
#define ENUM_PRODUCT_COMPLEX_VOLATILITY_STRATEGY         4
#define ENUM_PRODUCT_COMPLEX_FUTURES_SPREAD              5
#define ENUM_PRODUCT_COMPLEX_INTER_PRODUCT_SPREAD        6
#define ENUM_PRODUCT_COMPLEX_STANDARD_FUTURE_STRATEGY    7
#define ENUM_PRODUCT_COMPLEX_PACK_AND_BUNDLE             8
#define ENUM_PRODUCT_COMPLEX_STRIP                       9
#define ENUM_PRODUCT_COMPLEX_FLEXIBLE_SIMPLE_INSTRUMENT  10

// DataType PutOrCall
#define ENUM_PUT_OR_CALL_PUT                             0
#define ENUM_PUT_OR_CALL_CALL                            1

// DataType QuoteCancelType
#define ENUM_QUOTE_CANCEL_TYPE_CANCEL_ALL_QUOTES         4

// DataType QuoteEntryRejectReason
#define ENUM_QUOTE_ENTRY_REJECT_REASON_UNKNOWN_SECURITY  1
#define ENUM_QUOTE_ENTRY_REJECT_REASON_DUPLICATE_QUOTE   6
#define ENUM_QUOTE_ENTRY_REJECT_REASON_INVALID_PRICE     8
#define ENUM_QUOTE_ENTRY_REJECT_REASON_NO_REFERENCE_PRICE_AVAILABLE 16
#define ENUM_QUOTE_ENTRY_REJECT_REASON_NO_SINGLE_SIDED_QUOTES 100
#define ENUM_QUOTE_ENTRY_REJECT_REASON_INVALID_QUOTING_MODEL 103
#define ENUM_QUOTE_ENTRY_REJECT_REASON_INVALID_SIZE      106
#define ENUM_QUOTE_ENTRY_REJECT_REASON_INVALID_UNDERLYING_PRICE 107
#define ENUM_QUOTE_ENTRY_REJECT_REASON_BID_PRICE_NOT_REASONABLE 108
#define ENUM_QUOTE_ENTRY_REJECT_REASON_ASK_PRICE_NOT_REASONABLE 109
#define ENUM_QUOTE_ENTRY_REJECT_REASON_BID_PRICE_EXCEEDS_RANGE 110
#define ENUM_QUOTE_ENTRY_REJECT_REASON_ASK_PRICE_EXCEEDS_RANGE 111
#define ENUM_QUOTE_ENTRY_REJECT_REASON_INSTRUMENT_STATE_FREEZE 115
#define ENUM_QUOTE_ENTRY_REJECT_REASON_DELETION_ALREADY_PENDING 116
#define ENUM_QUOTE_ENTRY_REJECT_REASON_PRE_TRADE_RISK_SESSION_LIMIT_EXCEEDED 117
#define ENUM_QUOTE_ENTRY_REJECT_REASON_PRE_TRADE_RISK_BU_LIMIT_EXCEEDED 118
#define ENUM_QUOTE_ENTRY_REJECT_REASON_ENTITLEMENT_NOT_ASSIGNED_FOR_UNDERLYING 119
#define ENUM_QUOTE_ENTRY_REJECT_REASON_CURRENTLY_NOT_TRADEABLE_ON_BOOK 124
#define ENUM_QUOTE_ENTRY_REJECT_REASON_QUANTITY_LIMIT_EXCEEDED 125
#define ENUM_QUOTE_ENTRY_REJECT_REASON_VALUE_LIMIT_EXCEEDED 126
#define ENUM_QUOTE_ENTRY_REJECT_REASON_INVALID_QUOTE_SPREAD 127
#define ENUM_QUOTE_ENTRY_REJECT_REASON_BOOK_OR_CANCEL    128
#define ENUM_QUOTE_ENTRY_REJECT_REASON_CANT_PROC_IN_CURR_INSTR_STATE 131
#define ENUM_QUOTE_ENTRY_REJECT_REASON_INVALID_QUOTE_TYPE 134
#define ENUM_QUOTE_ENTRY_REJECT_REASON_TRADING_INDICATION_RUNNING_FOR_TRADER 143
#define ENUM_QUOTE_ENTRY_REJECT_REASON_ON_BOOK_TRADING_DISABLED_FOR_INSTRUMENT_TYPE 144
#define ENUM_QUOTE_ENTRY_REJECT_REASON_LIQUIDITY_PROVIDER_PROTECTION_BID_SIDE_CANCELLED 147
#define ENUM_QUOTE_ENTRY_REJECT_REASON_LIQUIDITY_PROVIDER_PROTECTION_ASK_SIDE_CANCELLED 148
#define ENUM_QUOTE_ENTRY_REJECT_REASON_OUTSIDE_QUOTING_PERIOD 155
#define ENUM_QUOTE_ENTRY_REJECT_REASON_MATCH_PRICE_NOT_ON_PRICE_STEP 156
#define ENUM_QUOTE_ENTRY_REJECT_REASON_QUANTITY_LIMIT_EXCEEDS_TSL 161

// DataType QuoteEntryStatus
#define ENUM_QUOTE_ENTRY_STATUS_ACCEPTED                 0
#define ENUM_QUOTE_ENTRY_STATUS_REJECTED                 5
#define ENUM_QUOTE_ENTRY_STATUS_REMOVED_AND_REJECTED     6
#define ENUM_QUOTE_ENTRY_STATUS_PENDING                  10

// DataType QuoteEventLiquidityInd
#define ENUM_QUOTE_EVENT_LIQUIDITY_IND_ADDED_LIQUIDITY   1
#define ENUM_QUOTE_EVENT_LIQUIDITY_IND_REMOVED_LIQUIDITY 2
#define ENUM_QUOTE_EVENT_LIQUIDITY_IND_AUCTION           4

// DataType QuoteEventReason
#define ENUM_QUOTE_EVENT_REASON_PENDING_CANCELLATION_EXECUTED 14
#define ENUM_QUOTE_EVENT_REASON_INVALID_PRICE            15
#define ENUM_QUOTE_EVENT_REASON_CROSS_REJECTED           16
#define ENUM_QUOTE_EVENT_REASON_BOOK_OR_CANCEL           17
#define ENUM_QUOTE_EVENT_REASON_PLP                      18

// DataType QuoteEventSide
#define ENUM_QUOTE_EVENT_SIDE_BUY                        1
#define ENUM_QUOTE_EVENT_SIDE_SELL                       2

// DataType QuoteEventType
#define ENUM_QUOTE_EVENT_TYPE_MODIFIED_QUOTE_SIDE        2
#define ENUM_QUOTE_EVENT_TYPE_REMOVED_QUOTE_SIDE         3
#define ENUM_QUOTE_EVENT_TYPE_PARTIALLY_FILLED           4
#define ENUM_QUOTE_EVENT_TYPE_FILLED                     5

// DataType QuoteInstruction
#define ENUM_QUOTE_INSTRUCTION_DO_NOT_QUOTE              0
#define ENUM_QUOTE_INSTRUCTION_QUOTE                     1

// DataType QuoteQualifier
#define ENUM_QUOTE_QUALIFIER_NONE                        0
#define ENUM_QUOTE_QUALIFIER_STP                         4

// DataType QuoteRefPriceSource
#define ENUM_QUOTE_REF_PRICE_SOURCE_UNDERLYING           1
#define ENUM_QUOTE_REF_PRICE_SOURCE_CUSTOM_UNDERLYING_PRICE 2
#define ENUM_QUOTE_REF_PRICE_SOURCE_REF_PRICE            3

// DataType QuoteReqID
#define LEN_QUOTE_REQID                                  20

// DataType QuoteSizeType
#define ENUM_QUOTE_SIZE_TYPE_TOTAL_SIZE                  1
#define ENUM_QUOTE_SIZE_TYPE_OPEN_SIZE                   2

// DataType QuoteStatus
#define ENUM_QUOTE_STATUS_REMOVED                        6
#define ENUM_QUOTE_STATUS_EXPIRED                        7
#define ENUM_QUOTE_STATUS_ACTIVE                         16

// DataType QuoteSubType
#define ENUM_QUOTE_SUB_TYPE_WORKING_DELTA                1

// DataType QuoteType
#define ENUM_QUOTE_TYPE_INDICATIVE                       0
#define ENUM_QUOTE_TYPE_TRADEABLE                        1
#define ENUM_QUOTE_TYPE_TRADEABLE_BOC                    100

// DataType QuotingStatus
#define ENUM_QUOTING_STATUS_OPEN_ACTIVE                  1
#define ENUM_QUOTING_STATUS_OPEN_IDLE                    2
#define ENUM_QUOTING_STATUS_CLOSED_INACTIVE              3

// DataType RefApplID
#define ENUM_REF_APPLID_TRADE                            1
#define ENUM_REF_APPLID_NEWS                             2
#define ENUM_REF_APPLID_SERVICE_AVAILABILITY             3
#define ENUM_REF_APPLID_SESSION_DATA                     4
#define ENUM_REF_APPLID_LISTENER_DATA                    5
#define ENUM_REF_APPLID_RISK_CONTROL                     6
#define ENUM_REF_APPLID_TES_MAINTENANCE                  7
#define ENUM_REF_APPLID_TES_TRADE                        8
#define ENUM_REF_APPLID_SRQS_MAINTENANCE                 9
#define ENUM_REF_APPLID_SERVICE_AVAILABILITY_MARKET      10

// DataType RefApplLastMsgID
#define LEN_REF_APPL_LAST_MSGID                          16

// DataType RegulatoryTradeID
#define LEN_REGULATORY_TRADEID                           52

// DataType RelatedProductComplex
#define ENUM_RELATED_PRODUCT_COMPLEX_STANDARD_OPTION_STRATEGY 2
#define ENUM_RELATED_PRODUCT_COMPLEX_NON_STANDARD_OPTION_STRATEGY 3
#define ENUM_RELATED_PRODUCT_COMPLEX_VOLATILITY_STRATEGY 4
#define ENUM_RELATED_PRODUCT_COMPLEX_FUTURES_SPREAD      5
#define ENUM_RELATED_PRODUCT_COMPLEX_INTER_PRODUCT_SPREAD 6
#define ENUM_RELATED_PRODUCT_COMPLEX_STANDARD_FUTURE_STRATEGY 7
#define ENUM_RELATED_PRODUCT_COMPLEX_PACK_AND_BUNDLE     8
#define ENUM_RELATED_PRODUCT_COMPLEX_STRIP               9

// DataType RequestingPartyClearingFirm
#define LEN_REQUESTING_PARTY_CLEARING_FIRM               9

// DataType RequestingPartyEnteringFirm
#define LEN_REQUESTING_PARTY_ENTERING_FIRM               9

// DataType RequestingPartyIDEnteringFirm
#define ENUM_REQUESTING_PARTY_ID_ENTERING_FIRM_PARTICIPANT 1
#define ENUM_REQUESTING_PARTY_ID_ENTERING_FIRM_MARKET_SUPERVISION 2

// DataType RequestingPartyIDExecutingSystem
#define ENUM_REQUESTING_PARTY_ID_EXECUTING_SYSTEM_EUREX_CLEARING 1
#define ENUM_REQUESTING_PARTY_ID_EXECUTING_SYSTEM_T7     2

// DataType RequestingPartySubIDType
#define ENUM_REQUESTING_PARTY_SUB_ID_TYPE_REQUESTER      5000
#define ENUM_REQUESTING_PARTY_SUB_ID_TYPE_RESPONDER      5001
#define ENUM_REQUESTING_PARTY_SUB_ID_TYPE_BOTH           5002
#define ENUM_REQUESTING_PARTY_SUB_ID_TYPE_SYSTEM         5003

// DataType RiskLimitAction
#define ENUM_RISK_LIMIT_ACTION_QUEUE_INBOUND             0
#define ENUM_RISK_LIMIT_ACTION_REJECT                    2
#define ENUM_RISK_LIMIT_ACTION_WARNING                   4

// DataType RiskLimitGroup
#define LEN_RISK_LIMIT_GROUP                             3

// DataType RiskLimitPlatform
#define ENUM_RISK_LIMIT_PLATFORM_ON_BOOK                 0
#define ENUM_RISK_LIMIT_PLATFORM_OFF_BOOK                1

// DataType RiskLimitRequestingPartyRole
#define ENUM_RISK_LIMIT_REQUESTING_PARTY_ROLE_CLEARING_FIRM 4
#define ENUM_RISK_LIMIT_REQUESTING_PARTY_ROLE_EXCHANGE   22
#define ENUM_RISK_LIMIT_REQUESTING_PARTY_ROLE_EXECUTING_UNIT 59

// DataType RiskLimitType
#define ENUM_RISK_LIMIT_TYPE_LONG_LIMIT                  4
#define ENUM_RISK_LIMIT_TYPE_SHORT_LIMIT                 5

// DataType RiskLimitViolationIndicator
#define ENUM_RISK_LIMIT_VIOLATION_INDICATOR_NO           0
#define ENUM_RISK_LIMIT_VIOLATION_INDICATOR_YES          1

// DataType RootPartyClearingFirm
#define LEN_ROOT_PARTY_CLEARING_FIRM                     5

// DataType RootPartyClearingOrganization
#define LEN_ROOT_PARTY_CLEARING_ORGANIZATION             4

// DataType RootPartyContraFirm
#define LEN_ROOT_PARTY_CONTRA_FIRM                       5

// DataType RootPartyContraTrader
#define LEN_ROOT_PARTY_CONTRA_TRADER                     6

// DataType RootPartyEnteringTrader
#define LEN_ROOT_PARTY_ENTERING_TRADER                   6

// DataType RootPartyExecutingFirm
#define LEN_ROOT_PARTY_EXECUTING_FIRM                    5

// DataType RootPartyExecutingTrader
#define LEN_ROOT_PARTY_EXECUTING_TRADER                  6

// DataType RootPartyIDBeneficiary
#define LEN_ROOT_PARTY_ID_BENEFICIARY                    9

// DataType RootPartyIDExecutionVenue
#define LEN_ROOT_PARTY_ID_EXECUTION_VENUE                4

// DataType RootPartyIDInvestmentDecisionMakerQualifier
#define ENUM_ROOT_PARTY_ID_INVESTMENT_DECISION_MAKER_QUALIFIER_ALGO 22
#define ENUM_ROOT_PARTY_ID_INVESTMENT_DECISION_MAKER_QUALIFIER_HUMAN 24

// DataType RootPartyIDOrderOriginationFirm
#define LEN_ROOT_PARTY_ID_ORDER_ORIGINATION_FIRM         7

// DataType RootPartyIDPositionAccount
#define LEN_ROOT_PARTY_ID_POSITION_ACCOUNT               32

// DataType RootPartyIDTakeUpTradingFirm
#define LEN_ROOT_PARTY_ID_TAKE_UP_TRADING_FIRM           5

// DataType RootPartySubIDType
#define ENUM_ROOT_PARTY_SUB_ID_TYPE_BUYER                4001
#define ENUM_ROOT_PARTY_SUB_ID_TYPE_SELLER               4002
#define ENUM_ROOT_PARTY_SUB_ID_TYPE_BROKER               4003

// DataType SecondaryGatewayStatus
#define ENUM_SECONDARY_GATEWAY_STATUS_STANDBY            0
#define ENUM_SECONDARY_GATEWAY_STATUS_ACTIVE             1

// DataType SelectiveRequestForQuoteServiceStatus
#define ENUM_SELECTIVE_REQUEST_FOR_QUOTE_SERVICE_STATUS_UNAVAILABLE 0
#define ENUM_SELECTIVE_REQUEST_FOR_QUOTE_SERVICE_STATUS_AVAILABLE 1

// DataType SessionMode
#define ENUM_SESSION_MODE_HF                             1
#define ENUM_SESSION_MODE_LF                             2
#define ENUM_SESSION_MODE_GUI                            3

// DataType SessionRejectReason
#define ENUM_SESSION_REJECT_REASON_REQUIRED_TAG_MISSING  1
#define ENUM_SESSION_REJECT_REASON_VALUE_IS_INCORRECT    5
#define ENUM_SESSION_REJECT_REASON_DECRYPTION_PROBLEM    7
#define ENUM_SESSION_REJECT_REASON_INVALID_MSGID         11
#define ENUM_SESSION_REJECT_REASON_INCORRECT_NUM_IN_GROUP_COUNT 16
#define ENUM_SESSION_REJECT_REASON_OTHER                 99
#define ENUM_SESSION_REJECT_REASON_THROTTLE_LIMIT_EXCEEDED 100
#define ENUM_SESSION_REJECT_REASON_EXPOSURE_LIMIT_EXCEEDED 101
#define ENUM_SESSION_REJECT_REASON_SERVICE_TEMPORARILY_NOT_AVAILABLE 102
#define ENUM_SESSION_REJECT_REASON_SERVICE_NOT_AVAILABLE 103
#define ENUM_SESSION_REJECT_REASON_OUTBOUND_CONVERSION_ERROR 105
#define ENUM_SESSION_REJECT_REASON_HEARTBEAT_VIOLATION   152
#define ENUM_SESSION_REJECT_REASON_INTERNAL_TECHNICAL_ERROR 200
#define ENUM_SESSION_REJECT_REASON_VALIDATION_ERROR      210
#define ENUM_SESSION_REJECT_REASON_USER_ALREADY_LOGGED_IN 211
#define ENUM_SESSION_REJECT_REASON_GATEWAY_IS_STANDBY    216
#define ENUM_SESSION_REJECT_REASON_SESSION_LOGIN_LIMIT_REACHED 217
#define ENUM_SESSION_REJECT_REASON_USER_ENTITLEMENT_DATA_TIMEOUT 223
#define ENUM_SESSION_REJECT_REASON_PS_GATEWAY_SESSION_LIMIT_REACHED 224
#define ENUM_SESSION_REJECT_REASON_USER_LOGIN_LIMIT_REACHED 225
#define ENUM_SESSION_REJECT_REASON_OUTSTANDING_LOGINS_BU_LIMIT_REACHED 226
#define ENUM_SESSION_REJECT_REASON_OUTSTANDING_LOGINS_SESSION_LIMIT_REACHED 227
#define ENUM_SESSION_REJECT_REASON_ORDER_NOT_FOUND       10000
#define ENUM_SESSION_REJECT_REASON_PRICE_NOT_REASONABLE  10001
#define ENUM_SESSION_REJECT_REASON_CLIENT_ORDERID_NOT_UNIQUE 10002
#define ENUM_SESSION_REJECT_REASON_QUOTE_ACTIVATION_IN_PROGRESS 10003
#define ENUM_SESSION_REJECT_REASON_BU_BOOK_ORDER_LIMIT_EXCEEDED 10004
#define ENUM_SESSION_REJECT_REASON_SESSION_BOOK_ORDER_LIMIT_EXCEEDED 10005
#define ENUM_SESSION_REJECT_REASON_STOP_BID_PRICE_NOT_REASONABLE 10006
#define ENUM_SESSION_REJECT_REASON_STOP_ASK_PRICE_NOT_REASONABLE 10007
#define ENUM_SESSION_REJECT_REASON_ORDER_NOT_EXECUTABLE_WITHIN_VALIDITY 10008
#define ENUM_SESSION_REJECT_REASON_CREATE_CI_THROTTLE_EXCEEDED 10010
#define ENUM_SESSION_REJECT_REASON_TRANSACTION_NOT_ALLOWED_IN_CURRENT_STATE 10011

// DataType SessionStatus
#define ENUM_SESSION_STATUS_ACTIVE                       0
#define ENUM_SESSION_STATUS_LOGOUT                       4

// DataType SessionSubMode
#define ENUM_SESSION_SUB_MODE_REGULAR_TRADING_SESSION    0
#define ENUM_SESSION_SUB_MODE_FIX_TRADING_SESSION        1
#define ENUM_SESSION_SUB_MODE_REGULAR_BACK_OFFICE_SESSION 2

// DataType SettlMethod
#define LEN_SETTL_METHOD                                 1
#define ENUM_SETTL_METHOD_CASH_SETTLEMENT                "C"
#define ENUM_SETTL_METHOD_CASH_SETTLEMENT_CHAR           'C'
#define ENUM_SETTL_METHOD_PHYSICAL_SETTLEMENT            "P"
#define ENUM_SETTL_METHOD_PHYSICAL_SETTLEMENT_CHAR       'P'

// DataType Side
#define ENUM_SIDE_BUY                                    1
#define ENUM_SIDE_SELL                                   2

// DataType SideComplianceText
#define LEN_SIDE_COMPLIANCE_TEXT                         20

// DataType SideDisclosureInstruction
#define ENUM_SIDE_DISCLOSURE_INSTRUCTION_NO              0
#define ENUM_SIDE_DISCLOSURE_INSTRUCTION_YES             1

// DataType SideLiquidityInd
#define ENUM_SIDE_LIQUIDITY_IND_ADDED_LIQUIDITY          1
#define ENUM_SIDE_LIQUIDITY_IND_REMOVED_LIQUIDITY        2
#define ENUM_SIDE_LIQUIDITY_IND_AUCTION                  4

// DataType SideTrdSubTyp
#define ENUM_SIDE_TRD_SUB_TYP_BLOCK_TRADE                2001
#define ENUM_SIDE_TRD_SUB_TYP_TRADE_AT_MARKET            2004

// DataType SkipValidations
#define ENUM_SKIP_VALIDATIONS_FALSE                      0
#define ENUM_SKIP_VALIDATIONS_TRUE                       1

// DataType Symbol
#define LEN_SYMBOL                                       4

// DataType T7EntryServiceRtmStatus
#define ENUM_T7_ENTRY_SERVICE_RTM_STATUS_UNAVAILABLE     0
#define ENUM_T7_ENTRY_SERVICE_RTM_STATUS_AVAILABLE       1

// DataType T7EntryServiceStatus
#define ENUM_T7_ENTRY_SERVICE_STATUS_UNAVAILABLE         0
#define ENUM_T7_ENTRY_SERVICE_STATUS_AVAILABLE           1

// DataType TargetPartyEnteringTrader
#define LEN_TARGET_PARTY_ENTERING_TRADER                 6

// DataType TargetPartyExecutingFirm
#define LEN_TARGET_PARTY_EXECUTING_FIRM                  5

// DataType TargetPartyExecutingTrader
#define LEN_TARGET_PARTY_EXECUTING_TRADER                6

// DataType TargetPartyIDDeskID
#define LEN_TARGET_PARTY_ID_DESKID                       3

// DataType Text
#define LEN_TEXT                                         12

// DataType TimeInForce
#define ENUM_TIME_IN_FORCE_DAY                           0
#define ENUM_TIME_IN_FORCE_GTC                           1
#define ENUM_TIME_IN_FORCE_IOC                           3
#define ENUM_TIME_IN_FORCE_GTD                           6

// DataType TradSesEvent
#define ENUM_TRAD_SES_EVENT_START_OF_SERVICE             101
#define ENUM_TRAD_SES_EVENT_MARKET_RESET                 102
#define ENUM_TRAD_SES_EVENT_END_OF_RESTATEMENT           103
#define ENUM_TRAD_SES_EVENT_END_OF_DAY_SERVICE           104
#define ENUM_TRAD_SES_EVENT_SERVICE_RESUMED              105

// DataType TradSesMode
#define ENUM_TRAD_SES_MODE_TESTING                       1
#define ENUM_TRAD_SES_MODE_SIMULATED                     2
#define ENUM_TRAD_SES_MODE_PRODUCTION                    3
#define ENUM_TRAD_SES_MODE_ACCEPTANCE                    4
#define ENUM_TRAD_SES_MODE_DISASTER_RECOVERY             5

// DataType TradeAllocStatus
#define ENUM_TRADE_ALLOC_STATUS_PENDING                  1
#define ENUM_TRADE_ALLOC_STATUS_APPROVED                 2
#define ENUM_TRADE_ALLOC_STATUS_AUTO_APPROVED            3
#define ENUM_TRADE_ALLOC_STATUS_UPLOADED                 4
#define ENUM_TRADE_ALLOC_STATUS_CANCELED                 5

// DataType TradeManagerStatus
#define ENUM_TRADE_MANAGER_STATUS_UNAVAILABLE            0
#define ENUM_TRADE_MANAGER_STATUS_AVAILABLE              1

// DataType TradePlatform
#define ENUM_TRADE_PLATFORM_OFF_BOOK                     0
#define ENUM_TRADE_PLATFORM_ON_BOOK                      1

// DataType TradePublishIndicator
#define ENUM_TRADE_PUBLISH_INDICATOR_DO_NOT_PUBLISH_TRADE 0
#define ENUM_TRADE_PUBLISH_INDICATOR_PUBLISH_TRADE       1
#define ENUM_TRADE_PUBLISH_INDICATOR_DEFERRED_PUBLICATION 2
#define ENUM_TRADE_PUBLISH_INDICATOR_PUBLISHED           3

// DataType TradeReportID
#define LEN_TRADE_REPORTID                               20

// DataType TradeReportText
#define LEN_TRADE_REPORT_TEXT                            20

// DataType TradeReportType
#define ENUM_TRADE_REPORT_TYPE_SUBMIT                    0
#define ENUM_TRADE_REPORT_TYPE_ALLEGED                   1
#define ENUM_TRADE_REPORT_TYPE_ACCEPT                    2
#define ENUM_TRADE_REPORT_TYPE_DECLINE                   3
#define ENUM_TRADE_REPORT_TYPE_NO_WAS_REPLACED           5
#define ENUM_TRADE_REPORT_TYPE_TRADE_REPORT_CANCEL       6
#define ENUM_TRADE_REPORT_TYPE_TRADE_BREAK               7
#define ENUM_TRADE_REPORT_TYPE_ALLEGED_NEW               11
#define ENUM_TRADE_REPORT_TYPE_ALLEGED_NO_WAS            13

// DataType TradeRequestResult
#define ENUM_TRADE_REQUEST_RESULT_CANCEL_PENDING         100
#define ENUM_TRADE_REQUEST_RESULT_CANCEL_DECLINED        101
#define ENUM_TRADE_REQUEST_RESULT_CANCEL_APPROVED        102

// DataType TradeUnderlying
#define ENUM_TRADE_UNDERLYING_NO                         1
#define ENUM_TRADE_UNDERLYING_YES                        2

// DataType TradingCapacity
#define ENUM_TRADING_CAPACITY_CUSTOMER                   1
#define ENUM_TRADING_CAPACITY_PRINCIPAL                  5
#define ENUM_TRADING_CAPACITY_MARKET_MAKER               6

// DataType TradingSessionSubID
#define ENUM_TRADING_SESSION_SUBID_CLOSING_AUCTION       4

// DataType TransactionDelayIndicator
#define ENUM_TRANSACTION_DELAY_INDICATOR_NOT_DELAYED     0
#define ENUM_TRANSACTION_DELAY_INDICATOR_DELAYED         1

// DataType TransferReason
#define ENUM_TRANSFER_REASON_OWNER                       1
#define ENUM_TRANSFER_REASON_CLEARER                     2

// DataType TrdRptStatus
#define ENUM_TRD_RPT_STATUS_ACCEPTED                     0
#define ENUM_TRD_RPT_STATUS_REJECTED                     1
#define ENUM_TRD_RPT_STATUS_CANCELLED                    2
#define ENUM_TRD_RPT_STATUS_PENDING_NEW                  4
#define ENUM_TRD_RPT_STATUS_TERMINATED                   7
#define ENUM_TRD_RPT_STATUS_DEEMED_VERIFIED              9

// DataType TrdType
#define ENUM_TRD_TYPE_BLOCK_TRADE                        1
#define ENUM_TRD_TYPE_EXCHANGE_FOR_SWAP                  12
#define ENUM_TRD_TYPE_VOLA_TRADE                         1000
#define ENUM_TRD_TYPE_EFP_FIN_TRADE                      1001
#define ENUM_TRD_TYPE_EFP_INDEX_FUTURES_TRADE            1002
#define ENUM_TRD_TYPE_TRADE_AT_MARKET                    1004
#define ENUM_TRD_TYPE_ENLIGHT                            1006
#define ENUM_TRD_TYPE_BTRF                               1010

// DataType Triggered
#define ENUM_TRIGGERED_NOT_TRIGGERED                     0
#define ENUM_TRIGGERED_TRIGGERED_STOP                    1
#define ENUM_TRIGGERED_TRIGGERED_OCO                     2

// DataType UnderlyingCurrency
#define LEN_UNDERLYING_CURRENCY                          3

// DataType UnderlyingIssuer
#define LEN_UNDERLYING_ISSUER                            30

// DataType UnderlyingSecurityDesc
#define LEN_UNDERLYING_SECURITY_DESC                     30

// DataType UnderlyingSecurityID
#define LEN_UNDERLYING_SECURITYID                        12

// DataType UnderlyingStipType
#define LEN_UNDERLYING_STIP_TYPE                         7
#define ENUM_UNDERLYING_STIP_TYPE_PAY_FREQUENCY          "PAYFREQ"

// DataType UnderlyingStipValue
#define LEN_UNDERLYING_STIP_VALUE                        32

// DataType UserStatus
#define ENUM_USER_STATUS_USER_FORCED_LOGOUT              7
#define ENUM_USER_STATUS_USER_STOPPED                    10
#define ENUM_USER_STATUS_USER_RELEASED                   11

// DataType ValueCheckTypeMinLotSize
#define ENUM_VALUE_CHECK_TYPE_MIN_LOT_SIZE_DO_NOT_CHECK  0
#define ENUM_VALUE_CHECK_TYPE_MIN_LOT_SIZE_CHECK         1

// DataType ValueCheckTypeQuantity
#define ENUM_VALUE_CHECK_TYPE_QUANTITY_DO_NOT_CHECK      0
#define ENUM_VALUE_CHECK_TYPE_QUANTITY_CHECK             1

// DataType ValueCheckTypeValue
#define ENUM_VALUE_CHECK_TYPE_VALUE_DO_NOT_CHECK         0
#define ENUM_VALUE_CHECK_TYPE_VALUE_CHECK                1

// DataType VarText
#define LEN_VAR_TEXT                                     2000

/*
 * Structure defines for components and sequences
 */

// Structure: AffectedOrderRequestsGrp
typedef struct
{
    uint32_t AffectedOrderRequestID;
    char Pad4[LEN_PAD4];
} AffectedOrderRequestsGrpSeqT;

// Structure: BasketExecGrp
typedef struct
{
    uint32_t PackageID;
    int32_t SideMarketSegmentID;
    uint32_t AllocID;
    uint16_t SideTrdSubTyp;
    char Pad2[LEN_PAD2];
} BasketExecGrpSeqT;

// Structure: BasketRootPartyGrp
typedef struct
{
    uint16_t RootPartySubIDType;
    char RootPartyContraFirm[LEN_ROOT_PARTY_CONTRA_FIRM];
    char RootPartyContraTrader[LEN_ROOT_PARTY_CONTRA_TRADER];
    char BasketSideTradeReportID[LEN_BASKET_SIDE_TRADE_REPORTID];
    char Pad7[LEN_PAD7];
} BasketRootPartyGrpSeqT;

// Structure: BasketSideAllocExtBCGrp
typedef struct
{
    int64_t AllocQty;
    int64_t SecurityID;
    int64_t LastPx;
    uint64_t TransBkdTime;
    int64_t RelatedClosePrice;
    uint32_t PackageID;
    int32_t SideMarketSegmentID;
    uint32_t AllocID;
    uint32_t NegotiationID;
    uint32_t SRQSRelatedTradeID;
    uint16_t SideTrdSubTyp;
    uint16_t PartySubIDType;
    uint8_t Side;
    char PositionEffect[LEN_POSITION_EFFECT];
    uint8_t EffectOnBasket;
    uint8_t TradingCapacity;
    uint8_t TradeAllocStatus;
    uint8_t ProductComplex;
    uint8_t TradePublishIndicator;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char Account[LEN_ACCOUNT];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char Pad4[LEN_PAD4];
} BasketSideAllocExtBCGrpSeqT;

// Structure: BasketSideAllocExtGrp
typedef struct
{
    int64_t AllocQty;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    uint32_t PackageID;
    int32_t SideMarketSegmentID;
    uint32_t AllocID;
    uint8_t Side;
    char PositionEffect[LEN_POSITION_EFFECT];
    uint8_t TradingCapacity;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t ExecutingTraderQualifier;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t OrderAttributeRiskReduction;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char Account[LEN_ACCOUNT];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
} BasketSideAllocExtGrpSeqT;

// Structure: BasketSideAllocGrp
typedef struct
{
    int64_t AllocQty;
    uint32_t IndividualAllocID;
    uint16_t PartySubIDType;
    uint8_t Side;
    uint8_t InstrmtMatchSideID;
    uint8_t TradeAllocStatus;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char Pad4[LEN_PAD4];
} BasketSideAllocGrpSeqT;

// Structure: CrossRequestAckSideGrp
typedef struct
{
    uint64_t OrderID;
    uint8_t InputSource;
    uint8_t Side;
    char Pad6[LEN_PAD6];
} CrossRequestAckSideGrpSeqT;

// Structure: CrossRequestSideGrp
typedef struct
{
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int64_t MaximumPrice;
    uint8_t InputSource;
    uint8_t Side;
    uint8_t TradingCapacity;
    uint8_t ExecutingTraderQualifier;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    char PositionEffect[LEN_POSITION_EFFECT];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char Account[LEN_ACCOUNT];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char SideComplianceText[LEN_SIDE_COMPLIANCE_TEXT];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char Pad7[LEN_PAD7];
} CrossRequestSideGrpSeqT;

// Structure: EnrichmentRulesGrp
typedef struct
{
    uint16_t EnrichmentRuleID;
    uint8_t PartyIDOriginationMarket;
    char Account[LEN_ACCOUNT];
    char PositionEffect[LEN_POSITION_EFFECT];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char Pad1[LEN_PAD1];
} EnrichmentRulesGrpSeqT;

// Structure: FillsGrp
typedef struct
{
    int64_t FillPx;
    int64_t FillQty;
    uint32_t FillMatchID;
    int32_t FillExecID;
    uint8_t FillLiquidityInd;
    char Pad7[LEN_PAD7];
} FillsGrpSeqT;

// Structure: InstrmntLegExecGrp
typedef struct
{
    int64_t LegSecurityID;
    int64_t LegLastPx;
    int64_t LegLastQty;
    int32_t LegExecID;
    uint8_t LegSide;
    uint8_t FillRefID;
    char Pad2[LEN_PAD2];
} InstrmntLegExecGrpSeqT;

// Structure: InstrmtLegGrp
typedef struct
{
    int64_t LegSecurityID;
    int64_t LegPrice;
    int32_t LegSymbol;
    uint32_t LegRatioQty;
    uint8_t LegSide;
    uint8_t LegSecurityType;
    char Pad6[LEN_PAD6];
} InstrmtLegGrpSeqT;

// Structure: InstrmtMatchSideGrp
typedef struct
{
    int64_t SecurityID;
    int64_t LastPx;
    uint64_t TransBkdTime;
    int64_t RelatedClosePrice;
    uint32_t PackageID;
    int32_t SideMarketSegmentID;
    uint32_t NegotiationID;
    uint32_t SRQSRelatedTradeID;
    uint16_t SideTrdSubTyp;
    uint8_t ProductComplex;
    uint8_t TradePublishIndicator;
    uint8_t InstrmtMatchSideID;
    uint8_t EffectOnBasket;
    char TradeReportText[LEN_TRADE_REPORT_TEXT];
    char Pad6[LEN_PAD6];
} InstrmtMatchSideGrpSeqT;

// Structure: InstrumentAttributeGrp
typedef struct
{
    uint8_t InstrAttribType;
    char InstrAttribValue[LEN_INSTR_ATTRIB_VALUE];
    char Pad7[LEN_PAD7];
} InstrumentAttributeGrpSeqT;

// Structure: InstrumentEventGrp
typedef struct
{
    uint32_t EventDate;
    uint8_t EventType;
    char Pad3[LEN_PAD3];
} InstrumentEventGrpSeqT;

// Structure: LegOrdGrp
typedef struct
{
    char LegAccount[LEN_LEG_ACCOUNT];
    char LegPositionEffect[LEN_LEG_POSITION_EFFECT];
    char Pad5[LEN_PAD5];
} LegOrdGrpSeqT;

// Structure: MMParameterGrp
typedef struct
{
    int64_t ExposureDuration;
    int64_t CumQty;
    int64_t Delta;
    int64_t Vega;
    int32_t PctCount;
    uint32_t TargetPartyIDSessionID;
} MMParameterGrpSeqT;

// Structure: MessageHeaderIn
typedef struct
{
    uint32_t BodyLen;
    uint16_t TemplateID;
    char NetworkMsgID[LEN_NETWORK_MSGID];
    char Pad2[LEN_PAD2];
} MessageHeaderInCompT;

// Structure: MessageHeaderOut
typedef struct
{
    uint32_t BodyLen;
    uint16_t TemplateID;
    char Pad2[LEN_PAD2];
} MessageHeaderOutCompT;

// Structure: NRBCHeader
typedef struct
{
    uint64_t SendingTime;
    uint32_t ApplSubID;
    uint8_t ApplID;
    uint8_t LastFragment;
    char Pad2[LEN_PAD2];
} NRBCHeaderCompT;

// Structure: NRResponseHeaderME
typedef struct
{
    uint64_t RequestTime;
    uint64_t TrdRegTSTimeIn;
    uint64_t TrdRegTSTimeOut;
    uint64_t ResponseIn;
    uint64_t SendingTime;
    uint32_t MsgSeqNum;
    uint8_t LastFragment;
    char Pad3[LEN_PAD3];
} NRResponseHeaderMECompT;

// Structure: NotAffectedOrdersGrp
typedef struct
{
    uint64_t NotAffectedOrderID;
    uint64_t NotAffOrigClOrdID;
} NotAffectedOrdersGrpSeqT;

// Structure: NotAffectedSecuritiesGrp
typedef struct
{
    uint64_t NotAffectedSecurityID;
} NotAffectedSecuritiesGrpSeqT;

// Structure: NotifHeader
typedef struct
{
    uint64_t SendingTime;
} NotifHeaderCompT;

// Structure: OrderBookItemGrp
typedef struct
{
    int64_t SecurityID;
    int64_t BestBidPx;
    int64_t BestBidSize;
    int64_t BestOfferPx;
    int64_t BestOfferSize;
    uint8_t MDBookType;
    uint8_t MDSubBookType;
    char Pad6[LEN_PAD6];
} OrderBookItemGrpSeqT;

// Structure: PartyDetailsGrp
typedef struct
{
    uint32_t PartyDetailIDExecutingTrader;
    char PartyDetailExecutingTrader[LEN_PARTY_DETAIL_EXECUTING_TRADER];
    uint8_t PartyDetailRoleQualifier;
    uint8_t PartyDetailStatus;
    char PartyDetailDeskID[LEN_PARTY_DETAIL_DESKID];
    char Pad1[LEN_PAD1];
} PartyDetailsGrpSeqT;

// Structure: QuotReqLegsGrp
typedef struct
{
    int64_t LegSecurityID;
    uint32_t LegRatioQty;
    int32_t LegSymbol;
    uint8_t LegSecurityType;
    uint8_t LegSide;
    char Pad6[LEN_PAD6];
} QuotReqLegsGrpSeqT;

// Structure: QuoteEntryAckGrp
typedef struct
{
    int64_t SecurityID;
    int64_t CxlSize;
    uint32_t QuoteEntryRejectReason;
    uint8_t QuoteEntryStatus;
    uint8_t Side;
    char Pad2[LEN_PAD2];
} QuoteEntryAckGrpSeqT;

// Structure: QuoteEntryGrp
typedef struct
{
    int64_t SecurityID;
    int64_t BidPx;
    int64_t BidSize;
    int64_t OfferPx;
    int64_t OfferSize;
} QuoteEntryGrpSeqT;

// Structure: QuoteEventGrp
typedef struct
{
    int64_t SecurityID;
    int64_t QuoteEventPx;
    int64_t QuoteEventQty;
    uint64_t QuoteMsgID;
    uint32_t QuoteEventMatchID;
    int32_t QuoteEventExecID;
    uint8_t QuoteEventType;
    uint8_t QuoteEventSide;
    uint8_t QuoteEventLiquidityInd;
    uint8_t QuoteEventReason;
    char Pad4[LEN_PAD4];
} QuoteEventGrpSeqT;

// Structure: QuoteLegExecGrp
typedef struct
{
    int64_t LegSecurityID;
    int64_t LegLastPx;
    int64_t LegLastQty;
    int32_t LegExecID;
    uint8_t LegSide;
    uint8_t NoQuoteEventsIndex;
    char Pad2[LEN_PAD2];
} QuoteLegExecGrpSeqT;

// Structure: RBCHeader
typedef struct
{
    uint64_t SendingTime;
    uint64_t ApplSeqNum;
    uint32_t ApplSubID;
    uint16_t PartitionID;
    uint8_t ApplResendFlag;
    uint8_t ApplID;
    uint8_t LastFragment;
    char Pad7[LEN_PAD7];
} RBCHeaderCompT;

// Structure: RBCHeaderME
typedef struct
{
    uint64_t TrdRegTSTimeOut;
    uint64_t NotificationIn;
    uint64_t SendingTime;
    uint32_t ApplSubID;
    uint16_t PartitionID;
    char ApplMsgID[LEN_APPL_MSGID];
    uint8_t ApplID;
    uint8_t ApplResendFlag;
    uint8_t LastFragment;
    char Pad7[LEN_PAD7];
} RBCHeaderMECompT;

// Structure: RequestHeader
typedef struct
{
    uint32_t MsgSeqNum;
    uint32_t SenderSubID;
} RequestHeaderCompT;

// Structure: ResponseHeader
typedef struct
{
    uint64_t RequestTime;
    uint64_t SendingTime;
    uint32_t MsgSeqNum;
    char Pad4[LEN_PAD4];
} ResponseHeaderCompT;

// Structure: ResponseHeaderME
typedef struct
{
    uint64_t RequestTime;
    uint64_t TrdRegTSTimeIn;
    uint64_t TrdRegTSTimeOut;
    uint64_t ResponseIn;
    uint64_t SendingTime;
    uint32_t MsgSeqNum;
    uint16_t PartitionID;
    uint8_t ApplID;
    char ApplMsgID[LEN_APPL_MSGID];
    uint8_t LastFragment;
} ResponseHeaderMECompT;

// Structure: RiskLimitQtyGrp
typedef struct
{
    int64_t RiskLimitQty;
    uint8_t RiskLimitType;
    char Pad7[LEN_PAD7];
} RiskLimitQtyGrpSeqT;

// Structure: RiskLimitsRptGrp
typedef struct
{
    int64_t RiskLimitQty;
    int64_t RiskLimitOpenQty;
    int64_t RiskLimitNetPositionQty;
    uint8_t RiskLimitType;
    uint8_t RiskLimitRequestingPartyRole;
    uint8_t RiskLimitViolationIndicator;
    char RiskLimitGroup[LEN_RISK_LIMIT_GROUP];
    char Pad2[LEN_PAD2];
} RiskLimitsRptGrpSeqT;

// Structure: SRQSRelatedTradeIDGrp
typedef struct
{
    uint32_t SRQSRelatedTradeID;
    char Pad4[LEN_PAD4];
} SRQSRelatedTradeIDGrpSeqT;

// Structure: SessionsGrp
typedef struct
{
    uint32_t PartyIDSessionID;
    uint8_t SessionMode;
    uint8_t SessionSubMode;
    char Pad2[LEN_PAD2];
} SessionsGrpSeqT;

// Structure: SideAllocExtGrp
typedef struct
{
    int64_t AllocQty;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    uint32_t IndividualAllocID;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    uint8_t Side;
    uint8_t TradeAllocStatus;
    uint8_t TradingCapacity;
    char PositionEffect[LEN_POSITION_EFFECT];
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t ExecutingTraderQualifier;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t OrderAttributeRiskReduction;
    char Account[LEN_ACCOUNT];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char Pad7[LEN_PAD7];
} SideAllocExtGrpSeqT;

// Structure: SideAllocGrp
typedef struct
{
    int64_t AllocQty;
    uint32_t IndividualAllocID;
    uint8_t Side;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
} SideAllocGrpSeqT;

// Structure: SideAllocGrpBC
typedef struct
{
    int64_t AllocQty;
    uint32_t IndividualAllocID;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    uint8_t Side;
    uint8_t TradeAllocStatus;
    char Pad7[LEN_PAD7];
} SideAllocGrpBCSeqT;

// Structure: SideCrossLegGrp
typedef struct
{
    uint8_t LegInputSource;
    char LegPositionEffect[LEN_LEG_POSITION_EFFECT];
    char LegAccount[LEN_LEG_ACCOUNT];
    char Pad4[LEN_PAD4];
} SideCrossLegGrpSeqT;

// Structure: TargetParties
typedef struct
{
    uint8_t SideDisclosureInstruction;
    uint8_t PriceDisclosureInstruction;
    uint8_t LeavesQtyDisclosureInstruction;
    uint8_t LastPxDisclosureInstruction;
    uint8_t LastQtyDisclosureInstruction;
    uint8_t FreeText5DisclosureInstruction;
    uint8_t QuoteInstruction;
    char TargetPartyExecutingFirm[LEN_TARGET_PARTY_EXECUTING_FIRM];
    char TargetPartyExecutingTrader[LEN_TARGET_PARTY_EXECUTING_TRADER];
    char Pad6[LEN_PAD6];
} TargetPartiesSeqT;

// Structure: TrdInstrmntLegGrp
typedef struct
{
    int64_t LegSecurityID;
    int64_t LegPrice;
    int64_t LegQty;
} TrdInstrmntLegGrpSeqT;

// Structure: UnderlyingStipGrp
typedef struct
{
    char UnderlyingStipValue[LEN_UNDERLYING_STIP_VALUE];
    char UnderlyingStipType[LEN_UNDERLYING_STIP_TYPE];
    char Pad1[LEN_PAD1];
} UnderlyingStipGrpSeqT;

/*
 * Structure defines for messages
 */

// Message:	    AddComplexInstrumentRequest
// TemplateID:  10301
// Alias:       Create Strategy
// FIX MsgType: SecurityDefinitionRequest = "c"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int32_t MarketSegmentID;
    int32_t SecuritySubType;
    uint8_t ProductComplex;
    uint8_t NoLegs;
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char Pad2[LEN_PAD2];
    InstrmtLegGrpSeqT InstrmtLegGrp[MAX_ADD_COMPLEX_INSTRUMENT_REQUEST_INSTRMT_LEG_GRP];
} AddComplexInstrumentRequestT;

// Message:	    AddComplexInstrumentResponse
// TemplateID:  10302
// Alias:       Create Strategy Response
// FIX MsgType: SecurityDefinition = "d"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    int64_t LowLimitPrice;
    int64_t HighLimitPrice;
    int64_t SecurityID;
    uint64_t LastUpdateTime;
    uint64_t SecurityResponseID;
    int32_t MarketSegmentID;
    int32_t NumberOfSecurities;
    int32_t SecuritySubType;
    uint8_t MultilegModel;
    uint8_t ImpliedMarketIndicator;
    uint8_t ProductComplex;
    uint8_t NoLegs;
    InstrmtLegGrpSeqT InstrmtLegGrp[MAX_ADD_COMPLEX_INSTRUMENT_RESPONSE_INSTRMT_LEG_GRP];
} AddComplexInstrumentResponseT;

// Message:	    AddFlexibleInstrumentRequest
// TemplateID:  10309
// Alias:       Create Flexible Instrument Request
// FIX MsgType: SecurityDefinitionRequest = "c"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t StrikePrice;
    int32_t MarketSegmentID;
    uint32_t MaturityDate;
    char SettlMethod[LEN_SETTL_METHOD];
    uint8_t OptAttribute;
    uint8_t PutOrCall;
    uint8_t ExerciseStyle;
    char ComplianceText[LEN_COMPLIANCE_TEXT];
} AddFlexibleInstrumentRequestT;

// Message:	    AddFlexibleInstrumentResponse
// TemplateID:  10310
// Alias:       Create Flexible Instrument Response
// FIX MsgType: SecurityDefinition = "d"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t SecurityResponseID;
    int64_t SecurityID;
    int64_t StrikePrice;
    int32_t MarketSegmentID;
    uint32_t MaturityDate;
    uint8_t ProductComplex;
    char SettlMethod[LEN_SETTL_METHOD];
    uint8_t OptAttribute;
    uint8_t PutOrCall;
    uint8_t ExerciseStyle;
    char Symbol[LEN_SYMBOL];
    char Pad7[LEN_PAD7];
} AddFlexibleInstrumentResponseT;

// Message:	    AmendBasketTradeRequest
// TemplateID:  10629
// Alias:       Amend Basket Trade Request
// FIX MsgType: TradeMatchReport = "DC"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t BasketTrdMatchID;
    uint32_t BasketExecID;
    int32_t MarketSegmentID;
    uint32_t MaturityMonthYear;
    uint32_t BasketProfileID;
    uint16_t TrdType;
    uint8_t TradeReportType;
    uint8_t BasketTradeReportType;
    uint8_t NoBasketRootPartyGrps;
    uint8_t NoInstrmtMatchSides;
    uint8_t NoBasketSideAlloc;
    char BasketTradeReportText[LEN_BASKET_TRADE_REPORT_TEXT];
    char TradeReportID[LEN_TRADE_REPORTID];
    char Pad1[LEN_PAD1];
    BasketRootPartyGrpSeqT BasketRootPartyGrp[MAX_AMEND_BASKET_TRADE_REQUEST_BASKET_ROOT_PARTY_GRP];
    InstrmtMatchSideGrpSeqT InstrmtMatchSideGrp[MAX_AMEND_BASKET_TRADE_REQUEST_INSTRMT_MATCH_SIDE_GRP];
    BasketSideAllocGrpSeqT BasketSideAllocGrp[MAX_AMEND_BASKET_TRADE_REQUEST_BASKET_SIDE_ALLOC_GRP];
} AmendBasketTradeRequestT;

// Message:	    ApproveBasketTradeRequest
// TemplateID:  10623
// Alias:       Approve Basket Trade Request
// FIX MsgType: TradeMatchReport = "DC"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t BasketTrdMatchID;
    uint32_t BasketExecID;
    int32_t MarketSegmentID;
    uint32_t BasketProfileID;
    uint16_t RootPartySubIDType;
    uint16_t TrdType;
    uint8_t TradeReportType;
    uint8_t NoBasketSideAlloc;
    char BasketTradeReportText[LEN_BASKET_TRADE_REPORT_TEXT];
    char TradeReportID[LEN_TRADE_REPORTID];
    char BasketSideTradeReportID[LEN_BASKET_SIDE_TRADE_REPORTID];
    char Pad2[LEN_PAD2];
    BasketSideAllocExtGrpSeqT BasketSideAllocExtGrp[MAX_APPROVE_BASKET_TRADE_REQUEST_BASKET_SIDE_ALLOC_EXT_GRP];
} ApproveBasketTradeRequestT;

// Message:	    ApproveTESTradeRequest
// TemplateID:  10603
// Alias:       Approve TES Trade Request
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int64_t AllocQty;
    uint32_t PackageID;
    uint32_t AllocID;
    uint32_t TESExecID;
    int32_t MarketSegmentID;
    int32_t RelatedMarketSegmentID;
    uint16_t TrdType;
    uint8_t TradingCapacity;
    uint8_t TradeReportType;
    uint8_t Side;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    uint8_t OrderAttributeRiskReduction;
    char TradeReportID[LEN_TRADE_REPORTID];
    char PositionEffect[LEN_POSITION_EFFECT];
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char Account[LEN_ACCOUNT];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char Pad1[LEN_PAD1];
} ApproveTESTradeRequestT;

// Message:	    BasketApproveBroadcast
// TemplateID:  10627
// Alias:       Approve Basket Broadcast
// FIX MsgType: TradeMatchReport = "DC"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t BasketTrdMatchID;
    uint64_t TransactTime;
    uint32_t BasketExecID;
    int32_t MarketSegmentID;
    uint32_t BasketProfileID;
    uint16_t TrdType;
    uint8_t TradeReportType;
    uint8_t BasketTradeReportType;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    uint8_t NoBasketSideAlloc;
    uint8_t NoBasketRootPartyGrps;
    uint8_t PartyIDEnteringFirm;
    char PartyEnteringTrader[LEN_PARTY_ENTERING_TRADER];
    char BasketTradeReportText[LEN_BASKET_TRADE_REPORT_TEXT];
    char TradeReportID[LEN_TRADE_REPORTID];
    char Pad6[LEN_PAD6];
    BasketRootPartyGrpSeqT BasketRootPartyGrp[MAX_BASKET_APPROVE_BROADCAST_BASKET_ROOT_PARTY_GRP];
    BasketSideAllocExtBCGrpSeqT BasketSideAllocExtBCGrp[MAX_BASKET_APPROVE_BROADCAST_BASKET_SIDE_ALLOC_EXT_BC_GRP];
} BasketApproveBroadcastT;

// Message:	    BasketBroadcast
// TemplateID:  10625
// Alias:       Basket Trading Broadcast
// FIX MsgType: TradeMatchReport = "DC"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t BasketTrdMatchID;
    uint64_t TransactTime;
    uint32_t BasketExecID;
    int32_t MarketSegmentID;
    uint32_t MaturityMonthYear;
    uint32_t BasketProfileID;
    uint16_t TrdType;
    uint8_t TradeReportType;
    uint8_t BasketTradeReportType;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    uint8_t NoBasketRootPartyGrpsBC;
    uint8_t NoInstrmtMatchSides;
    uint8_t NoBasketSideAlloc;
    char BasketTradeReportText[LEN_BASKET_TRADE_REPORT_TEXT];
    char TradeReportID[LEN_TRADE_REPORTID];
    BasketRootPartyGrpSeqT BasketRootPartyGrp[MAX_BASKET_BROADCAST_BASKET_ROOT_PARTY_GRP];
    InstrmtMatchSideGrpSeqT InstrmtMatchSideGrp[MAX_BASKET_BROADCAST_INSTRMT_MATCH_SIDE_GRP];
    BasketSideAllocGrpSeqT BasketSideAllocGrp[MAX_BASKET_BROADCAST_BASKET_SIDE_ALLOC_GRP];
} BasketBroadcastT;

// Message:	    BasketDeleteBroadcast
// TemplateID:  10626
// Alias:       Delete Basket Broadcast
// FIX MsgType: TradeMatchReport = "DC"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t BasketTrdMatchID;
    uint64_t TransactTime;
    int32_t MarketSegmentID;
    uint32_t BasketExecID;
    uint32_t BasketProfileID;
    uint16_t TrdType;
    uint8_t DeleteReason;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    char TradeReportID[LEN_TRADE_REPORTID];
    char Pad4[LEN_PAD4];
} BasketDeleteBroadcastT;

// Message:	    BasketExecutionBroadcast
// TemplateID:  10628
// Alias:       Basket Execution Broadcast
// FIX MsgType: TradeMatchReport = "DC"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t BasketTrdMatchID;
    uint64_t TransactTime;
    uint32_t BasketExecID;
    int32_t MarketSegmentID;
    uint32_t BasketProfileID;
    uint16_t TrdType;
    uint8_t TradeReportType;
    uint8_t NoInstrmtMatchSides;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    char BasketSideTradeReportID[LEN_BASKET_SIDE_TRADE_REPORTID];
    char Pad3[LEN_PAD3];
    BasketExecGrpSeqT BasketExecGrp[MAX_BASKET_EXECUTION_BROADCAST_BASKET_EXEC_GRP];
} BasketExecutionBroadcastT;

// Message:	    BasketResponse
// TemplateID:  10624
// Alias:       Basket Trading Response
// FIX MsgType: TradeMatchReportAck = "DD"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    uint32_t BasketExecID;
    char TradeReportID[LEN_TRADE_REPORTID];
} BasketResponseT;

// Message:	    BroadcastErrorNotification
// TemplateID:  10032
// Alias:       Gap Fill
// FIX MsgType: ApplicationMessageReport = "BY"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NotifHeaderCompT NotifHeader;
    uint32_t ApplIDStatus;
    uint32_t RefApplSubID;
    uint16_t VarTextLen;
    uint8_t RefApplID;
    uint8_t SessionStatus;
    char Pad4[LEN_PAD4];
    char VarText[LEN_VAR_TEXT];
} BroadcastErrorNotificationT;

// Message:	    CLIPDeletionNotification
// TemplateID:  10134
// Alias:       CLIP Deletion Notification
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    uint64_t OrderID;
    uint64_t ExecID;
    int64_t SecurityID;
    int64_t CxlQty;
    int32_t MarketSegmentID;
    uint16_t ExecRestatementReason;
    uint8_t ProductComplex;
    uint8_t Side;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    char Pad6[LEN_PAD6];
} CLIPDeletionNotificationT;

// Message:	    CLIPExecutionNotification
// TemplateID:  10135
// Alias:       CLIP Execution Notification
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    uint64_t OrderID;
    uint64_t ExecID;
    int64_t SecurityID;
    int64_t CxlQty;
    int64_t LeavesQty;
    int64_t CumQty;
    int32_t MarketSegmentID;
    uint16_t ExecRestatementReason;
    uint16_t NoLegExecs;
    uint8_t ProductComplex;
    uint8_t Side;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint8_t MatchType;
    uint8_t NoFills;
    char Pad2[LEN_PAD2];
    FillsGrpSeqT FillsGrp[MAX_CLIP_EXECUTION_NOTIFICATION_FILLS_GRP];
    InstrmntLegExecGrpSeqT InstrmntLegExecGrp[MAX_CLIP_EXECUTION_NOTIFICATION_INSTRMNT_LEG_EXEC_GRP];
} CLIPExecutionNotificationT;

// Message:	    CLIPResponse
// TemplateID:  10133
// Alias:       Response to a CLIP Request
// FIX MsgType: CrossRequestAck = "DT"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t ExecID;
    int64_t SecurityID;
    int32_t MarketSegmentID;
    uint8_t NoSides;
    char Pad3[LEN_PAD3];
    int32_t CrossRequestID;
    char Pad4[LEN_PAD4];
    CrossRequestAckSideGrpSeqT CrossRequestAckSideGrp[MAX_CLIP_RESPONSE_CROSS_REQUEST_ACK_SIDE_GRP];
} CLIPResponseT;

// Message:	    CrossRequest
// TemplateID:  10118
// Alias:       Cross Request
// FIX MsgType: CrossRequest = "DS"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t SecurityID;
    int64_t OrderQty;
    int32_t MarketSegmentID;
    char ComplianceText[LEN_COMPLIANCE_TEXT];
} CrossRequestT;

// Message:	    CrossRequestResponse
// TemplateID:  10119
// Alias:       Cross Request Response
// FIX MsgType: RequestAcknowledge = "U1"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t ExecID;
} CrossRequestResponseT;

// Message:	    DeleteAllOrderBroadcast
// TemplateID:  10122
// Alias:       Order Mass Cancellation Notification
// FIX MsgType: OrderMassActionReport = "BZ"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    uint64_t MassActionReportID;
    int64_t SecurityID;
    int64_t Price;
    int32_t MarketSegmentID;
    uint32_t TargetPartyIDSessionID;
    uint32_t TargetPartyIDExecutingTrader;
    uint32_t PartyIDEnteringTrader;
    uint16_t NoNotAffectedOrders;
    uint16_t NoAffectedOrderRequests;
    uint8_t PartyIDEnteringFirm;
    uint8_t MassActionReason;
    uint8_t ExecInst;
    uint8_t Side;
    NotAffectedOrdersGrpSeqT NotAffectedOrdersGrp[MAX_DELETE_ALL_ORDER_BROADCAST_NOT_AFFECTED_ORDERS_GRP];
    AffectedOrderRequestsGrpSeqT AffectedOrderRequestsGrp[MAX_DELETE_ALL_ORDER_BROADCAST_AFFECTED_ORDER_REQUESTS_GRP];
} DeleteAllOrderBroadcastT;

// Message:	    DeleteAllOrderNRResponse
// TemplateID:  10124
// Alias:       Order Mass Cancellation Response No Hits
// FIX MsgType: OrderMassActionReport = "BZ"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t MassActionReportID;
} DeleteAllOrderNRResponseT;

// Message:	    DeleteAllOrderQuoteEventBroadcast
// TemplateID:  10308
// Alias:       Mass Cancellation Event
// FIX MsgType: OrderMassActionReport = "BZ"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    uint64_t MassActionReportID;
    int64_t SecurityID;
    int32_t MarketSegmentID;
    uint8_t MassActionReason;
    uint8_t ExecInst;
    char Pad2[LEN_PAD2];
} DeleteAllOrderQuoteEventBroadcastT;

// Message:	    DeleteAllOrderRequest
// TemplateID:  10120
// Alias:       Order Mass Cancellation Request
// FIX MsgType: OrderMassActionRequest = "CA"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t SecurityID;
    int64_t Price;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t TargetPartyIDSessionID;
    uint32_t TargetPartyIDExecutingTrader;
    uint8_t Side;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
} DeleteAllOrderRequestT;

// Message:	    DeleteAllOrderResponse
// TemplateID:  10121
// Alias:       Order Mass Cancellation Response
// FIX MsgType: OrderMassActionReport = "BZ"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderMECompT ResponseHeaderME;
    uint64_t MassActionReportID;
    uint16_t NoNotAffectedOrders;
    uint16_t NoAffectedOrderRequests;
    char Pad4[LEN_PAD4];
    NotAffectedOrdersGrpSeqT NotAffectedOrdersGrp[MAX_DELETE_ALL_ORDER_RESPONSE_NOT_AFFECTED_ORDERS_GRP];
    AffectedOrderRequestsGrpSeqT AffectedOrderRequestsGrp[MAX_DELETE_ALL_ORDER_RESPONSE_AFFECTED_ORDER_REQUESTS_GRP];
} DeleteAllOrderResponseT;

// Message:	    DeleteAllQuoteBroadcast
// TemplateID:  10410
// Alias:       Quote Mass Cancellation Notification
// FIX MsgType: OrderMassActionReport = "BZ"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    uint64_t MassActionReportID;
    int64_t SecurityID;
    int32_t MarketSegmentID;
    uint32_t TargetPartyIDSessionID;
    uint32_t PartyIDEnteringTrader;
    uint32_t TargetPartyIDExecutingTrader;
    uint16_t NoNotAffectedSecurities;
    uint8_t MassActionReason;
    uint8_t PartyIDEnteringFirm;
    char TargetPartyIDDeskID[LEN_TARGET_PARTY_ID_DESKID];
    char Pad1[LEN_PAD1];
    NotAffectedSecuritiesGrpSeqT NotAffectedSecuritiesGrp[MAX_DELETE_ALL_QUOTE_BROADCAST_NOT_AFFECTED_SECURITIES_GRP];
} DeleteAllQuoteBroadcastT;

// Message:	    DeleteAllQuoteRequest
// TemplateID:  10408
// Alias:       Quote Mass Cancellation Request
// FIX MsgType: OrderMassActionRequest = "CA"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t TargetPartyIDSessionID;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char Pad6[LEN_PAD6];
} DeleteAllQuoteRequestT;

// Message:	    DeleteAllQuoteResponse
// TemplateID:  10409
// Alias:       Quote Mass Cancellation Response
// FIX MsgType: OrderMassActionReport = "BZ"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t MassActionReportID;
    uint16_t NoNotAffectedSecurities;
    char Pad6[LEN_PAD6];
    NotAffectedSecuritiesGrpSeqT NotAffectedSecuritiesGrp[MAX_DELETE_ALL_QUOTE_RESPONSE_NOT_AFFECTED_SECURITIES_GRP];
} DeleteAllQuoteResponseT;

// Message:	    DeleteBasketTradeRequest
// TemplateID:  10622
// Alias:       Delete Basket Trade Request
// FIX MsgType: TradeMatchReport = "DC"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t BasketTrdMatchID;
    uint32_t BasketExecID;
    int32_t MarketSegmentID;
    uint32_t BasketProfileID;
    uint16_t TrdType;
    uint8_t TradeReportType;
    char TradeReportID[LEN_TRADE_REPORTID];
    char Pad5[LEN_PAD5];
} DeleteBasketTradeRequestT;

// Message:	    DeleteCLIPRequest
// TemplateID:  10132
// Alias:       CLIP Delete Request
// FIX MsgType: CrossRequest = "DS"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t OrderID;
    int64_t SecurityID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint8_t ExecutingTraderQualifier;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    char Pad2[LEN_PAD2];
    int32_t CrossRequestID;
    char Pad4[LEN_PAD4];
} DeleteCLIPRequestT;

// Message:	    DeleteOrderBroadcast
// TemplateID:  10112
// Alias:       Cancel Order Notification
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    uint64_t ExecID;
    int64_t CumQty;
    int64_t CxlQty;
    int32_t MarketSegmentID;
    uint32_t PartyIDEnteringTrader;
    uint32_t PartyIDSessionID;
    uint16_t ExecRestatementReason;
    uint8_t PartyIDEnteringFirm;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint8_t ProductComplex;
    uint8_t Side;
    char FIXClOrdID[LEN_FIX_CL_ORDID];
    char Pad1[LEN_PAD1];
} DeleteOrderBroadcastT;

// Message:	    DeleteOrderComplexRequest
// TemplateID:  10123
// Alias:       Cancel Order Multi Leg
// FIX MsgType: OrderCancelRequest = "F"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t TargetPartyIDSessionID;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char FIXClOrdID[LEN_FIX_CL_ORDID];
    char Pad1[LEN_PAD1];
} DeleteOrderComplexRequestT;

// Message:	    DeleteOrderNRResponse
// TemplateID:  10111
// Alias:       Cancel Order Response (Lean Order)
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    uint64_t ExecID;
    int64_t CumQty;
    int64_t CxlQty;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint16_t ExecRestatementReason;
    uint8_t ProductComplex;
    uint8_t TransactionDelayIndicator;
    char Pad2[LEN_PAD2];
} DeleteOrderNRResponseT;

// Message:	    DeleteOrderResponse
// TemplateID:  10110
// Alias:       Cancel Order Response (Standard Order)
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderMECompT ResponseHeaderME;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    uint64_t ExecID;
    int64_t CumQty;
    int64_t CxlQty;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint16_t ExecRestatementReason;
    uint8_t ProductComplex;
    uint8_t TransactionDelayIndicator;
    char Pad2[LEN_PAD2];
} DeleteOrderResponseT;

// Message:	    DeleteOrderSingleRequest
// TemplateID:  10109
// Alias:       Cancel Order Single
// FIX MsgType: OrderCancelRequest = "F"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t SimpleSecurityID;
    uint32_t TargetPartyIDSessionID;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char FIXClOrdID[LEN_FIX_CL_ORDID];
    char Pad5[LEN_PAD5];
} DeleteOrderSingleRequestT;

// Message:	    DeleteTESTradeRequest
// TemplateID:  10602
// Alias:       Delete TES Trade Request
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint32_t PackageID;
    int32_t MarketSegmentID;
    uint32_t TESExecID;
    int32_t RelatedMarketSegmentID;
    uint16_t TrdType;
    uint8_t TradeReportType;
    char TradeReportID[LEN_TRADE_REPORTID];
    char Pad1[LEN_PAD1];
} DeleteTESTradeRequestT;

// Message:	    EnterBasketTradeRequest
// TemplateID:  10620
// Alias:       Enter Basket Trade Request
// FIX MsgType: TradeMatchReport = "DC"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int32_t MarketSegmentID;
    uint32_t MaturityMonthYear;
    uint32_t BasketProfileID;
    uint16_t TrdType;
    uint8_t TradeReportType;
    uint8_t BasketTradeReportType;
    uint8_t NoBasketRootPartyGrps;
    uint8_t NoInstrmtMatchSides;
    uint8_t NoBasketSideAlloc;
    char BasketTradeReportText[LEN_BASKET_TRADE_REPORT_TEXT];
    char TradeReportID[LEN_TRADE_REPORTID];
    char Pad5[LEN_PAD5];
    BasketRootPartyGrpSeqT BasketRootPartyGrp[MAX_ENTER_BASKET_TRADE_REQUEST_BASKET_ROOT_PARTY_GRP];
    InstrmtMatchSideGrpSeqT InstrmtMatchSideGrp[MAX_ENTER_BASKET_TRADE_REQUEST_INSTRMT_MATCH_SIDE_GRP];
    BasketSideAllocGrpSeqT BasketSideAllocGrp[MAX_ENTER_BASKET_TRADE_REQUEST_BASKET_SIDE_ALLOC_GRP];
} EnterBasketTradeRequestT;

// Message:	    EnterCLIPRequest
// TemplateID:  10131
// Alias:       CLIP Enter Request
// FIX MsgType: CrossRequest = "DS"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t SecurityID;
    int64_t Price;
    int64_t OrderQty;
    int32_t MarketSegmentID;
    int32_t CrossID;
    int32_t CrossRequestID;
    uint8_t NoSides;
    uint8_t NoCrossLegs;
    uint8_t SideDisclosureInstruction;
    uint8_t PriceDisclosureInstruction;
    uint8_t OrderQtyDisclosureInstruction;
    char RootPartyContraFirm[LEN_ROOT_PARTY_CONTRA_FIRM];
    char RootPartyContraTrader[LEN_ROOT_PARTY_CONTRA_TRADER];
    char Pad4[LEN_PAD4];
    CrossRequestSideGrpSeqT CrossRequestSideGrp[MAX_ENTER_CLIP_REQUEST_CROSS_REQUEST_SIDE_GRP];
    SideCrossLegGrpSeqT SideCrossLegGrp[MAX_ENTER_CLIP_REQUEST_SIDE_CROSS_LEG_GRP];
} EnterCLIPRequestT;

// Message:	    EnterTESTradeRequest
// TemplateID:  10600
// Alias:       Enter TES Trade Request
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t SecurityID;
    int64_t LastPx;
    uint64_t TransBkdTime;
    int64_t UnderlyingPx;
    int64_t RelatedClosePrice;
    int64_t RelatedTradeQuantity;
    int64_t UnderlyingQty;
    int32_t MarketSegmentID;
    uint32_t UnderlyingSettlementDate;
    uint32_t UnderlyingMaturityDate;
    uint32_t RelatedTradeID;
    int32_t RelatedMarketSegmentID;
    uint32_t NegotiationID;
    uint8_t NoSRQSRelatedTradeIDs;
    char Pad1[LEN_PAD1];
    uint16_t TrdType;
    uint8_t ProductComplex;
    uint8_t TradeReportType;
    uint8_t TradePublishIndicator;
    uint8_t NoSideAllocs;
    uint8_t NoEvents;
    uint8_t NoLegs;
    uint8_t NoInstrAttrib;
    uint8_t NoUnderlyingStips;
    uint8_t PartyIDSettlementLocation;
    uint8_t HedgeType;
    char TradeReportText[LEN_TRADE_REPORT_TEXT];
    char TradeReportID[LEN_TRADE_REPORTID];
    char UnderlyingSecurityID[LEN_UNDERLYING_SECURITYID];
    char UnderlyingSecurityDesc[LEN_UNDERLYING_SECURITY_DESC];
    char UnderlyingCurrency[LEN_UNDERLYING_CURRENCY];
    char UnderlyingIssuer[LEN_UNDERLYING_ISSUER];
    char Pad7[LEN_PAD7];
    SideAllocGrpSeqT SideAllocGrp[MAX_ENTER_TES_TRADE_REQUEST_SIDE_ALLOC_GRP];
    TrdInstrmntLegGrpSeqT TrdInstrmntLegGrp[MAX_ENTER_TES_TRADE_REQUEST_TRD_INSTRMNT_LEG_GRP];
    InstrumentEventGrpSeqT InstrumentEventGrp[MAX_ENTER_TES_TRADE_REQUEST_INSTRUMENT_EVENT_GRP];
    InstrumentAttributeGrpSeqT InstrumentAttributeGrp[MAX_ENTER_TES_TRADE_REQUEST_INSTRUMENT_ATTRIBUTE_GRP];
    UnderlyingStipGrpSeqT UnderlyingStipGrp[MAX_ENTER_TES_TRADE_REQUEST_UNDERLYING_STIP_GRP];
    SRQSRelatedTradeIDGrpSeqT SRQSRelatedTradeIDGrp[MAX_ENTER_TES_TRADE_REQUEST_SRQS_RELATED_TRADE_ID_GRP];
} EnterTESTradeRequestT;

// Message:	    ForcedLogoutNotification
// TemplateID:  10012
// Alias:       Session Logout Notification
// FIX MsgType: Logout = "5"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NotifHeaderCompT NotifHeader;
    uint16_t VarTextLen;
    char Pad6[LEN_PAD6];
    char VarText[LEN_VAR_TEXT];
} ForcedLogoutNotificationT;

// Message:	    ForcedUserLogoutNotification
// TemplateID:  10043
// Alias:       User Logout Notification
// FIX MsgType: Logout = "5"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NotifHeaderCompT NotifHeader;
    uint8_t UserStatus;
    char Pad3[LEN_PAD3];
    uint32_t Username;
    uint16_t VarTextLen;
    char Pad6[LEN_PAD6];
    char VarText[LEN_VAR_TEXT];
} ForcedUserLogoutNotificationT;

// Message:	    Heartbeat
// TemplateID:  10011
// Alias:       Heartbeat
// FIX MsgType: Heartbeat = "0"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
} HeartbeatT;

// Message:	    HeartbeatNotification
// TemplateID:  10023
// Alias:       Heartbeat Notification
// FIX MsgType: Heartbeat = "0"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NotifHeaderCompT NotifHeader;
} HeartbeatNotificationT;

// Message:	    InquireEnrichmentRuleIDListRequest
// TemplateID:  10040
// Alias:       Trade Enrichment List Inquire
// FIX MsgType: TradeEnrichmentListRequest = "U7"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    char LastEntityProcessed[LEN_LAST_ENTITY_PROCESSED];
} InquireEnrichmentRuleIDListRequestT;

// Message:	    InquireEnrichmentRuleIDListResponse
// TemplateID:  10041
// Alias:       Trade Enrichment List Inquire Response
// FIX MsgType: TradeEnrichmentListReport = "U9"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    char LastEntityProcessed[LEN_LAST_ENTITY_PROCESSED];
    uint16_t NoEnrichmentRules;
    char Pad6[LEN_PAD6];
    EnrichmentRulesGrpSeqT EnrichmentRulesGrp[MAX_INQUIRE_ENRICHMENT_RULE_ID_LIST_RESPONSE_ENRICHMENT_RULES_GRP];
} InquireEnrichmentRuleIDListResponseT;

// Message:	    InquireMMParameterRequest
// TemplateID:  10305
// Alias:       Inquire Market Maker Parameters
// FIX MsgType: MMParameterRequest = "U17"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int32_t MarketSegmentID;
    uint32_t TargetPartyIDSessionID;
} InquireMMParameterRequestT;

// Message:	    InquireMMParameterResponse
// TemplateID:  10306
// Alias:       Inquire Market Maker Parameters Response
// FIX MsgType: MMParameterResponse = "U18"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t MMParameterReportID;
    int32_t MarketSegmentID;
    uint8_t NoMMParameters;
    char Pad3[LEN_PAD3];
    MMParameterGrpSeqT MMParameterGrp[MAX_INQUIRE_MM_PARAMETER_RESPONSE_MM_PARAMETER_GRP];
} InquireMMParameterResponseT;

// Message:	    InquirePreTradeRiskLimitsRequest
// TemplateID:  10311
// Alias:       Inquire Pre-Trade Risk Limits Request
// FIX MsgType: PartyRiskLimitsRequest = "CL"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int32_t MarketSegmentID;
    uint8_t RiskLimitPlatform;
    char PartyExecutingUnit[LEN_PARTY_EXECUTING_UNIT];
    char RiskLimitGroup[LEN_RISK_LIMIT_GROUP];
    char Pad3[LEN_PAD3];
} InquirePreTradeRiskLimitsRequestT;

// Message:	    InquireSessionListRequest
// TemplateID:  10035
// Alias:       Session List Inquire
// FIX MsgType: SessionDetailsListRequest = "U5"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
} InquireSessionListRequestT;

// Message:	    InquireSessionListResponse
// TemplateID:  10036
// Alias:       Session List Inquire Response
// FIX MsgType: SessionDetailsListResponse = "U6"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    uint16_t NoSessions;
    char Pad6[LEN_PAD6];
    SessionsGrpSeqT SessionsGrp[MAX_INQUIRE_SESSION_LIST_RESPONSE_SESSIONS_GRP];
} InquireSessionListResponseT;

// Message:	    InquireUserRequest
// TemplateID:  10038
// Alias:       User List Inquire
// FIX MsgType: PartyDetailListRequest = "CF"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    char LastEntityProcessed[LEN_LAST_ENTITY_PROCESSED];
} InquireUserRequestT;

// Message:	    InquireUserResponse
// TemplateID:  10039
// Alias:       User List Inquire Response
// FIX MsgType: PartyDetailListReport = "CG"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    char LastEntityProcessed[LEN_LAST_ENTITY_PROCESSED];
    uint16_t NoPartyDetails;
    char Pad6[LEN_PAD6];
    PartyDetailsGrpSeqT PartyDetailsGrp[MAX_INQUIRE_USER_RESPONSE_PARTY_DETAILS_GRP];
} InquireUserResponseT;

// Message:	    LegalNotificationBroadcast
// TemplateID:  10037
// Alias:       Legal Notification
// FIX MsgType: UserNotification = "CB"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    uint16_t VarTextLen;
    uint8_t UserStatus;
    char Pad5[LEN_PAD5];
    char VarText[LEN_VAR_TEXT];
} LegalNotificationBroadcastT;

// Message:	    LogonRequest
// TemplateID:  10000
// Alias:       Session Logon
// FIX MsgType: Logon = "A"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint32_t HeartBtInt;
    uint32_t PartyIDSessionID;
    char DefaultCstmApplVerID[LEN_DEFAULT_CSTM_APPL_VERID];
    char Password[LEN_PASSWORD];
    char ApplUsageOrders[LEN_APPL_USAGE_ORDERS];
    char ApplUsageQuotes[LEN_APPL_USAGE_QUOTES];
    char OrderRoutingIndicator[LEN_ORDER_ROUTING_INDICATOR];
    char FIXEngineName[LEN_FIX_ENGINE_NAME];
    char FIXEngineVersion[LEN_FIX_ENGINE_VERSION];
    char FIXEngineVendor[LEN_FIX_ENGINE_VENDOR];
    char ApplicationSystemName[LEN_APPLICATION_SYSTEM_NAME];
    char ApplicationSystemVersion[LEN_APPLICATION_SYSTEM_VERSION];
    char ApplicationSystemVendor[LEN_APPLICATION_SYSTEM_VENDOR];
    char Pad3[LEN_PAD3];
} LogonRequestT;

// Message:	    LogonResponse
// TemplateID:  10001
// Alias:       Session Logon Response
// FIX MsgType: Logon = "A"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    int64_t ThrottleTimeInterval;
    uint32_t ThrottleNoMsgs;
    uint32_t ThrottleDisconnectLimit;
    uint32_t HeartBtInt;
    uint32_t SessionInstanceID;
    uint16_t MarketID;
    uint8_t TradSesMode;
    char DefaultCstmApplVerID[LEN_DEFAULT_CSTM_APPL_VERID];
    char DefaultCstmApplVerSubID[LEN_DEFAULT_CSTM_APPL_VER_SUBID];
    char Pad2[LEN_PAD2];
} LogonResponseT;

// Message:	    LogoutRequest
// TemplateID:  10002
// Alias:       Session Logout
// FIX MsgType: Logout = "5"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
} LogoutRequestT;

// Message:	    LogoutResponse
// TemplateID:  10003
// Alias:       Session Logout Response
// FIX MsgType: Logout = "5"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
} LogoutResponseT;

// Message:	    MMParameterDefinitionRequest
// TemplateID:  10303
// Alias:       Market Maker Parameter Definition
// FIX MsgType: MMParameterDefinitionRequest = "U14"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t ExposureDuration;
    int64_t CumQty;
    int64_t Delta;
    int64_t Vega;
    int32_t MarketSegmentID;
    uint32_t TargetPartyIDSessionID;
    int32_t PctCount;
    char Pad4[LEN_PAD4];
} MMParameterDefinitionRequestT;

// Message:	    MMParameterDefinitionResponse
// TemplateID:  10304
// Alias:       Market Maker Parameter Definition Response
// FIX MsgType: RequestAcknowledge = "U1"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t ExecID;
} MMParameterDefinitionResponseT;

// Message:	    MassQuoteRequest
// TemplateID:  10405
// Alias:       Mass Quote
// FIX MsgType: MassQuote = "i"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t QuoteID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t MatchInstCrossID;
    uint16_t EnrichmentRuleID;
    uint8_t PriceValidityCheckType;
    uint8_t ValueCheckTypeValue;
    uint8_t QuoteSizeType;
    uint8_t QuoteType;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t NoQuoteEntries;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char Pad6[LEN_PAD6];
    QuoteEntryGrpSeqT QuoteEntryGrp[MAX_MASS_QUOTE_REQUEST_QUOTE_ENTRY_GRP];
} MassQuoteRequestT;

// Message:	    MassQuoteResponse
// TemplateID:  10406
// Alias:       Mass Quote Response
// FIX MsgType: MassQuoteAcknowledgement = "b"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t QuoteID;
    uint64_t QuoteResponseID;
    int32_t MarketSegmentID;
    uint8_t NoQuoteSideEntries;
    char Pad3[LEN_PAD3];
    QuoteEntryAckGrpSeqT QuoteEntryAckGrp[MAX_MASS_QUOTE_RESPONSE_QUOTE_ENTRY_ACK_GRP];
} MassQuoteResponseT;

// Message:	    ModifyBasketTradeRequest
// TemplateID:  10621
// Alias:       Modify Basket Trade Request
// FIX MsgType: TradeMatchReport = "DC"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t BasketTrdMatchID;
    uint32_t BasketExecID;
    int32_t MarketSegmentID;
    uint32_t MaturityMonthYear;
    uint32_t BasketProfileID;
    uint16_t TrdType;
    uint8_t TradeReportType;
    uint8_t NoBasketRootPartyGrps;
    uint8_t NoInstrmtMatchSides;
    uint8_t NoBasketSideAlloc;
    char BasketTradeReportText[LEN_BASKET_TRADE_REPORT_TEXT];
    char TradeReportID[LEN_TRADE_REPORTID];
    char Pad2[LEN_PAD2];
    BasketRootPartyGrpSeqT BasketRootPartyGrp[MAX_MODIFY_BASKET_TRADE_REQUEST_BASKET_ROOT_PARTY_GRP];
    InstrmtMatchSideGrpSeqT InstrmtMatchSideGrp[MAX_MODIFY_BASKET_TRADE_REQUEST_INSTRMT_MATCH_SIDE_GRP];
    BasketSideAllocGrpSeqT BasketSideAllocGrp[MAX_MODIFY_BASKET_TRADE_REQUEST_BASKET_SIDE_ALLOC_GRP];
} ModifyBasketTradeRequestT;

// Message:	    ModifyOrderComplexRequest
// TemplateID:  10114
// Alias:       Replace Order Multi Leg
// FIX MsgType: MultilegOrderCancelReplace = "AC"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    int64_t Price;
    int64_t OrderQty;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t ExpireDate;
    uint32_t MatchInstCrossID;
    uint32_t TargetPartyIDSessionID;
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    uint8_t ApplSeqIndicator;
    uint8_t ProductComplex;
    uint8_t Side;
    uint8_t OrdType;
    uint8_t PriceValidityCheckType;
    uint8_t ValueCheckTypeValue;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t ExecInst;
    uint8_t TimeInForce;
    uint8_t TradingCapacity;
    uint8_t OwnershipIndicator;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char FIXClOrdID[LEN_FIX_CL_ORDID];
    uint8_t NoLegs;
    char Pad5[LEN_PAD5];
    LegOrdGrpSeqT LegOrdGrp[MAX_MODIFY_ORDER_COMPLEX_REQUEST_LEG_ORD_GRP];
} ModifyOrderComplexRequestT;

// Message:	    ModifyOrderComplexShortRequest
// TemplateID:  10130
// Alias:       Replace Order Multi Leg (short layout)
// FIX MsgType: MultilegOrderCancelReplace = "AC"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    int64_t Price;
    int64_t OrderQty;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t MatchInstCrossID;
    uint16_t EnrichmentRuleID;
    uint8_t ApplSeqIndicator;
    uint8_t ProductComplex;
    uint8_t Side;
    uint8_t PriceValidityCheckType;
    uint8_t ValueCheckTypeValue;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t ExecInst;
    uint8_t TimeInForce;
    uint8_t TradingCapacity;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char Pad2[LEN_PAD2];
} ModifyOrderComplexShortRequestT;

// Message:	    ModifyOrderNRResponse
// TemplateID:  10108
// Alias:       Replace Order Response (Lean Order)
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    uint64_t ExecID;
    int64_t LeavesQty;
    int64_t CumQty;
    int64_t CxlQty;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint16_t ExecRestatementReason;
    uint8_t CrossedIndicator;
    uint8_t ProductComplex;
    uint8_t Triggered;
    uint8_t TransactionDelayIndicator;
} ModifyOrderNRResponseT;

// Message:	    ModifyOrderResponse
// TemplateID:  10107
// Alias:       Replace Order Response (Standard Order)
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderMECompT ResponseHeaderME;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    uint64_t ExecID;
    int64_t LeavesQty;
    int64_t CumQty;
    int64_t CxlQty;
    uint64_t TrdRegTSTimePriority;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint16_t ExecRestatementReason;
    uint8_t CrossedIndicator;
    uint8_t ProductComplex;
    uint8_t Triggered;
    uint8_t TransactionDelayIndicator;
} ModifyOrderResponseT;

// Message:	    ModifyOrderSingleRequest
// TemplateID:  10106
// Alias:       Replace Order Single
// FIX MsgType: OrderCancelReplaceRequest = "G"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t Price;
    int64_t OrderQty;
    int64_t StopPx;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    uint32_t ExpireDate;
    int32_t MarketSegmentID;
    uint32_t SimpleSecurityID;
    uint32_t MatchInstCrossID;
    uint32_t TargetPartyIDSessionID;
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    uint8_t ApplSeqIndicator;
    uint8_t Side;
    uint8_t OrdType;
    uint8_t PriceValidityCheckType;
    uint8_t ValueCheckTypeValue;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t TimeInForce;
    uint8_t ExecInst;
    uint8_t TradingSessionSubID;
    uint8_t TradingCapacity;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char Account[LEN_ACCOUNT];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PositionEffect[LEN_POSITION_EFFECT];
    uint8_t OwnershipIndicator;
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char FIXClOrdID[LEN_FIX_CL_ORDID];
    char Pad7[LEN_PAD7];
} ModifyOrderSingleRequestT;

// Message:	    ModifyOrderSingleShortRequest
// TemplateID:  10126
// Alias:       Replace Order Single (short layout)
// FIX MsgType: OrderCancelReplaceRequest = "G"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t Price;
    int64_t OrderQty;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    uint32_t SimpleSecurityID;
    uint32_t MatchInstCrossID;
    uint16_t EnrichmentRuleID;
    uint8_t Side;
    uint8_t PriceValidityCheckType;
    uint8_t ValueCheckTypeValue;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t TimeInForce;
    uint8_t ApplSeqIndicator;
    uint8_t ExecInst;
    uint8_t TradingCapacity;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char Pad3[LEN_PAD3];
} ModifyOrderSingleShortRequestT;

// Message:	    ModifyTESTradeRequest
// TemplateID:  10601
// Alias:       Modify TES Trade Request
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t LastPx;
    uint64_t TransBkdTime;
    int32_t MarketSegmentID;
    uint32_t PackageID;
    uint32_t TESExecID;
    int32_t RelatedMarketSegmentID;
    uint32_t NegotiationID;
    uint8_t NoSRQSRelatedTradeIDs;
    char Pad1[LEN_PAD1];
    uint16_t TrdType;
    uint8_t TradeReportType;
    uint8_t TradePublishIndicator;
    uint8_t NoSideAllocs;
    uint8_t NoLegs;
    char TradeReportText[LEN_TRADE_REPORT_TEXT];
    char TradeReportID[LEN_TRADE_REPORTID];
    char Pad4[LEN_PAD4];
    SideAllocGrpSeqT SideAllocGrp[MAX_MODIFY_TES_TRADE_REQUEST_SIDE_ALLOC_GRP];
    TrdInstrmntLegGrpSeqT TrdInstrmntLegGrp[MAX_MODIFY_TES_TRADE_REQUEST_TRD_INSTRMNT_LEG_GRP];
    SRQSRelatedTradeIDGrpSeqT SRQSRelatedTradeIDGrp[MAX_MODIFY_TES_TRADE_REQUEST_SRQS_RELATED_TRADE_ID_GRP];
} ModifyTESTradeRequestT;

// Message:	    NewOrderComplexRequest
// TemplateID:  10113
// Alias:       New Order Multi Leg
// FIX MsgType: NewOrderMultileg = "AB"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t ClOrdID;
    int64_t SecurityID;
    int64_t Price;
    int64_t OrderQty;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t ExpireDate;
    uint32_t MatchInstCrossID;
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    uint8_t ApplSeqIndicator;
    uint8_t ProductComplex;
    uint8_t Side;
    uint8_t OrdType;
    uint8_t PriceValidityCheckType;
    uint8_t ValueCheckTypeValue;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t OrderAttributeRiskReduction;
    uint8_t ExecInst;
    uint8_t TimeInForce;
    uint8_t TradingCapacity;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char FIXClOrdID[LEN_FIX_CL_ORDID];
    uint8_t NoLegs;
    char Pad1[LEN_PAD1];
    LegOrdGrpSeqT LegOrdGrp[MAX_NEW_ORDER_COMPLEX_REQUEST_LEG_ORD_GRP];
} NewOrderComplexRequestT;

// Message:	    NewOrderComplexShortRequest
// TemplateID:  10129
// Alias:       New Order Multi Leg (short layout)
// FIX MsgType: NewOrderMultileg = "AB"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t SecurityID;
    int64_t Price;
    int64_t OrderQty;
    uint64_t ClOrdID;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t MatchInstCrossID;
    uint16_t EnrichmentRuleID;
    uint8_t ApplSeqIndicator;
    uint8_t ProductComplex;
    uint8_t Side;
    uint8_t PriceValidityCheckType;
    uint8_t ValueCheckTypeValue;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t ExecInst;
    uint8_t TimeInForce;
    uint8_t TradingCapacity;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char Pad2[LEN_PAD2];
} NewOrderComplexShortRequestT;

// Message:	    NewOrderNRResponse
// TemplateID:  10102
// Alias:       New Order Response (Lean Order)
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t OrderID;
    uint64_t ClOrdID;
    int64_t SecurityID;
    uint64_t ExecID;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint16_t ExecRestatementReason;
    uint8_t CrossedIndicator;
    uint8_t ProductComplex;
    uint8_t Triggered;
    uint8_t TransactionDelayIndicator;
} NewOrderNRResponseT;

// Message:	    NewOrderResponse
// TemplateID:  10101
// Alias:       New Order Response (Standard Order)
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderMECompT ResponseHeaderME;
    uint64_t OrderID;
    uint64_t ClOrdID;
    int64_t SecurityID;
    uint64_t ExecID;
    uint64_t TrdRegTSEntryTime;
    uint64_t TrdRegTSTimePriority;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint16_t ExecRestatementReason;
    uint8_t CrossedIndicator;
    uint8_t ProductComplex;
    uint8_t Triggered;
    uint8_t TransactionDelayIndicator;
} NewOrderResponseT;

// Message:	    NewOrderSingleRequest
// TemplateID:  10100
// Alias:       New Order Single
// FIX MsgType: NewOrderSingle = "D"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t Price;
    int64_t OrderQty;
    int64_t StopPx;
    uint64_t ClOrdID;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    uint32_t ExpireDate;
    int32_t MarketSegmentID;
    uint32_t SimpleSecurityID;
    uint32_t MatchInstCrossID;
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    uint8_t ApplSeqIndicator;
    uint8_t Side;
    uint8_t OrdType;
    uint8_t PriceValidityCheckType;
    uint8_t ValueCheckTypeValue;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t OrderAttributeRiskReduction;
    uint8_t TimeInForce;
    uint8_t ExecInst;
    uint8_t TradingSessionSubID;
    uint8_t TradingCapacity;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char Account[LEN_ACCOUNT];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PositionEffect[LEN_POSITION_EFFECT];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char FIXClOrdID[LEN_FIX_CL_ORDID];
    char Pad3[LEN_PAD3];
} NewOrderSingleRequestT;

// Message:	    NewOrderSingleShortRequest
// TemplateID:  10125
// Alias:       New Order Single (short layout)
// FIX MsgType: NewOrderSingle = "D"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t Price;
    int64_t OrderQty;
    uint64_t ClOrdID;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    uint32_t SimpleSecurityID;
    uint32_t MatchInstCrossID;
    uint16_t EnrichmentRuleID;
    uint8_t Side;
    uint8_t ApplSeqIndicator;
    uint8_t PriceValidityCheckType;
    uint8_t ValueCheckTypeValue;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t TimeInForce;
    uint8_t ExecInst;
    uint8_t TradingCapacity;
    uint8_t OrderOrigination;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char Pad3[LEN_PAD3];
} NewOrderSingleShortRequestT;

// Message:	    NewsBroadcast
// TemplateID:  10031
// Alias:       News
// FIX MsgType: News = "B"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t OrigTime;
    uint16_t VarTextLen;
    char Headline[LEN_HEADLINE];
    char Pad6[LEN_PAD6];
    char VarText[LEN_VAR_TEXT];
} NewsBroadcastT;

// Message:	    OrderExecNotification
// TemplateID:  10104
// Alias:       Book Order Execution
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    uint64_t ExecID;
    int64_t LeavesQty;
    int64_t CumQty;
    int64_t CxlQty;
    int32_t MarketSegmentID;
    uint16_t NoLegExecs;
    uint16_t ExecRestatementReason;
    uint8_t Side;
    uint8_t ProductComplex;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint8_t Triggered;
    uint8_t CrossedIndicator;
    char FIXClOrdID[LEN_FIX_CL_ORDID];
    uint8_t NoFills;
    char Pad5[LEN_PAD5];
    FillsGrpSeqT FillsGrp[MAX_ORDER_EXEC_NOTIFICATION_FILLS_GRP];
    InstrmntLegExecGrpSeqT InstrmntLegExecGrp[MAX_ORDER_EXEC_NOTIFICATION_INSTRMNT_LEG_EXEC_GRP];
} OrderExecNotificationT;

// Message:	    OrderExecReportBroadcast
// TemplateID:  10117
// Alias:       Extended Order Information
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    uint64_t ExecID;
    uint64_t TrdRegTSEntryTime;
    uint64_t TrdRegTSTimePriority;
    int64_t Price;
    int64_t LeavesQty;
    int64_t CumQty;
    int64_t CxlQty;
    int64_t OrderQty;
    int64_t StopPx;
    int32_t MarketSegmentID;
    uint32_t ExpireDate;
    uint32_t MatchInstCrossID;
    uint32_t PartyIDExecutingUnit;
    uint32_t PartyIDSessionID;
    uint32_t PartyIDExecutingTrader;
    uint32_t PartyIDEnteringTrader;
    uint16_t NoLegExecs;
    uint16_t ExecRestatementReason;
    uint8_t PartyIDEnteringFirm;
    uint8_t ProductComplex;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint8_t Side;
    uint8_t OrdType;
    uint8_t TradingCapacity;
    uint8_t TimeInForce;
    uint8_t ExecInst;
    uint8_t TradingSessionSubID;
    uint8_t ApplSeqIndicator;
    char Account[LEN_ACCOUNT];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PositionEffect[LEN_POSITION_EFFECT];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char FIXClOrdID[LEN_FIX_CL_ORDID];
    uint8_t NoFills;
    uint8_t NoLegs;
    uint8_t Triggered;
    uint8_t CrossedIndicator;
    char Pad2[LEN_PAD2];
    LegOrdGrpSeqT LegOrdGrp[MAX_ORDER_EXEC_REPORT_BROADCAST_LEG_ORD_GRP];
    FillsGrpSeqT FillsGrp[MAX_ORDER_EXEC_REPORT_BROADCAST_FILLS_GRP];
    InstrmntLegExecGrpSeqT InstrmntLegExecGrp[MAX_ORDER_EXEC_REPORT_BROADCAST_INSTRMNT_LEG_EXEC_GRP];
} OrderExecReportBroadcastT;

// Message:	    OrderExecResponse
// TemplateID:  10103
// Alias:       Immediate Execution Response
// FIX MsgType: ExecutionReport = "8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderMECompT ResponseHeaderME;
    uint64_t OrderID;
    uint64_t ClOrdID;
    uint64_t OrigClOrdID;
    int64_t SecurityID;
    uint64_t ExecID;
    uint64_t TrdRegTSEntryTime;
    uint64_t TrdRegTSTimePriority;
    int64_t LeavesQty;
    int64_t CumQty;
    int64_t CxlQty;
    int32_t MarketSegmentID;
    uint16_t NoLegExecs;
    uint16_t ExecRestatementReason;
    uint8_t Side;
    uint8_t ProductComplex;
    char OrdStatus[LEN_ORD_STATUS];
    char ExecType[LEN_EXEC_TYPE];
    uint8_t Triggered;
    uint8_t CrossedIndicator;
    uint8_t TransactionDelayIndicator;
    uint8_t NoFills;
    FillsGrpSeqT FillsGrp[MAX_ORDER_EXEC_RESPONSE_FILLS_GRP];
    InstrmntLegExecGrpSeqT InstrmntLegExecGrp[MAX_ORDER_EXEC_RESPONSE_INSTRMNT_LEG_EXEC_GRP];
} OrderExecResponseT;

// Message:	    PartyActionReport
// TemplateID:  10042
// Alias:       Party Action Report
// FIX MsgType: PartyActionReport = "DI"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    uint32_t TradeDate;
    uint32_t RequestingPartyIDExecutingTrader;
    uint32_t PartyIDExecutingUnit;
    uint32_t PartyIDExecutingTrader;
    uint32_t RequestingPartyIDExecutingSystem;
    uint16_t MarketID;
    uint8_t PartyActionType;
    uint8_t RequestingPartyIDEnteringFirm;
} PartyActionReportT;

// Message:	    PartyEntitlementsUpdateReport
// TemplateID:  10034
// Alias:       Entitlement Notification
// FIX MsgType: PartyEntitlementsUpdateReport = "CZ"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    uint32_t TradeDate;
    uint32_t PartyDetailIDExecutingUnit;
    uint32_t RequestingPartyIDExecutingSystem;
    uint16_t MarketID;
    char ListUpdateAction[LEN_LIST_UPDATE_ACTION];
    char RequestingPartyEnteringFirm[LEN_REQUESTING_PARTY_ENTERING_FIRM];
    char RequestingPartyClearingFirm[LEN_REQUESTING_PARTY_CLEARING_FIRM];
    uint8_t PartyDetailStatus;
    char Pad6[LEN_PAD6];
} PartyEntitlementsUpdateReportT;

// Message:	    PreTradeRiskLimitResponse
// TemplateID:  10313
// Alias:       Pre-Trade Risk Limit Response
// FIX MsgType: PartyRiskLimitsReport = "CM"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t RiskLimitReportID;
    int32_t MarketSegmentID;
    uint8_t NoRiskLimits;
    uint8_t PartyDetailStatus;
    uint8_t RiskLimitPlatform;
    char PartyDetailExecutingUnit[LEN_PARTY_DETAIL_EXECUTING_UNIT];
    char Pad4[LEN_PAD4];
    RiskLimitsRptGrpSeqT RiskLimitsRptGrp[MAX_PRE_TRADE_RISK_LIMIT_RESPONSE_RISK_LIMITS_RPT_GRP];
} PreTradeRiskLimitResponseT;

// Message:	    PreTradeRiskLimitsDefinitionRequest
// TemplateID:  10312
// Alias:       Pre-Trade Risk Limits Definition Request
// FIX MsgType: PartyRiskLimitsDefinitionRequest = "CS"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int32_t MarketSegmentID;
    uint8_t RiskLimitPlatform;
    uint8_t NoRiskLimitsQty;
    uint8_t PartyDetailStatus;
    char RiskLimitGroup[LEN_RISK_LIMIT_GROUP];
    char PartyDetailExecutingUnit[LEN_PARTY_DETAIL_EXECUTING_UNIT];
    char Pad1[LEN_PAD1];
    RiskLimitQtyGrpSeqT RiskLimitQtyGrp[MAX_PRE_TRADE_RISK_LIMITS_DEFINITION_REQUEST_RISK_LIMIT_QTY_GRP];
} PreTradeRiskLimitsDefinitionRequestT;

// Message:	    QuoteActivationNotification
// TemplateID:  10411
// Alias:       Quote Activation Notification
// FIX MsgType: OrderMassActionReport = "BZ"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    uint64_t MassActionReportID;
    int32_t MarketSegmentID;
    uint32_t PartyIDEnteringTrader;
    uint16_t NoNotAffectedSecurities;
    uint8_t PartyIDEnteringFirm;
    uint8_t MassActionType;
    uint8_t MassActionSubType;
    uint8_t MassActionReason;
    char Pad2[LEN_PAD2];
    NotAffectedSecuritiesGrpSeqT NotAffectedSecuritiesGrp[MAX_QUOTE_ACTIVATION_NOTIFICATION_NOT_AFFECTED_SECURITIES_GRP];
} QuoteActivationNotificationT;

// Message:	    QuoteActivationRequest
// TemplateID:  10403
// Alias:       Quote Activation Request
// FIX MsgType: OrderMassActionRequest = "CA"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t TargetPartyIDSessionID;
    uint8_t MassActionType;
    uint8_t MassActionSubType;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t ExecutingTraderQualifier;
    char Pad4[LEN_PAD4];
} QuoteActivationRequestT;

// Message:	    QuoteActivationResponse
// TemplateID:  10404
// Alias:       Quote Activation Response
// FIX MsgType: OrderMassActionReport = "BZ"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t MassActionReportID;
    uint16_t NoNotAffectedSecurities;
    char Pad6[LEN_PAD6];
    NotAffectedSecuritiesGrpSeqT NotAffectedSecuritiesGrp[MAX_QUOTE_ACTIVATION_RESPONSE_NOT_AFFECTED_SECURITIES_GRP];
} QuoteActivationResponseT;

// Message:	    QuoteExecutionReport
// TemplateID:  10407
// Alias:       Quote Execution Notification
// FIX MsgType: QuoteExecutionReport = "U8"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    uint64_t ExecID;
    int32_t MarketSegmentID;
    uint16_t NoLegExecs;
    uint8_t NoQuoteEvents;
    char Pad1[LEN_PAD1];
    QuoteEventGrpSeqT QuoteEventGrp[MAX_QUOTE_EXECUTION_REPORT_QUOTE_EVENT_GRP];
    QuoteLegExecGrpSeqT QuoteLegExecGrp[MAX_QUOTE_EXECUTION_REPORT_QUOTE_LEG_EXEC_GRP];
} QuoteExecutionReportT;

// Message:	    RFQRequest
// TemplateID:  10401
// Alias:       Quote Request
// FIX MsgType: QuoteRequest = "R"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t SecurityID;
    int64_t OrderQty;
    int32_t MarketSegmentID;
    uint8_t Side;
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char Pad7[LEN_PAD7];
} RFQRequestT;

// Message:	    RFQResponse
// TemplateID:  10402
// Alias:       Quote Request Response
// FIX MsgType: RequestAcknowledge = "U1"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint64_t ExecID;
} RFQResponseT;

// Message:	    Reject
// TemplateID:  10010
// Alias:       Reject
// FIX MsgType: Reject = "3"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRResponseHeaderMECompT NRResponseHeaderME;
    uint32_t SessionRejectReason;
    uint16_t VarTextLen;
    uint8_t SessionStatus;
    char Pad1[LEN_PAD1];
    char VarText[LEN_VAR_TEXT];
} RejectT;

// Message:	    RetransmitMEMessageRequest
// TemplateID:  10026
// Alias:       Retransmit (Order/Quote Event)
// FIX MsgType: ApplicationMessageRequest = "BW"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint32_t SubscriptionScope;
    uint16_t PartitionID;
    uint8_t RefApplID;
    char ApplBegMsgID[LEN_APPL_BEG_MSGID];
    char ApplEndMsgID[LEN_APPL_END_MSGID];
    char Pad1[LEN_PAD1];
} RetransmitMEMessageRequestT;

// Message:	    RetransmitMEMessageResponse
// TemplateID:  10027
// Alias:       Retransmit Response (Order/Quote Event)
// FIX MsgType: ApplicationMessageRequestAck = "BX"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    uint16_t ApplTotalMessageCount;
    char ApplEndMsgID[LEN_APPL_END_MSGID];
    char RefApplLastMsgID[LEN_REF_APPL_LAST_MSGID];
    char Pad6[LEN_PAD6];
} RetransmitMEMessageResponseT;

// Message:	    RetransmitRequest
// TemplateID:  10008
// Alias:       Retransmit
// FIX MsgType: ApplicationMessageRequest = "BW"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t ApplBegSeqNum;
    uint64_t ApplEndSeqNum;
    uint16_t PartitionID;
    uint8_t RefApplID;
    char Pad5[LEN_PAD5];
} RetransmitRequestT;

// Message:	    RetransmitResponse
// TemplateID:  10009
// Alias:       Retransmit Response
// FIX MsgType: ApplicationMessageRequestAck = "BX"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    uint64_t ApplEndSeqNum;
    uint64_t RefApplLastSeqNum;
    uint16_t ApplTotalMessageCount;
    char Pad6[LEN_PAD6];
} RetransmitResponseT;

// Message:	    RiskNotificationBroadcast
// TemplateID:  10033
// Alias:       Risk Notification
// FIX MsgType: PartyRiskLimitsUpdateReport = "CR"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    uint32_t TradeDate;
    uint32_t PartyDetailIDExecutingUnit;
    uint32_t RequestingPartyIDExecutingSystem;
    uint16_t MarketID;
    char ListUpdateAction[LEN_LIST_UPDATE_ACTION];
    uint8_t RiskLimitAction;
    char RequestingPartyEnteringFirm[LEN_REQUESTING_PARTY_ENTERING_FIRM];
    char RequestingPartyClearingFirm[LEN_REQUESTING_PARTY_CLEARING_FIRM];
    char Pad6[LEN_PAD6];
} RiskNotificationBroadcastT;

// Message:	    SRQSCreateDealNotification
// TemplateID:  10708
// Alias:       SRQS Create Deal Notification
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    int64_t LastPx;
    int64_t LastQty;
    uint64_t QuoteID;
    int64_t SecurityID;
    uint64_t ExpireTime;
    int64_t UnderlyingPx;
    int64_t UnderlyingDeltaPercentage;
    int64_t UnderlyingEffectiveDeltaPercentage;
    int64_t UnderlyingQty;
    int64_t UnderlyingPriceStipValue;
    uint32_t NegotiationID;
    uint32_t TradeID;
    uint32_t OrigTradeID;
    uint8_t TrdRptStatus;
    uint8_t TradeReportType;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    uint8_t Side;
    uint8_t NoOrderBookItems;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t TradingCapacity;
    uint8_t TradePublishIndicator;
    char RootPartyExecutingFirm[LEN_ROOT_PARTY_EXECUTING_FIRM];
    char RootPartyExecutingTrader[LEN_ROOT_PARTY_EXECUTING_TRADER];
    char RootPartyEnteringTrader[LEN_ROOT_PARTY_ENTERING_TRADER];
    char TargetPartyExecutingFirm[LEN_TARGET_PARTY_EXECUTING_FIRM];
    char TargetPartyExecutingTrader[LEN_TARGET_PARTY_EXECUTING_TRADER];
    char FirmTradeID[LEN_FIRM_TRADEID];
    char FirmNegotiationID[LEN_FIRM_NEGOTIATIONID];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char FreeText5[LEN_FREE_TEXT5];
    char PositionEffect[LEN_POSITION_EFFECT];
    char Account[LEN_ACCOUNT];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char Pad1[LEN_PAD1];
    OrderBookItemGrpSeqT OrderBookItemGrp[MAX_SRQS_CREATE_DEAL_NOTIFICATION_ORDER_BOOK_ITEM_GRP];
} SRQSCreateDealNotificationT;

// Message:	    SRQSDealNotification
// TemplateID:  10709
// Alias:       SRQS Deal Notification
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    int64_t UnderlyingPriceStipValue;
    int64_t UnderlyingPx;
    int64_t LastPx;
    int64_t LastQty;
    uint32_t NegotiationID;
    uint32_t TradeID;
    uint16_t RequestingPartySubIDType;
    uint8_t TrdRptStatus;
    uint8_t TradeRequestResult;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t TradingCapacity;
    char RootPartyExecutingFirm[LEN_ROOT_PARTY_EXECUTING_FIRM];
    char RootPartyExecutingTrader[LEN_ROOT_PARTY_EXECUTING_TRADER];
    char RootPartyEnteringTrader[LEN_ROOT_PARTY_ENTERING_TRADER];
    char TargetPartyExecutingFirm[LEN_TARGET_PARTY_EXECUTING_FIRM];
    char TargetPartyExecutingTrader[LEN_TARGET_PARTY_EXECUTING_TRADER];
    char TargetPartyEnteringTrader[LEN_TARGET_PARTY_ENTERING_TRADER];
    char FirmTradeID[LEN_FIRM_TRADEID];
    char FirmNegotiationID[LEN_FIRM_NEGOTIATIONID];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char FreeText5[LEN_FREE_TEXT5];
    char PositionEffect[LEN_POSITION_EFFECT];
    char Account[LEN_ACCOUNT];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
} SRQSDealNotificationT;

// Message:	    SRQSDealResponse
// TemplateID:  10705
// Alias:       SRQS Deal Response
// FIX MsgType: QuoteAck = "CW"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    int64_t SecurityID;
    uint64_t QuoteID;
    uint32_t NegotiationID;
    uint32_t TradeID;
    uint32_t SecondaryTradeID;
    char FirmTradeID[LEN_FIRM_TRADEID];
    char FirmNegotiationID[LEN_FIRM_NEGOTIATIONID];
    char Pad4[LEN_PAD4];
} SRQSDealResponseT;

// Message:	    SRQSEnterQuoteRequest
// TemplateID:  10702
// Alias:       SRQS Enter Quote Request
// FIX MsgType: QuoteRequest = "R"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t BidPx;
    int64_t OfferPx;
    int64_t UnderlyingPx;
    int64_t UnderlyingDeltaPercentage;
    int64_t BidSize;
    int64_t OfferSize;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t NegotiationID;
    uint8_t ValueCheckTypeQuantity;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t ExecutingTraderQualifier;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t TradingCapacity;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char FreeText5[LEN_FREE_TEXT5];
    char PositionEffect[LEN_POSITION_EFFECT];
    char Account[LEN_ACCOUNT];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char Pad1[LEN_PAD1];
} SRQSEnterQuoteRequestT;

// Message:	    SRQSHitQuoteRequest
// TemplateID:  10704
// Alias:       SRQS Hit Quote Request
// FIX MsgType: QuoteResponse = "AJ"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint64_t QuoteID;
    int64_t OrderQty;
    uint64_t ValidUntilTime;
    int64_t UnderlyingQty;
    int64_t UnderlyingPriceStipValue;
    uint64_t PartyIDClientID;
    uint64_t PartyIdInvestmentDecisionMaker;
    uint64_t ExecutingTrader;
    int32_t MarketSegmentID;
    uint32_t NegotiationID;
    uint8_t Side;
    uint8_t ValueCheckTypeQuantity;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t ExecutingTraderQualifier;
    uint8_t PartyIdInvestmentDecisionMakerQualifier;
    uint8_t TradingCapacity;
    uint8_t TradePublishIndicator;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char FirmTradeID[LEN_FIRM_TRADEID];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char FreeText5[LEN_FREE_TEXT5];
    char PositionEffect[LEN_POSITION_EFFECT];
    char Account[LEN_ACCOUNT];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char Pad3[LEN_PAD3];
} SRQSHitQuoteRequestT;

// Message:	    SRQSNegotiationNotification
// TemplateID:  10713
// Alias:       SRQS Negotiation Notification for Respondent
// FIX MsgType: QuoteStatusReport = "AI"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    int64_t QuoteRefPrice;
    int64_t UnderlyingDeltaPercentage;
    int64_t BidPx;
    int64_t OfferPx;
    int64_t LastPx;
    int64_t LeavesQty;
    int64_t LastQty;
    uint64_t EffectiveTime;
    uint32_t NegotiationID;
    uint32_t NumberOfRespondents;
    uint8_t QuoteType;
    uint8_t QuoteSubType;
    uint8_t QuoteStatus;
    uint8_t QuoteInstruction;
    uint8_t Side;
    uint8_t QuoteRefPriceSource;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char PartyEnteringTrader[LEN_PARTY_ENTERING_TRADER];
    char TargetPartyExecutingFirm[LEN_TARGET_PARTY_EXECUTING_FIRM];
    char TargetPartyExecutingTrader[LEN_TARGET_PARTY_EXECUTING_TRADER];
    char FirmNegotiationID[LEN_FIRM_NEGOTIATIONID];
    char FreeText5[LEN_FREE_TEXT5];
    char Pad6[LEN_PAD6];
} SRQSNegotiationNotificationT;

// Message:	    SRQSNegotiationRequesterNotification
// TemplateID:  10712
// Alias:       SRQS Negotiation Notification for Requester
// FIX MsgType: QuoteStatusReport = "AI"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    uint64_t TrdRegTSExecutionTime;
    int64_t QuoteRefPrice;
    int64_t UnderlyingDeltaPercentage;
    int64_t BidPx;
    int64_t OfferPx;
    int64_t OrderQty;
    int64_t LastPx;
    int64_t LeavesQty;
    int64_t LastQty;
    uint64_t EffectiveTime;
    uint32_t NegotiationID;
    uint32_t NumberOfRespondents;
    uint8_t QuoteType;
    uint8_t QuoteSubType;
    uint8_t QuoteStatus;
    uint8_t NoTargetPartyIDs;
    uint8_t NumberOfRespDisclosureInstruction;
    uint8_t Side;
    uint8_t QuoteRefPriceSource;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char PartyEnteringTrader[LEN_PARTY_ENTERING_TRADER];
    char FirmNegotiationID[LEN_FIRM_NEGOTIATIONID];
    char FreeText5[LEN_FREE_TEXT5];
    TargetPartiesSeqT TargetParties[MAX_SRQS_NEGOTIATION_REQUESTER_NOTIFICATION_TARGET_PARTIES];
} SRQSNegotiationRequesterNotificationT;

// Message:	    SRQSNegotiationStatusNotification
// TemplateID:  10715
// Alias:       SRQS Negotiation Status Notification
// FIX MsgType: QuoteStatusReport = "AI"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    uint64_t EffectiveTime;
    uint32_t NegotiationID;
    uint8_t QuoteStatus;
    char FirmNegotiationID[LEN_FIRM_NEGOTIATIONID];
    char Pad7[LEN_PAD7];
} SRQSNegotiationStatusNotificationT;

// Message:	    SRQSOpenNegotiationNotification
// TemplateID:  10711
// Alias:       SRQS Open Negotiation Notification for Respondent
// FIX MsgType: QuoteStatusReport = "AI"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    uint64_t NegotiationStartTime;
    int64_t SecurityID;
    int64_t BidPx;
    int64_t OfferPx;
    int64_t LeavesQty;
    int64_t LastQty;
    int64_t LastPx;
    int64_t QuoteRefPrice;
    int64_t UnderlyingDeltaPercentage;
    uint64_t ExpireTime;
    uint32_t NegotiationID;
    int32_t MarketSegmentID;
    int32_t SecuritySubType;
    uint32_t NumberOfRespondents;
    uint8_t QuoteType;
    uint8_t QuoteSubType;
    uint8_t QuoteQualifier;
    uint8_t QuoteStatus;
    uint8_t NoLegs;
    uint8_t Side;
    uint8_t QuoteRefPriceSource;
    uint8_t TradeUnderlying;
    uint8_t ProductComplex;
    uint8_t NegotiateUnderlying;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char PartyEnteringTrader[LEN_PARTY_ENTERING_TRADER];
    char TargetPartyExecutingFirm[LEN_TARGET_PARTY_EXECUTING_FIRM];
    char TargetPartyExecutingTrader[LEN_TARGET_PARTY_EXECUTING_TRADER];
    char FirmNegotiationID[LEN_FIRM_NEGOTIATIONID];
    char FreeText5[LEN_FREE_TEXT5];
    char Pad2[LEN_PAD2];
    QuotReqLegsGrpSeqT QuotReqLegsGrp[MAX_SRQS_OPEN_NEGOTIATION_NOTIFICATION_QUOT_REQ_LEGS_GRP];
} SRQSOpenNegotiationNotificationT;

// Message:	    SRQSOpenNegotiationRequest
// TemplateID:  10700
// Alias:       SRQS Open Negotiation Request
// FIX MsgType: QuoteRequest = "R"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t SecurityID;
    int64_t BidPx;
    int64_t OfferPx;
    int64_t OrderQty;
    int64_t QuoteRefPrice;
    int64_t UnderlyingDeltaPercentage;
    uint64_t ValidUntilTime;
    int32_t MarketSegmentID;
    int32_t SecuritySubType;
    uint8_t QuoteType;
    uint8_t QuoteSubType;
    uint8_t QuoteQualifier;
    uint8_t NoLegs;
    uint8_t NoTargetPartyIDs;
    uint8_t NumberOfRespDisclosureInstruction;
    uint8_t Side;
    uint8_t QuoteRefPriceSource;
    uint8_t TradeUnderlying;
    uint8_t NegotiateUnderlying;
    uint8_t ProductComplex;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char FreeText5[LEN_FREE_TEXT5];
    char QuoteReqID[LEN_QUOTE_REQID];
    char Pad2[LEN_PAD2];
    QuotReqLegsGrpSeqT QuotReqLegsGrp[MAX_SRQS_OPEN_NEGOTIATION_REQUEST_QUOT_REQ_LEGS_GRP];
    TargetPartiesSeqT TargetParties[MAX_SRQS_OPEN_NEGOTIATION_REQUEST_TARGET_PARTIES];
} SRQSOpenNegotiationRequestT;

// Message:	    SRQSOpenNegotiationRequesterNotification
// TemplateID:  10710
// Alias:       SRQS Open Negotiation Notification for Requester
// FIX MsgType: QuoteStatusReport = "AI"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    int64_t SecurityID;
    int64_t BidPx;
    int64_t OfferPx;
    int64_t OrderQty;
    int64_t LastPx;
    int64_t LastQty;
    int64_t QuoteRefPrice;
    int64_t UnderlyingDeltaPercentage;
    uint64_t ExpireTime;
    uint32_t NegotiationID;
    int32_t MarketSegmentID;
    int32_t SecuritySubType;
    uint32_t NumberOfRespondents;
    uint8_t QuoteType;
    uint8_t QuoteSubType;
    uint8_t QuoteQualifier;
    uint8_t QuoteStatus;
    uint8_t NoLegs;
    uint8_t NoTargetPartyIDs;
    uint8_t Side;
    uint8_t QuoteRefPriceSource;
    uint8_t TradeUnderlying;
    uint8_t NegotiateUnderlying;
    uint8_t ProductComplex;
    uint8_t NumberOfRespDisclosureInstruction;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char PartyEnteringTrader[LEN_PARTY_ENTERING_TRADER];
    char FirmNegotiationID[LEN_FIRM_NEGOTIATIONID];
    char FreeText5[LEN_FREE_TEXT5];
    char Pad3[LEN_PAD3];
    QuotReqLegsGrpSeqT QuotReqLegsGrp[MAX_SRQS_OPEN_NEGOTIATION_REQUESTER_NOTIFICATION_QUOT_REQ_LEGS_GRP];
    TargetPartiesSeqT TargetParties[MAX_SRQS_OPEN_NEGOTIATION_REQUESTER_NOTIFICATION_TARGET_PARTIES];
} SRQSOpenNegotiationRequesterNotificationT;

// Message:	    SRQSQuoteNotification
// TemplateID:  10707
// Alias:       SRQS Quote Notification for Responder
// FIX MsgType: Quote = "S"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    uint64_t QuoteID;
    uint64_t SecondaryQuoteID;
    int64_t BidPx;
    int64_t BidSize;
    int64_t OfferPx;
    int64_t OfferSize;
    int64_t UnderlyingPx;
    int64_t UnderlyingDeltaPercentage;
    uint32_t NegotiationID;
    uint8_t QuotingStatus;
    uint8_t TradingCapacity;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char PartyEnteringTrader[LEN_PARTY_ENTERING_TRADER];
    char QuoteReqID[LEN_QUOTE_REQID];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char FreeText5[LEN_FREE_TEXT5];
    char PositionEffect[LEN_POSITION_EFFECT];
    char Account[LEN_ACCOUNT];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char Pad6[LEN_PAD6];
} SRQSQuoteNotificationT;

// Message:	    SRQSQuoteResponse
// TemplateID:  10703
// Alias:       SRQS Quote Response
// FIX MsgType: QuoteAck = "CW"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    uint64_t QuoteID;
    uint32_t NegotiationID;
    char QuoteReqID[LEN_QUOTE_REQID];
} SRQSQuoteResponseT;

// Message:	    SRQSQuotingStatusRequest
// TemplateID:  10717
// Alias:       SRQS Enter Quoting Status Request
// FIX MsgType: QuoteRequest = "R"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int32_t MarketSegmentID;
    uint32_t NegotiationID;
    uint8_t QuotingStatus;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char FreeText5[LEN_FREE_TEXT5];
} SRQSQuotingStatusRequestT;

// Message:	    SRQSStatusBroadcast
// TemplateID:  10714
// Alias:       SRQS Status Notification
// FIX MsgType: QuoteStatusReport = "AI"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint32_t TradeDate;
    uint8_t TradSesEvent;
    char Pad3[LEN_PAD3];
} SRQSStatusBroadcastT;

// Message:	    SRQSUpdateDealStatusRequest
// TemplateID:  10706
// Alias:       SRQS Update Deal Request
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t UnderlyingPriceStipValue;
    int64_t UnderlyingPx;
    int64_t LastQty;
    int32_t MarketSegmentID;
    uint32_t NegotiationID;
    uint32_t TradeID;
    uint8_t TradeReportType;
    uint8_t TrdRptStatus;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char FreeText5[LEN_FREE_TEXT5];
    char Pad3[LEN_PAD3];
} SRQSUpdateDealStatusRequestT;

// Message:	    SRQSUpdateNegotiationRequest
// TemplateID:  10701
// Alias:       SRQS Update Negotiation Request
// FIX MsgType: QuoteRequest = "R"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t QuoteRefPrice;
    int64_t UnderlyingDeltaPercentage;
    int64_t BidPx;
    int64_t OfferPx;
    int64_t OrderQty;
    int32_t MarketSegmentID;
    uint32_t NegotiationID;
    uint8_t NoTargetPartyIDs;
    uint8_t NumberOfRespDisclosureInstruction;
    uint8_t Side;
    uint8_t QuoteCancelType;
    uint8_t QuoteRefPriceSource;
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    char FreeText5[LEN_FREE_TEXT5];
    char Pad4[LEN_PAD4];
    TargetPartiesSeqT TargetParties[MAX_SRQS_UPDATE_NEGOTIATION_REQUEST_TARGET_PARTIES];
} SRQSUpdateNegotiationRequestT;

// Message:	    ServiceAvailabilityBroadcast
// TemplateID:  10030
// Alias:       Service Availability
// FIX MsgType: UserNotification = "CB"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRBCHeaderCompT NRBCHeader;
    uint32_t MatchingEngineTradeDate;
    uint32_t TradeManagerTradeDate;
    uint32_t ApplSeqTradeDate;
    uint32_t T7EntryServiceTradeDate;
    uint32_t T7EntryServiceRtmTradeDate;
    uint16_t PartitionID;
    uint8_t MatchingEngineStatus;
    uint8_t TradeManagerStatus;
    uint8_t ApplSeqStatus;
    uint8_t T7EntryServiceStatus;
    uint8_t T7EntryServiceRtmStatus;
    char Pad5[LEN_PAD5];
} ServiceAvailabilityBroadcastT;

// Message:	    ServiceAvailabilityMarketBroadcast
// TemplateID:  10044
// Alias:       Service Availability Market
// FIX MsgType: UserNotification = "CB"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NRBCHeaderCompT NRBCHeader;
    uint32_t SelectiveRequestForQuoteServiceTradeDate;
    uint8_t SelectiveRequestForQuoteServiceStatus;
    char Pad3[LEN_PAD3];
} ServiceAvailabilityMarketBroadcastT;

// Message:	    SubscribeRequest
// TemplateID:  10025
// Alias:       Subscribe
// FIX MsgType: ApplicationMessageRequest = "BW"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint32_t SubscriptionScope;
    uint8_t RefApplID;
    char Pad3[LEN_PAD3];
} SubscribeRequestT;

// Message:	    SubscribeResponse
// TemplateID:  10005
// Alias:       Subscribe Response
// FIX MsgType: ApplicationMessageRequestAck = "BX"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    uint32_t ApplSubID;
    char Pad4[LEN_PAD4];
} SubscribeResponseT;

// Message:	    TESApproveBroadcast
// TemplateID:  10607
// Alias:       Approve TES Trade Broadcast
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    int64_t SecurityID;
    int64_t LastPx;
    int64_t AllocQty;
    uint64_t TransactTime;
    int64_t UnderlyingPx;
    uint64_t TransBkdTime;
    int64_t RelatedClosePrice;
    int64_t RelatedTradeQuantity;
    int64_t UnderlyingQty;
    int32_t MarketSegmentID;
    uint32_t PackageID;
    uint32_t TESExecID;
    uint32_t AllocID;
    uint32_t UnderlyingSettlementDate;
    uint32_t UnderlyingMaturityDate;
    uint32_t RelatedTradeID;
    int32_t RelatedMarketSegmentID;
    uint32_t NegotiationID;
    uint8_t NoSRQSRelatedTradeIDs;
    char Pad1[LEN_PAD1];
    uint16_t TrdType;
    uint8_t Side;
    uint8_t TradePublishIndicator;
    uint8_t ProductComplex;
    uint8_t TradeReportType;
    uint8_t TradingCapacity;
    uint8_t PartyIDSettlementLocation;
    uint8_t TradeAllocStatus;
    uint8_t HedgeType;
    uint8_t NoLegs;
    uint8_t NoEvents;
    uint8_t NoInstrAttrib;
    uint8_t NoUnderlyingStips;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    char TradeReportID[LEN_TRADE_REPORTID];
    char PartyExecutingFirm[LEN_PARTY_EXECUTING_FIRM];
    char PartyExecutingTrader[LEN_PARTY_EXECUTING_TRADER];
    uint8_t PartyIDEnteringFirm;
    char PartyEnteringTrader[LEN_PARTY_ENTERING_TRADER];
    char PositionEffect[LEN_POSITION_EFFECT];
    char RootPartyExecutingFirm[LEN_ROOT_PARTY_EXECUTING_FIRM];
    char RootPartyExecutingTrader[LEN_ROOT_PARTY_EXECUTING_TRADER];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char PartyIDTakeUpTradingFirm[LEN_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char Account[LEN_ACCOUNT];
    char PartyIDPositionAccount[LEN_PARTY_ID_POSITION_ACCOUNT];
    char PartyIDOrderOriginationFirm[LEN_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char PartyIDBeneficiary[LEN_PARTY_ID_BENEFICIARY];
    char PartyIDLocationID[LEN_PARTY_ID_LOCATIONID];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char ComplianceText[LEN_COMPLIANCE_TEXT];
    char UnderlyingSecurityID[LEN_UNDERLYING_SECURITYID];
    char UnderlyingSecurityDesc[LEN_UNDERLYING_SECURITY_DESC];
    char UnderlyingCurrency[LEN_UNDERLYING_CURRENCY];
    char UnderlyingIssuer[LEN_UNDERLYING_ISSUER];
    char Pad4[LEN_PAD4];
    TrdInstrmntLegGrpSeqT TrdInstrmntLegGrp[MAX_TES_APPROVE_BROADCAST_TRD_INSTRMNT_LEG_GRP];
    InstrumentEventGrpSeqT InstrumentEventGrp[MAX_TES_APPROVE_BROADCAST_INSTRUMENT_EVENT_GRP];
    InstrumentAttributeGrpSeqT InstrumentAttributeGrp[MAX_TES_APPROVE_BROADCAST_INSTRUMENT_ATTRIBUTE_GRP];
    UnderlyingStipGrpSeqT UnderlyingStipGrp[MAX_TES_APPROVE_BROADCAST_UNDERLYING_STIP_GRP];
    SRQSRelatedTradeIDGrpSeqT SRQSRelatedTradeIDGrp[MAX_TES_APPROVE_BROADCAST_SRQS_RELATED_TRADE_ID_GRP];
} TESApproveBroadcastT;

// Message:	    TESBroadcast
// TemplateID:  10604
// Alias:       TES Broadcast
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    int64_t SecurityID;
    int64_t LastPx;
    uint64_t TransactTime;
    int64_t UnderlyingPx;
    uint64_t TransBkdTime;
    int64_t RelatedClosePrice;
    int64_t RelatedTradeQuantity;
    int64_t UnderlyingQty;
    int32_t MarketSegmentID;
    uint32_t PackageID;
    uint32_t TESExecID;
    uint32_t UnderlyingSettlementDate;
    uint32_t UnderlyingMaturityDate;
    uint32_t RelatedTradeID;
    int32_t RelatedMarketSegmentID;
    uint32_t NegotiationID;
    uint8_t NoSRQSRelatedTradeIDs;
    char Pad1[LEN_PAD1];
    uint16_t TrdType;
    uint8_t TradeReportType;
    uint8_t ProductComplex;
    uint8_t TradePublishIndicator;
    uint8_t NoEvents;
    uint8_t NoInstrAttrib;
    uint8_t NoUnderlyingStips;
    uint8_t NoSideAllocs;
    uint8_t NoLegs;
    uint8_t PartyIDSettlementLocation;
    uint8_t HedgeType;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    char TradeReportText[LEN_TRADE_REPORT_TEXT];
    char TradeReportID[LEN_TRADE_REPORTID];
    char RootPartyExecutingFirm[LEN_ROOT_PARTY_EXECUTING_FIRM];
    char RootPartyExecutingTrader[LEN_ROOT_PARTY_EXECUTING_TRADER];
    char UnderlyingSecurityID[LEN_UNDERLYING_SECURITYID];
    char UnderlyingSecurityDesc[LEN_UNDERLYING_SECURITY_DESC];
    char UnderlyingCurrency[LEN_UNDERLYING_CURRENCY];
    char UnderlyingIssuer[LEN_UNDERLYING_ISSUER];
    char Pad3[LEN_PAD3];
    SideAllocGrpBCSeqT SideAllocGrpBC[MAX_TES_BROADCAST_SIDE_ALLOC_GRPBC];
    TrdInstrmntLegGrpSeqT TrdInstrmntLegGrp[MAX_TES_BROADCAST_TRD_INSTRMNT_LEG_GRP];
    InstrumentEventGrpSeqT InstrumentEventGrp[MAX_TES_BROADCAST_INSTRUMENT_EVENT_GRP];
    InstrumentAttributeGrpSeqT InstrumentAttributeGrp[MAX_TES_BROADCAST_INSTRUMENT_ATTRIBUTE_GRP];
    UnderlyingStipGrpSeqT UnderlyingStipGrp[MAX_TES_BROADCAST_UNDERLYING_STIP_GRP];
    SRQSRelatedTradeIDGrpSeqT SRQSRelatedTradeIDGrp[MAX_TES_BROADCAST_SRQS_RELATED_TRADE_ID_GRP];
} TESBroadcastT;

// Message:	    TESDeleteBroadcast
// TemplateID:  10606
// Alias:       Delete TES Trade Broadcast
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    int32_t MarketSegmentID;
    uint32_t PackageID;
    uint32_t TESExecID;
    uint16_t TrdType;
    uint8_t DeleteReason;
    uint8_t TradeReportType;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    char TradeReportID[LEN_TRADE_REPORTID];
    char Pad3[LEN_PAD3];
} TESDeleteBroadcastT;

// Message:	    TESExecutionBroadcast
// TemplateID:  10610
// Alias:       TES Execution Broadcast
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint64_t TransactTime;
    int32_t MarketSegmentID;
    uint32_t PackageID;
    uint32_t TESExecID;
    uint32_t AllocID;
    uint16_t TrdType;
    uint8_t TradeReportType;
    uint8_t Side;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    char Pad3[LEN_PAD3];
} TESExecutionBroadcastT;

// Message:	    TESResponse
// TemplateID:  10611
// Alias:       TES Response
// FIX MsgType: TradeCaptureReportAck = "AR"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
    uint32_t TESExecID;
    char TradeReportID[LEN_TRADE_REPORTID];
} TESResponseT;

// Message:	    TESTradeBroadcast
// TemplateID:  10614
// Alias:       TES Trade Broadcast
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    int64_t SecurityID;
    int64_t LastPx;
    int64_t LastQty;
    int64_t ClearingTradePrice;
    int64_t ClearingTradeQty;
    uint64_t TransactTime;
    int64_t RelatedSecurityID;
    uint64_t RootPartyIDClientID;
    uint64_t ExecutingTrader;
    uint64_t RootPartyIDInvestmentDecisionMaker;
    uint64_t BasketTrdMatchID;
    uint32_t PackageID;
    int32_t MarketSegmentID;
    uint32_t TradeID;
    uint32_t TradeDate;
    uint32_t SideTradeID;
    uint32_t RootPartyIDSessionID;
    uint32_t OrigTradeID;
    uint32_t RootPartyIDExecutingUnit;
    uint32_t RootPartyIDExecutingTrader;
    uint32_t RootPartyIDClearingUnit;
    uint32_t StrategyLinkID;
    int32_t RelatedSymbol;
    int32_t TotNumTradeReports;
    uint32_t NegotiationID;
    uint8_t NoSRQSRelatedTradeIDs;
    char Pad1[LEN_PAD1];
    uint16_t TrdType;
    uint8_t ProductComplex;
    uint8_t RelatedProductComplex;
    uint8_t Side;
    uint8_t TradingCapacity;
    uint8_t TradeReportType;
    uint8_t QuoteQualifier;
    uint8_t TransferReason;
    uint8_t TradePublishIndicator;
    uint8_t MultiLegReportingType;
    char PositionEffect[LEN_POSITION_EFFECT];
    uint8_t MultilegPriceModel;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t OrderAttributeRiskReduction;
    uint8_t ExecutingTraderQualifier;
    uint8_t RootPartyIDInvestmentDecisionMakerQualifier;
    char Account[LEN_ACCOUNT];
    char RootPartyIDPositionAccount[LEN_ROOT_PARTY_ID_POSITION_ACCOUNT];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char RootPartyExecutingFirm[LEN_ROOT_PARTY_EXECUTING_FIRM];
    char RootPartyExecutingTrader[LEN_ROOT_PARTY_EXECUTING_TRADER];
    char RootPartyClearingFirm[LEN_ROOT_PARTY_CLEARING_FIRM];
    char RootPartyClearingOrganization[LEN_ROOT_PARTY_CLEARING_ORGANIZATION];
    char RootPartyIDBeneficiary[LEN_ROOT_PARTY_ID_BENEFICIARY];
    char RootPartyIDTakeUpTradingFirm[LEN_ROOT_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char RootPartyIDOrderOriginationFirm[LEN_ROOT_PARTY_ID_ORDER_ORIGINATION_FIRM];
    char RootPartyIDExecutionVenue[LEN_ROOT_PARTY_ID_EXECUTION_VENUE];
    char RegulatoryTradeID[LEN_REGULATORY_TRADEID];
    char BasketSideTradeReportID[LEN_BASKET_SIDE_TRADE_REPORTID];
    char Pad1_1[LEN_PAD1_1];
    SRQSRelatedTradeIDGrpSeqT SRQSRelatedTradeIDGrp[MAX_TES_TRADE_BROADCAST_SRQS_RELATED_TRADE_ID_GRP];
} TESTradeBroadcastT;

// Message:	    TESTradingSessionStatusBroadcast
// TemplateID:  10615
// Alias:       TES Status Broadcast
// FIX MsgType: TradingSessionStatus = "h"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint32_t TradeDate;
    uint8_t TradSesEvent;
    char Pad3[LEN_PAD3];
} TESTradingSessionStatusBroadcastT;

// Message:	    TESUploadBroadcast
// TemplateID:  10613
// Alias:       TES Trade Upload Broadcast
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    int64_t SecurityID;
    int64_t LastPx;
    uint64_t TransBkdTime;
    uint64_t TransactTime;
    int64_t UnderlyingPx;
    int64_t RelatedClosePrice;
    int64_t RelatedTradeQuantity;
    int64_t UnderlyingQty;
    int32_t MarketSegmentID;
    uint32_t PackageID;
    uint32_t TESExecID;
    uint32_t UnderlyingSettlementDate;
    uint32_t UnderlyingMaturityDate;
    uint32_t RelatedTradeID;
    int32_t RelatedMarketSegmentID;
    uint32_t NegotiationID;
    uint8_t NoSRQSRelatedTradeIDs;
    char Pad1[LEN_PAD1];
    uint16_t TrdType;
    uint8_t ProductComplex;
    uint8_t TradeReportType;
    uint8_t TradePublishIndicator;
    uint8_t TradePlatform;
    uint8_t NoSideAllocs;
    uint8_t NoLegs;
    uint8_t NoEvents;
    uint8_t NoInstrAttrib;
    uint8_t NoUnderlyingStips;
    uint8_t HedgeType;
    uint8_t PartyIDSettlementLocation;
    char MessageEventSource[LEN_MESSAGE_EVENT_SOURCE];
    char TradeReportID[LEN_TRADE_REPORTID];
    char RootPartyExecutingFirm[LEN_ROOT_PARTY_EXECUTING_FIRM];
    char RootPartyExecutingTrader[LEN_ROOT_PARTY_EXECUTING_TRADER];
    char UnderlyingSecurityID[LEN_UNDERLYING_SECURITYID];
    char UnderlyingSecurityDesc[LEN_UNDERLYING_SECURITY_DESC];
    char UnderlyingCurrency[LEN_UNDERLYING_CURRENCY];
    char UnderlyingIssuer[LEN_UNDERLYING_ISSUER];
    char Pad6[LEN_PAD6];
    SideAllocExtGrpSeqT SideAllocExtGrp[MAX_TES_UPLOAD_BROADCAST_SIDE_ALLOC_EXT_GRP];
    TrdInstrmntLegGrpSeqT TrdInstrmntLegGrp[MAX_TES_UPLOAD_BROADCAST_TRD_INSTRMNT_LEG_GRP];
    InstrumentEventGrpSeqT InstrumentEventGrp[MAX_TES_UPLOAD_BROADCAST_INSTRUMENT_EVENT_GRP];
    InstrumentAttributeGrpSeqT InstrumentAttributeGrp[MAX_TES_UPLOAD_BROADCAST_INSTRUMENT_ATTRIBUTE_GRP];
    UnderlyingStipGrpSeqT UnderlyingStipGrp[MAX_TES_UPLOAD_BROADCAST_UNDERLYING_STIP_GRP];
    SRQSRelatedTradeIDGrpSeqT SRQSRelatedTradeIDGrp[MAX_TES_UPLOAD_BROADCAST_SRQS_RELATED_TRADE_ID_GRP];
} TESUploadBroadcastT;

// Message:	    TMTradingSessionStatusBroadcast
// TemplateID:  10501
// Alias:       Trade Notification Status
// FIX MsgType: TradingSessionStatus = "h"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    uint8_t TradSesEvent;
    char Pad7[LEN_PAD7];
} TMTradingSessionStatusBroadcastT;

// Message:	    ThrottleUpdateNotification
// TemplateID:  10028
// Alias:       Throttle Update Notification
// FIX MsgType: UserNotification = "CB"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    NotifHeaderCompT NotifHeader;
    int64_t ThrottleTimeInterval;
    uint32_t ThrottleNoMsgs;
    uint32_t ThrottleDisconnectLimit;
} ThrottleUpdateNotificationT;

// Message:	    TradeBroadcast
// TemplateID:  10500
// Alias:       Trade Notification
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderCompT RBCHeader;
    int64_t SecurityID;
    int64_t RelatedSecurityID;
    int64_t Price;
    int64_t LastPx;
    int64_t LastQty;
    int64_t SideLastPx;
    int64_t SideLastQty;
    int64_t ClearingTradePrice;
    int64_t ClearingTradeQty;
    uint64_t TransactTime;
    uint64_t OrderID;
    uint64_t ClOrdID;
    int64_t LeavesQty;
    int64_t CumQty;
    uint64_t RootPartyIDClientID;
    uint64_t ExecutingTrader;
    uint64_t RootPartyIDInvestmentDecisionMaker;
    uint32_t TradeID;
    uint32_t OrigTradeID;
    uint32_t RootPartyIDExecutingUnit;
    uint32_t RootPartyIDSessionID;
    uint32_t RootPartyIDExecutingTrader;
    uint32_t RootPartyIDClearingUnit;
    int32_t MarketSegmentID;
    int32_t RelatedSymbol;
    uint32_t SideTradeID;
    uint32_t MatchDate;
    uint32_t TrdMatchID;
    uint32_t StrategyLinkID;
    int32_t TotNumTradeReports;
    uint8_t MultiLegReportingType;
    uint8_t TradeReportType;
    uint8_t TransferReason;
    char RootPartyIDBeneficiary[LEN_ROOT_PARTY_ID_BENEFICIARY];
    char RootPartyIDTakeUpTradingFirm[LEN_ROOT_PARTY_ID_TAKE_UP_TRADING_FIRM];
    char RootPartyIDOrderOriginationFirm[LEN_ROOT_PARTY_ID_ORDER_ORIGINATION_FIRM];
    uint8_t MatchType;
    uint8_t MatchSubType;
    uint8_t Side;
    uint8_t SideLiquidityInd;
    uint8_t TradingCapacity;
    uint8_t OrderOrigination;
    uint8_t OrderAttributeLiquidityProvision;
    uint8_t OrderAttributeRiskReduction;
    uint8_t ExecutingTraderQualifier;
    uint8_t RootPartyIDInvestmentDecisionMakerQualifier;
    char Account[LEN_ACCOUNT];
    char RootPartyIDPositionAccount[LEN_ROOT_PARTY_ID_POSITION_ACCOUNT];
    char PositionEffect[LEN_POSITION_EFFECT];
    char CustOrderHandlingInst[LEN_CUST_ORDER_HANDLING_INST];
    char FreeText1[LEN_FREE_TEXT1];
    char FreeText2[LEN_FREE_TEXT2];
    char FreeText3[LEN_FREE_TEXT3];
    char OrderCategory[LEN_ORDER_CATEGORY];
    uint8_t OrdType;
    uint8_t RelatedProductComplex;
    uint8_t OrderSide;
    char RootPartyClearingOrganization[LEN_ROOT_PARTY_CLEARING_ORGANIZATION];
    char RootPartyExecutingFirm[LEN_ROOT_PARTY_EXECUTING_FIRM];
    char RootPartyExecutingTrader[LEN_ROOT_PARTY_EXECUTING_TRADER];
    char RootPartyClearingFirm[LEN_ROOT_PARTY_CLEARING_FIRM];
    char RegulatoryTradeID[LEN_REGULATORY_TRADEID];
    char RootPartyIDExecutionVenue[LEN_ROOT_PARTY_ID_EXECUTION_VENUE];
    char Pad2[LEN_PAD2];
} TradeBroadcastT;

// Message:	    TradingSessionStatusBroadcast
// TemplateID:  10307
// Alias:       Trading Session Event
// FIX MsgType: TradingSessionStatus = "h"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    RBCHeaderMECompT RBCHeaderME;
    int32_t MarketSegmentID;
    uint32_t TradeDate;
    uint8_t TradSesEvent;
    char RefApplLastMsgID[LEN_REF_APPL_LAST_MSGID];
    char Pad7[LEN_PAD7];
} TradingSessionStatusBroadcastT;

// Message:	    UnsubscribeRequest
// TemplateID:  10006
// Alias:       Unsubscribe
// FIX MsgType: ApplicationMessageRequest = "BW"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint32_t RefApplSubID;
    char Pad4[LEN_PAD4];
} UnsubscribeRequestT;

// Message:	    UnsubscribeResponse
// TemplateID:  10007
// Alias:       Unsubscribe Response
// FIX MsgType: ApplicationMessageRequestAck = "BX"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
} UnsubscribeResponseT;

// Message:	    UploadTESTradeRequest
// TemplateID:  10612
// Alias:       Upload TES Trade Request
// FIX MsgType: TradeCaptureReport = "AE"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    int64_t SecurityID;
    int64_t LastPx;
    uint64_t TransBkdTime;
    int64_t UnderlyingPx;
    int64_t RelatedClosePrice;
    int64_t RelatedTradeQuantity;
    int64_t UnderlyingQty;
    int32_t MarketSegmentID;
    uint32_t UnderlyingSettlementDate;
    uint32_t UnderlyingMaturityDate;
    uint32_t RelatedTradeID;
    int32_t RelatedMarketSegmentID;
    uint32_t NegotiationID;
    uint8_t NoSRQSRelatedTradeIDs;
    char Pad1[LEN_PAD1];
    uint16_t TrdType;
    uint8_t ProductComplex;
    uint8_t TradeReportType;
    uint8_t TradePublishIndicator;
    uint8_t NoSideAllocs;
    uint8_t NoLegs;
    uint8_t NoEvents;
    uint8_t NoInstrAttrib;
    uint8_t NoUnderlyingStips;
    uint8_t SkipValidations;
    uint8_t TrdRptStatus;
    uint8_t TradePlatform;
    uint8_t HedgeType;
    uint8_t PartyIDSettlementLocation;
    uint8_t ValueCheckTypeMinLotSize;
    char TradeReportID[LEN_TRADE_REPORTID];
    char TradeReportText[LEN_TRADE_REPORT_TEXT];
    char UnderlyingSecurityID[LEN_UNDERLYING_SECURITYID];
    char UnderlyingSecurityDesc[LEN_UNDERLYING_SECURITY_DESC];
    char UnderlyingCurrency[LEN_UNDERLYING_CURRENCY];
    char UnderlyingIssuer[LEN_UNDERLYING_ISSUER];
    char Pad3[LEN_PAD3];
    SideAllocExtGrpSeqT SideAllocExtGrp[MAX_UPLOAD_TES_TRADE_REQUEST_SIDE_ALLOC_EXT_GRP];
    TrdInstrmntLegGrpSeqT TrdInstrmntLegGrp[MAX_UPLOAD_TES_TRADE_REQUEST_TRD_INSTRMNT_LEG_GRP];
    InstrumentEventGrpSeqT InstrumentEventGrp[MAX_UPLOAD_TES_TRADE_REQUEST_INSTRUMENT_EVENT_GRP];
    InstrumentAttributeGrpSeqT InstrumentAttributeGrp[MAX_UPLOAD_TES_TRADE_REQUEST_INSTRUMENT_ATTRIBUTE_GRP];
    UnderlyingStipGrpSeqT UnderlyingStipGrp[MAX_UPLOAD_TES_TRADE_REQUEST_UNDERLYING_STIP_GRP];
    SRQSRelatedTradeIDGrpSeqT SRQSRelatedTradeIDGrp[MAX_UPLOAD_TES_TRADE_REQUEST_SRQS_RELATED_TRADE_ID_GRP];
} UploadTESTradeRequestT;

// Message:	    UserLoginRequest
// TemplateID:  10018
// Alias:       User Logon
// FIX MsgType: UserRequest = "BE"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint32_t Username;
    char Password[LEN_PASSWORD];
    char Pad4[LEN_PAD4];
} UserLoginRequestT;

// Message:	    UserLoginResponse
// TemplateID:  10019
// Alias:       User Logon Response
// FIX MsgType: UserResponse = "BF"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
} UserLoginResponseT;

// Message:	    UserLogoutRequest
// TemplateID:  10029
// Alias:       User Logout
// FIX MsgType: UserRequest = "BE"
typedef struct
{
    MessageHeaderInCompT MessageHeaderIn;
    RequestHeaderCompT RequestHeader;
    uint32_t Username;
    char Pad4[LEN_PAD4];
} UserLogoutRequestT;

// Message:	    UserLogoutResponse
// TemplateID:  10024
// Alias:       User Logout Response
// FIX MsgType: UserResponse = "BF"
typedef struct
{
    MessageHeaderOutCompT MessageHeaderOut;
    ResponseHeaderCompT ResponseHeader;
} UserLogoutResponseT;

/*
 * Begin of DEPRECATED defines
 */

#define BYTE_ARRAY_OF_0_16 {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}

#define	TID_ADDCOMPLEXINSTRUMENTREQUEST                  10301		// < AddComplexInstrumentRequest (Create Strategy)
#define	TID_ADDCOMPLEXINSTRUMENTRESPONSE                 10302		// < AddComplexInstrumentResponse (Create Strategy Response)
#define	TID_ADDFLEXIBLEINSTRUMENTREQUEST                 10309		// < AddFlexibleInstrumentRequest (Create Flexible Instrument Request)
#define	TID_ADDFLEXIBLEINSTRUMENTRESPONSE                10310		// < AddFlexibleInstrumentResponse (Create Flexible Instrument Response)
#define	TID_AMENDBASKETTRADEREQUEST                      10629		// < AmendBasketTradeRequest (Amend Basket Trade Request)
#define	TID_APPROVEBASKETTRADEREQUEST                    10623		// < ApproveBasketTradeRequest (Approve Basket Trade Request)
#define	TID_APPROVETESTRADEREQUEST                       10603		// < ApproveTESTradeRequest (Approve TES Trade Request)
#define	TID_BASKETAPPROVEBROADCAST                       10627		// < BasketApproveBroadcast (Approve Basket Broadcast)
#define	TID_BASKETBROADCAST                              10625		// < BasketBroadcast (Basket Trading Broadcast)
#define	TID_BASKETDELETEBROADCAST                        10626		// < BasketDeleteBroadcast (Delete Basket Broadcast)
#define	TID_BASKETEXECUTIONBROADCAST                     10628		// < BasketExecutionBroadcast (Basket Execution Broadcast)
#define	TID_BASKETRESPONSE                               10624		// < BasketResponse (Basket Trading Response)
#define	TID_BROADCASTERRORNOTIFICATION                   10032		// < BroadcastErrorNotification (Gap Fill)
#define	TID_CLIPDELETIONNOTIFICATION                     10134		// < CLIPDeletionNotification (CLIP Deletion Notification)
#define	TID_CLIPEXECUTIONNOTIFICATION                    10135		// < CLIPExecutionNotification (CLIP Execution Notification)
#define	TID_CLIPRESPONSE                                 10133		// < CLIPResponse (Response to a CLIP Request)
#define	TID_CROSSREQUEST                                 10118		// < CrossRequest (Cross Request)
#define	TID_CROSSREQUESTRESPONSE                         10119		// < CrossRequestResponse (Cross Request Response)
#define	TID_DELETEALLORDERBROADCAST                      10122		// < DeleteAllOrderBroadcast (Order Mass Cancellation Notification)
#define	TID_DELETEALLORDERNRRESPONSE                     10124		// < DeleteAllOrderNRResponse (Order Mass Cancellation Response No Hits)
#define	TID_DELETEALLORDERQUOTEEVENTBROADCAST            10308		// < DeleteAllOrderQuoteEventBroadcast (Mass Cancellation Event)
#define	TID_DELETEALLORDERREQUEST                        10120		// < DeleteAllOrderRequest (Order Mass Cancellation Request)
#define	TID_DELETEALLORDERRESPONSE                       10121		// < DeleteAllOrderResponse (Order Mass Cancellation Response)
#define	TID_DELETEALLQUOTEBROADCAST                      10410		// < DeleteAllQuoteBroadcast (Quote Mass Cancellation Notification)
#define	TID_DELETEALLQUOTEREQUEST                        10408		// < DeleteAllQuoteRequest (Quote Mass Cancellation Request)
#define	TID_DELETEALLQUOTERESPONSE                       10409		// < DeleteAllQuoteResponse (Quote Mass Cancellation Response)
#define	TID_DELETEBASKETTRADEREQUEST                     10622		// < DeleteBasketTradeRequest (Delete Basket Trade Request)
#define	TID_DELETECLIPREQUEST                            10132		// < DeleteCLIPRequest (CLIP Delete Request)
#define	TID_DELETEORDERBROADCAST                         10112		// < DeleteOrderBroadcast (Cancel Order Notification)
#define	TID_DELETEORDERCOMPLEXREQUEST                    10123		// < DeleteOrderComplexRequest (Cancel Order Multi Leg)
#define	TID_DELETEORDERNRRESPONSE                        10111		// < DeleteOrderNRResponse (Cancel Order Response (Lean Order))
#define	TID_DELETEORDERRESPONSE                          10110		// < DeleteOrderResponse (Cancel Order Response (Standard Order))
#define	TID_DELETEORDERSINGLEREQUEST                     10109		// < DeleteOrderSingleRequest (Cancel Order Single)
#define	TID_DELETETESTRADEREQUEST                        10602		// < DeleteTESTradeRequest (Delete TES Trade Request)
#define	TID_ENTERBASKETTRADEREQUEST                      10620		// < EnterBasketTradeRequest (Enter Basket Trade Request)
#define	TID_ENTERCLIPREQUEST                             10131		// < EnterCLIPRequest (CLIP Enter Request)
#define	TID_ENTERTESTRADEREQUEST                         10600		// < EnterTESTradeRequest (Enter TES Trade Request)
#define	TID_FORCEDLOGOUTNOTIFICATION                     10012		// < ForcedLogoutNotification (Session Logout Notification)
#define	TID_FORCEDUSERLOGOUTNOTIFICATION                 10043		// < ForcedUserLogoutNotification (User Logout Notification)
#define	TID_HEARTBEAT                                    10011		// < Heartbeat (Heartbeat)
#define	TID_HEARTBEATNOTIFICATION                        10023		// < HeartbeatNotification (Heartbeat Notification)
#define	TID_INQUIREENRICHMENTRULEIDLISTREQUEST           10040		// < InquireEnrichmentRuleIDListRequest (Trade Enrichment List Inquire)
#define	TID_INQUIREENRICHMENTRULEIDLISTRESPONSE          10041		// < InquireEnrichmentRuleIDListResponse (Trade Enrichment List Inquire Response)
#define	TID_INQUIREMMPARAMETERREQUEST                    10305		// < InquireMMParameterRequest (Inquire Market Maker Parameters)
#define	TID_INQUIREMMPARAMETERRESPONSE                   10306		// < InquireMMParameterResponse (Inquire Market Maker Parameters Response)
#define	TID_INQUIREPRETRADERISKLIMITSREQUEST             10311		// < InquirePreTradeRiskLimitsRequest (Inquire Pre-Trade Risk Limits Request)
#define	TID_INQUIRESESSIONLISTREQUEST                    10035		// < InquireSessionListRequest (Session List Inquire)
#define	TID_INQUIRESESSIONLISTRESPONSE                   10036		// < InquireSessionListResponse (Session List Inquire Response)
#define	TID_INQUIREUSERREQUEST                           10038		// < InquireUserRequest (User List Inquire)
#define	TID_INQUIREUSERRESPONSE                          10039		// < InquireUserResponse (User List Inquire Response)
#define	TID_LEGALNOTIFICATIONBROADCAST                   10037		// < LegalNotificationBroadcast (Legal Notification)
#define	TID_LOGONREQUEST                                 10000		// < LogonRequest (Session Logon)
#define	TID_LOGONRESPONSE                                10001		// < LogonResponse (Session Logon Response)
#define	TID_LOGOUTREQUEST                                10002		// < LogoutRequest (Session Logout)
#define	TID_LOGOUTRESPONSE                               10003		// < LogoutResponse (Session Logout Response)
#define	TID_MMPARAMETERDEFINITIONREQUEST                 10303		// < MMParameterDefinitionRequest (Market Maker Parameter Definition)
#define	TID_MMPARAMETERDEFINITIONRESPONSE                10304		// < MMParameterDefinitionResponse (Market Maker Parameter Definition Response)
#define	TID_MASSQUOTEREQUEST                             10405		// < MassQuoteRequest (Mass Quote)
#define	TID_MASSQUOTERESPONSE                            10406		// < MassQuoteResponse (Mass Quote Response)
#define	TID_MODIFYBASKETTRADEREQUEST                     10621		// < ModifyBasketTradeRequest (Modify Basket Trade Request)
#define	TID_MODIFYORDERCOMPLEXREQUEST                    10114		// < ModifyOrderComplexRequest (Replace Order Multi Leg)
#define	TID_MODIFYORDERCOMPLEXSHORTREQUEST               10130		// < ModifyOrderComplexShortRequest (Replace Order Multi Leg (short layout))
#define	TID_MODIFYORDERNRRESPONSE                        10108		// < ModifyOrderNRResponse (Replace Order Response (Lean Order))
#define	TID_MODIFYORDERRESPONSE                          10107		// < ModifyOrderResponse (Replace Order Response (Standard Order))
#define	TID_MODIFYORDERSINGLEREQUEST                     10106		// < ModifyOrderSingleRequest (Replace Order Single)
#define	TID_MODIFYORDERSINGLESHORTREQUEST                10126		// < ModifyOrderSingleShortRequest (Replace Order Single (short layout))
#define	TID_MODIFYTESTRADEREQUEST                        10601		// < ModifyTESTradeRequest (Modify TES Trade Request)
#define	TID_NEWORDERCOMPLEXREQUEST                       10113		// < NewOrderComplexRequest (New Order Multi Leg)
#define	TID_NEWORDERCOMPLEXSHORTREQUEST                  10129		// < NewOrderComplexShortRequest (New Order Multi Leg (short layout))
#define	TID_NEWORDERNRRESPONSE                           10102		// < NewOrderNRResponse (New Order Response (Lean Order))
#define	TID_NEWORDERRESPONSE                             10101		// < NewOrderResponse (New Order Response (Standard Order))
#define	TID_NEWORDERSINGLEREQUEST                        10100		// < NewOrderSingleRequest (New Order Single)
#define	TID_NEWORDERSINGLESHORTREQUEST                   10125		// < NewOrderSingleShortRequest (New Order Single (short layout))
#define	TID_NEWSBROADCAST                                10031		// < NewsBroadcast (News)
#define	TID_ORDEREXECNOTIFICATION                        10104		// < OrderExecNotification (Book Order Execution)
#define	TID_ORDEREXECREPORTBROADCAST                     10117		// < OrderExecReportBroadcast (Extended Order Information)
#define	TID_ORDEREXECRESPONSE                            10103		// < OrderExecResponse (Immediate Execution Response)
#define	TID_PARTYACTIONREPORT                            10042		// < PartyActionReport (Party Action Report)
#define	TID_PARTYENTITLEMENTSUPDATEREPORT                10034		// < PartyEntitlementsUpdateReport (Entitlement Notification)
#define	TID_PRETRADERISKLIMITRESPONSE                    10313		// < PreTradeRiskLimitResponse (Pre-Trade Risk Limit Response)
#define	TID_PRETRADERISKLIMITSDEFINITIONREQUEST          10312		// < PreTradeRiskLimitsDefinitionRequest (Pre-Trade Risk Limits Definition Request)
#define	TID_QUOTEACTIVATIONNOTIFICATION                  10411		// < QuoteActivationNotification (Quote Activation Notification)
#define	TID_QUOTEACTIVATIONREQUEST                       10403		// < QuoteActivationRequest (Quote Activation Request)
#define	TID_QUOTEACTIVATIONRESPONSE                      10404		// < QuoteActivationResponse (Quote Activation Response)
#define	TID_QUOTEEXECUTIONREPORT                         10407		// < QuoteExecutionReport (Quote Execution Notification)
#define	TID_RFQREQUEST                                   10401		// < RFQRequest (Quote Request)
#define	TID_RFQRESPONSE                                  10402		// < RFQResponse (Quote Request Response)
#define	TID_REJECT                                       10010		// < Reject (Reject)
#define	TID_RETRANSMITMEMESSAGEREQUEST                   10026		// < RetransmitMEMessageRequest (Retransmit (Order/Quote Event))
#define	TID_RETRANSMITMEMESSAGERESPONSE                  10027		// < RetransmitMEMessageResponse (Retransmit Response (Order/Quote Event))
#define	TID_RETRANSMITREQUEST                            10008		// < RetransmitRequest (Retransmit)
#define	TID_RETRANSMITRESPONSE                           10009		// < RetransmitResponse (Retransmit Response)
#define	TID_RISKNOTIFICATIONBROADCAST                    10033		// < RiskNotificationBroadcast (Risk Notification)
#define	TID_SRQSCREATEDEALNOTIFICATION                   10708		// < SRQSCreateDealNotification (SRQS Create Deal Notification)
#define	TID_SRQSDEALNOTIFICATION                         10709		// < SRQSDealNotification (SRQS Deal Notification)
#define	TID_SRQSDEALRESPONSE                             10705		// < SRQSDealResponse (SRQS Deal Response)
#define	TID_SRQSENTERQUOTEREQUEST                        10702		// < SRQSEnterQuoteRequest (SRQS Enter Quote Request)
#define	TID_SRQSHITQUOTEREQUEST                          10704		// < SRQSHitQuoteRequest (SRQS Hit Quote Request)
#define	TID_SRQSNEGOTIATIONNOTIFICATION                  10713		// < SRQSNegotiationNotification (SRQS Negotiation Notification for Respondent)
#define	TID_SRQSNEGOTIATIONREQUESTERNOTIFICATION         10712		// < SRQSNegotiationRequesterNotification (SRQS Negotiation Notification for Requester)
#define	TID_SRQSNEGOTIATIONSTATUSNOTIFICATION            10715		// < SRQSNegotiationStatusNotification (SRQS Negotiation Status Notification)
#define	TID_SRQSOPENNEGOTIATIONNOTIFICATION              10711		// < SRQSOpenNegotiationNotification (SRQS Open Negotiation Notification for Respondent)
#define	TID_SRQSOPENNEGOTIATIONREQUEST                   10700		// < SRQSOpenNegotiationRequest (SRQS Open Negotiation Request)
#define	TID_SRQSOPENNEGOTIATIONREQUESTERNOTIFICATION     10710		// < SRQSOpenNegotiationRequesterNotification (SRQS Open Negotiation Notification for Requester)
#define	TID_SRQSQUOTENOTIFICATION                        10707		// < SRQSQuoteNotification (SRQS Quote Notification for Responder)
#define	TID_SRQSQUOTERESPONSE                            10703		// < SRQSQuoteResponse (SRQS Quote Response)
#define	TID_SRQSQUOTINGSTATUSREQUEST                     10717		// < SRQSQuotingStatusRequest (SRQS Enter Quoting Status Request)
#define	TID_SRQSSTATUSBROADCAST                          10714		// < SRQSStatusBroadcast (SRQS Status Notification)
#define	TID_SRQSUPDATEDEALSTATUSREQUEST                  10706		// < SRQSUpdateDealStatusRequest (SRQS Update Deal Request)
#define	TID_SRQSUPDATENEGOTIATIONREQUEST                 10701		// < SRQSUpdateNegotiationRequest (SRQS Update Negotiation Request)
#define	TID_SERVICEAVAILABILITYBROADCAST                 10030		// < ServiceAvailabilityBroadcast (Service Availability)
#define	TID_SERVICEAVAILABILITYMARKETBROADCAST           10044		// < ServiceAvailabilityMarketBroadcast (Service Availability Market)
#define	TID_SUBSCRIBEREQUEST                             10025		// < SubscribeRequest (Subscribe)
#define	TID_SUBSCRIBERESPONSE                            10005		// < SubscribeResponse (Subscribe Response)
#define	TID_TESAPPROVEBROADCAST                          10607		// < TESApproveBroadcast (Approve TES Trade Broadcast)
#define	TID_TESBROADCAST                                 10604		// < TESBroadcast (TES Broadcast)
#define	TID_TESDELETEBROADCAST                           10606		// < TESDeleteBroadcast (Delete TES Trade Broadcast)
#define	TID_TESEXECUTIONBROADCAST                        10610		// < TESExecutionBroadcast (TES Execution Broadcast)
#define	TID_TESRESPONSE                                  10611		// < TESResponse (TES Response)
#define	TID_TESTRADEBROADCAST                            10614		// < TESTradeBroadcast (TES Trade Broadcast)
#define	TID_TESTRADINGSESSIONSTATUSBROADCAST             10615		// < TESTradingSessionStatusBroadcast (TES Status Broadcast )
#define	TID_TESUPLOADBROADCAST                           10613		// < TESUploadBroadcast (TES Trade Upload Broadcast )
#define	TID_TMTRADINGSESSIONSTATUSBROADCAST              10501		// < TMTradingSessionStatusBroadcast (Trade Notification Status)
#define	TID_THROTTLEUPDATENOTIFICATION                   10028		// < ThrottleUpdateNotification (Throttle Update Notification)
#define	TID_TRADEBROADCAST                               10500		// < TradeBroadcast (Trade Notification)
#define	TID_TRADINGSESSIONSTATUSBROADCAST                10307		// < TradingSessionStatusBroadcast (Trading Session Event)
#define	TID_UNSUBSCRIBEREQUEST                           10006		// < UnsubscribeRequest (Unsubscribe)
#define	TID_UNSUBSCRIBERESPONSE                          10007		// < UnsubscribeResponse (Unsubscribe Response)
#define	TID_UPLOADTESTRADEREQUEST                        10612		// < UploadTESTradeRequest (Upload TES Trade Request)
#define	TID_USERLOGINREQUEST                             10018		// < UserLoginRequest (User Logon)
#define	TID_USERLOGINRESPONSE                            10019		// < UserLoginResponse (User Logon Response)
#define	TID_USERLOGOUTREQUEST                            10029		// < UserLogoutRequest (User Logout)
#define	TID_USERLOGOUTRESPONSE                           10024		// < UserLogoutResponse (User Logout Response)

#define MAX_ADDCOMPLEXINSTRUMENTREQUEST_INSTRMTLEGGRP   	20
#define MAX_ADDCOMPLEXINSTRUMENTRESPONSE_INSTRMTLEGGRP  	20
#define MAX_AMENDBASKETTRADEREQUEST_BASKETROOTPARTYGRP  	2
#define MAX_AMENDBASKETTRADEREQUEST_INSTRMTMATCHSIDEGRP 	99
#define MAX_AMENDBASKETTRADEREQUEST_BASKETSIDEALLOCGRP  	198
#define MAX_APPROVEBASKETTRADEREQUEST_BASKETSIDEALLOCEXTGRP	198
#define MAX_BASKETAPPROVEBROADCAST_BASKETROOTPARTYGRP   	2
#define MAX_BASKETAPPROVEBROADCAST_BASKETSIDEALLOCEXTBCGRP	198
#define MAX_BASKETBROADCAST_BASKETROOTPARTYGRP          	3
#define MAX_BASKETBROADCAST_INSTRMTMATCHSIDEGRP         	99
#define MAX_BASKETBROADCAST_BASKETSIDEALLOCGRP          	198
#define MAX_BASKETEXECUTIONBROADCAST_BASKETEXECGRP      	99
#define MAX_CLIPEXECUTIONNOTIFICATION_FILLSGRP          	100
#define MAX_CLIPEXECUTIONNOTIFICATION_INSTRMNTLEGEXECGRP	600
#define MAX_CLIPRESPONSE_CROSSREQUESTACKSIDEGRP         	2
#define MAX_DELETEALLORDERBROADCAST_NOTAFFECTEDORDERSGRP	500
#define MAX_DELETEALLORDERBROADCAST_AFFECTEDORDGRP      	500
#define MAX_DELETEALLORDERBROADCAST_AFFECTEDORDERREQUESTSGRP	500
#define MAX_DELETEALLORDERRESPONSE_NOTAFFECTEDORDERSGRP 	500
#define MAX_DELETEALLORDERRESPONSE_AFFECTEDORDGRP       	500
#define MAX_DELETEALLORDERRESPONSE_AFFECTEDORDERREQUESTSGRP	500
#define MAX_DELETEALLQUOTEBROADCAST_NOTAFFECTEDSECURITIESGRP	500
#define MAX_DELETEALLQUOTERESPONSE_NOTAFFECTEDSECURITIESGRP	500
#define MAX_ENTERBASKETTRADEREQUEST_BASKETROOTPARTYGRP  	2
#define MAX_ENTERBASKETTRADEREQUEST_INSTRMTMATCHSIDEGRP 	99
#define MAX_ENTERBASKETTRADEREQUEST_BASKETSIDEALLOCGRP  	198
#define MAX_ENTERCLIPREQUEST_CROSSREQUESTSIDEGRP        	2
#define MAX_ENTERCLIPREQUEST_SIDECROSSLEGGRP            	40
#define MAX_ENTERTESTRADEREQUEST_SIDEALLOCGRP           	30
#define MAX_ENTERTESTRADEREQUEST_TRDINSTRMNTLEGGRP      	20
#define MAX_ENTERTESTRADEREQUEST_INSTRUMENTEVENTGRP     	2
#define MAX_ENTERTESTRADEREQUEST_INSTRUMENTATTRIBUTEGRP 	6
#define MAX_ENTERTESTRADEREQUEST_UNDERLYINGSTIPGRP      	1
#define MAX_ENTERTESTRADEREQUEST_SRQSRELATEDTRADEIDGRP  	12
#define MAX_INQUIREENRICHMENTRULEIDLISTRESPONSE_ENRICHMENTRULESGRP	400
#define MAX_INQUIREMMPARAMETERRESPONSE_MMPARAMETERGRP   	9
#define MAX_INQUIRESESSIONLISTRESPONSE_SESSIONSGRP      	1000
#define MAX_INQUIREUSERRESPONSE_PARTYDETAILSGRP         	1000
#define MAX_MASSQUOTEREQUEST_QUOTEENTRYGRP              	100
#define MAX_MASSQUOTERESPONSE_QUOTEENTRYACKGRP          	200
#define MAX_MODIFYBASKETTRADEREQUEST_BASKETROOTPARTYGRP 	2
#define MAX_MODIFYBASKETTRADEREQUEST_INSTRMTMATCHSIDEGRP	99
#define MAX_MODIFYBASKETTRADEREQUEST_BASKETSIDEALLOCGRP 	198
#define MAX_MODIFYORDERCOMPLEXREQUEST_LEGORDGRP         	20
#define MAX_MODIFYTESTRADEREQUEST_SIDEALLOCGRP          	30
#define MAX_MODIFYTESTRADEREQUEST_TRDINSTRMNTLEGGRP     	20
#define MAX_MODIFYTESTRADEREQUEST_SRQSRELATEDTRADEIDGRP 	12
#define MAX_NEWORDERCOMPLEXREQUEST_LEGORDGRP            	20
#define MAX_ORDEREXECNOTIFICATION_FILLSGRP              	100
#define MAX_ORDEREXECNOTIFICATION_INSTRMNTLEGEXECGRP    	600
#define MAX_ORDEREXECREPORTBROADCAST_LEGORDGRP          	20
#define MAX_ORDEREXECREPORTBROADCAST_FILLSGRP           	100
#define MAX_ORDEREXECREPORTBROADCAST_INSTRMNTLEGEXECGRP 	600
#define MAX_ORDEREXECRESPONSE_FILLSGRP                  	100
#define MAX_ORDEREXECRESPONSE_INSTRMNTLEGEXECGRP        	600
#define MAX_PRETRADERISKLIMITRESPONSE_RISKLIMITSRPTGRP  	64
#define MAX_PRETRADERISKLIMITSDEFINITIONREQUEST_RISKLIMITQTYGRP	2
#define MAX_QUOTEACTIVATIONNOTIFICATION_NOTAFFECTEDSECURITIESGRP	500
#define MAX_QUOTEACTIVATIONRESPONSE_NOTAFFECTEDSECURITIESGRP	500
#define MAX_QUOTEEXECUTIONREPORT_QUOTEEVENTGRP          	100
#define MAX_QUOTEEXECUTIONREPORT_QUOTELEGEXECGRP        	600
#define MAX_SRQSCREATEDEALNOTIFICATION_ORDERBOOKITEMGRP 	26
#define MAX_SRQSNEGOTIATIONREQUESTERNOTIFICATION_TARGETPARTIES	50
#define MAX_SRQSOPENNEGOTIATIONNOTIFICATION_QUOTREQLEGSGRP	20
#define MAX_SRQSOPENNEGOTIATIONREQUEST_QUOTREQLEGSGRP   	20
#define MAX_SRQSOPENNEGOTIATIONREQUEST_TARGETPARTIES    	50
#define MAX_SRQSOPENNEGOTIATIONREQUESTERNOTIFICATION_QUOTREQLEGSGRP	20
#define MAX_SRQSOPENNEGOTIATIONREQUESTERNOTIFICATION_TARGETPARTIES	50
#define MAX_SRQSUPDATENEGOTIATIONREQUEST_TARGETPARTIES  	50
#define MAX_TESAPPROVEBROADCAST_TRDINSTRMNTLEGGRP       	20
#define MAX_TESAPPROVEBROADCAST_INSTRUMENTEVENTGRP      	2
#define MAX_TESAPPROVEBROADCAST_INSTRUMENTATTRIBUTEGRP  	6
#define MAX_TESAPPROVEBROADCAST_UNDERLYINGSTIPGRP       	1
#define MAX_TESAPPROVEBROADCAST_SRQSRELATEDTRADEIDGRP   	12
#define MAX_TESBROADCAST_SIDEALLOCGRPBC                 	30
#define MAX_TESBROADCAST_TRDINSTRMNTLEGGRP              	20
#define MAX_TESBROADCAST_INSTRUMENTEVENTGRP             	2
#define MAX_TESBROADCAST_INSTRUMENTATTRIBUTEGRP         	6
#define MAX_TESBROADCAST_UNDERLYINGSTIPGRP              	1
#define MAX_TESBROADCAST_SRQSRELATEDTRADEIDGRP          	12
#define MAX_TESTRADEBROADCAST_SRQSRELATEDTRADEIDGRP     	12
#define MAX_TESUPLOADBROADCAST_SIDEALLOCEXTGRP          	30
#define MAX_TESUPLOADBROADCAST_TRDINSTRMNTLEGGRP        	20
#define MAX_TESUPLOADBROADCAST_INSTRUMENTEVENTGRP       	2
#define MAX_TESUPLOADBROADCAST_INSTRUMENTATTRIBUTEGRP   	6
#define MAX_TESUPLOADBROADCAST_UNDERLYINGSTIPGRP        	1
#define MAX_TESUPLOADBROADCAST_SRQSRELATEDTRADEIDGRP    	12
#define MAX_UPLOADTESTRADEREQUEST_SIDEALLOCEXTGRP       	30
#define MAX_UPLOADTESTRADEREQUEST_TRDINSTRMNTLEGGRP     	20
#define MAX_UPLOADTESTRADEREQUEST_INSTRUMENTEVENTGRP    	2
#define MAX_UPLOADTESTRADEREQUEST_INSTRUMENTATTRIBUTEGRP	6
#define MAX_UPLOADTESTRADEREQUEST_UNDERLYINGSTIPGRP     	1
#define MAX_UPLOADTESTRADEREQUEST_SRQSRELATEDTRADEIDGRP 	12

#define LEN_ACCOUNT                                      2
#define LEN_APPLBEGMSGID                                 16
#define LEN_APPLENDMSGID                                 16
#define ENUM_APPLID_TRADE                                1
#define ENUM_APPLID_NEWS                                 2
#define ENUM_APPLID_SERVICE_AVAILABILITY                 3
#define ENUM_APPLID_SESSION_DATA                         4
#define ENUM_APPLID_LISTENER_DATA                        5
#define ENUM_APPLID_RISKCONTROL                          6
#define ENUM_APPLID_TES_MAINTENANCE                      7
#define ENUM_APPLID_TES_TRADE                            8
#define ENUM_APPLID_SRQS_MAINTENANCE                     9
#define ENUM_APPLID_SERVICE_AVAILABILITY_MARKET          10
#define ENUM_APPLID_SPECIALIST_DATA                      11
#define ENUM_APPLIDSTATUS_OUTBOUND_CONVERSION_ERROR      105
#define LEN_APPLMSGID                                    16
#define ENUM_APPLRESENDFLAG_FALSE                        0
#define ENUM_APPLRESENDFLAG_TRUE                         1
#define ENUM_APPLSEQINDICATOR_NO_RECOVERY_REQUIRED       0
#define ENUM_APPLSEQINDICATOR_RECOVERY_REQUIRED          1
#define ENUM_APPLSEQSTATUS_UNAVAILABLE                   0
#define ENUM_APPLSEQSTATUS_AVAILABLE                     1
#define LEN_APPLUSAGEORDERS                              1
#define ENUM_APPLUSAGEORDERS_AUTOMATED                   "A"
#define ENUM_APPLUSAGEORDERS_MANUAL                      "M"
#define ENUM_APPLUSAGEORDERS_AUTOSELECT                  "B"
#define ENUM_APPLUSAGEORDERS_NONE                        "N"
#define LEN_APPLUSAGEQUOTES                              1
#define ENUM_APPLUSAGEQUOTES_AUTOMATED                   "A"
#define ENUM_APPLUSAGEQUOTES_MANUAL                      "M"
#define ENUM_APPLUSAGEQUOTES_AUTOSELECT                  "B"
#define ENUM_APPLUSAGEQUOTES_NONE                        "N"
#define LEN_APPLICATIONSYSTEMNAME                        30
#define LEN_APPLICATIONSYSTEMVENDOR                      30
#define LEN_APPLICATIONSYSTEMVERSION                     30
#define LEN_BASKETSIDETRADEREPORTID                      20
#define LEN_BASKETTRADEREPORTTEXT                        20
#define ENUM_BASKETTRADEREPORTTYPE_SUBMIT                0
#define ENUM_BASKETTRADEREPORTTYPE_ADDENDUM              4
#define ENUM_BASKETTRADEREPORTTYPE_NO_WAS_SUBSTITUE      5
#define LEN_COMPLIANCETEXT                               20
#define LEN_CORESRVC                                     100
#define ENUM_CORESRVC_TRADING                            "Order and Quote Management                                                                          "
#define ENUM_CORESRVC_ORDER_QUOTE_RETRANSMISSION         "Retransmission of Order and Quote Events                                                            "
#define ENUM_CORESRVC_TRADE_RETRANSMISSION               "Trades                                                                                              "
#define ENUM_CORESRVC_TES                                "T7 Entry                                                                                            "
#define ENUM_CORESRVC_SRQS                               "Selective Request for Quote Service                                                                 "
#define ENUM_CORESRVC_NONE                               "None                                                                                                "
#define ENUM_CROSSEDINDICATOR_NO_CROSSING                0
#define ENUM_CROSSEDINDICATOR_CROSS_REJECTED             1
#define LEN_CUSTORDERHANDLINGINST                        1
#define LEN_DEFAULTCSTMAPPLVERID                         30
#define LEN_DEFAULTCSTMAPPLVERSUBID                      5
#define ENUM_DEFAULTCSTMAPPLVERSUBID_CASH                "C0002"
#define ENUM_DEFAULTCSTMAPPLVERSUBID_DERIVATIVES         "D0002"
#define ENUM_DELETEREASON_NO_SPECIAL_REASON              100
#define ENUM_DELETEREASON_TAS_CHANGE                     101
#define ENUM_DELETEREASON_INTRADAY_EXPIRATION            102
#define ENUM_DELETEREASON_RISK_EVENT                     103
#define ENUM_DELETEREASON_STOP_TRADING                   104
#define ENUM_DELETEREASON_INSTRUMENT_DELETION            105
#define ENUM_DELETEREASON_INSTRUMENT_SUSPENSION          106
#define ENUM_DELETEREASON_PRETRADE_RISK_EVENT            107
#define ENUM_EFFECTONBASKET_ADD_VOLUME                   1
#define ENUM_EFFECTONBASKET_REMOVE_VOLUME                2
#define ENUM_EVENTTYPE_SWAP_START_DATE                   8
#define ENUM_EVENTTYPE_SWAP_END_DATE                     9
#define ENUM_EVENTTYPE_REDEMPTION                        26
#define ENUM_EVENTTYPE_DELISTING                         100
#define ENUM_EVENTTYPE_INSTRUMENT_ASSIGNMENT_ADDED       104
#define ENUM_EVENTTYPE_INSTRUMENT_ASSIGNMENT_REMOVED     105
#define ENUM_EVENTTYPE_CLOSED                            106
#define ENUM_EVENTTYPE_RESTRICTED                        107
#define ENUM_EVENTTYPE_BOOK                              108
#define ENUM_EVENTTYPE_CONTINUOUS                        109
#define ENUM_EVENTTYPE_AUCTION                           110
#define ENUM_EVENTTYPE_FREEZE                            111
#define ENUM_EVENTTYPE_CANCEL_FREEZE                     112
#define ENUM_EVENTTYPE_PRE_CALL                          113
#define ENUM_EXECINST_H                                  1
#define ENUM_EXECINST_Q                                  2
#define ENUM_EXECINST_H_Q                                3
#define ENUM_EXECINST_H_6                                5
#define ENUM_EXECINST_Q_6                                6
#define ENUM_EXECRESTATEMENTREASON_CORPORATE_ACTION      0
#define ENUM_EXECRESTATEMENTREASON_ORDER_BOOK_RESTATEMENT 1
#define ENUM_EXECRESTATEMENTREASON_EXCHANGE_OPTION       8
#define ENUM_EXECRESTATEMENTREASON_ORDER_ADDED           101
#define ENUM_EXECRESTATEMENTREASON_ORDER_MODIFIED        102
#define ENUM_EXECRESTATEMENTREASON_ORDER_CANCELLED       103
#define ENUM_EXECRESTATEMENTREASON_IOC_ORDER_CANCELLED   105
#define ENUM_EXECRESTATEMENTREASON_FOK_ORDER_CANCELLED   107
#define ENUM_EXECRESTATEMENTREASON_BOOK_ORDER_EXECUTED   108
#define ENUM_EXECRESTATEMENTREASON_CHANGED_TO_IOC        114
#define ENUM_EXECRESTATEMENTREASON_INSTRUMENT_STATE_CHANGE 122
#define ENUM_EXECRESTATEMENTREASON_MARKET_ORDER_TRIGGERED 135
#define ENUM_EXECRESTATEMENTREASON_PENDING_NEW           138
#define ENUM_EXECRESTATEMENTREASON_PENDING_NEW_APPLIED   141
#define ENUM_EXECRESTATEMENTREASON_PENDING_REPLACE       139
#define ENUM_EXECRESTATEMENTREASON_PENDING_REPLACE_APPLIED 142
#define ENUM_EXECRESTATEMENTREASON_END_OF_DAY_PROCESSING 146
#define ENUM_EXECRESTATEMENTREASON_ORDER_EXPIRATION      148
#define ENUM_EXECRESTATEMENTREASON_CAO_ORDER_ACTIVATED   149
#define ENUM_EXECRESTATEMENTREASON_CAO_ORDER_INACTIVATED 150
#define ENUM_EXECRESTATEMENTREASON_OAO_ORDER_ACTIVATED   151
#define ENUM_EXECRESTATEMENTREASON_OAO_ORDER_INACTIVATED 152
#define ENUM_EXECRESTATEMENTREASON_AAO_ORDER_ACTIVATED   153
#define ENUM_EXECRESTATEMENTREASON_AAO_ORDER_INACTIVATED 154
#define ENUM_EXECRESTATEMENTREASON_IAO_ORDER_ACTIVATED   159
#define ENUM_EXECRESTATEMENTREASON_IAO_ORDER_INACTIVATED 160
#define ENUM_EXECRESTATEMENTREASON_ORDER_REFRESHED       155
#define ENUM_EXECRESTATEMENTREASON_OCO_ORDER_TRIGGERED   164
#define ENUM_EXECRESTATEMENTREASON_STOP_ORDER_TRIGGERED  172
#define ENUM_EXECRESTATEMENTREASON_OWNERSHIP_CHANGED     181
#define ENUM_EXECRESTATEMENTREASON_ORDER_CANCELLATION_PENDING 197
#define ENUM_EXECRESTATEMENTREASON_PENDING_CANCELLATION_EXECUTED 199
#define ENUM_EXECRESTATEMENTREASON_BOC_ORDER_CANCELLED   212
#define ENUM_EXECRESTATEMENTREASON_TRAILING_STOP_UPDATE  213
#define ENUM_EXECRESTATEMENTREASON_EXCEEDS_MAXIMUM_QUANTITY 237
#define ENUM_EXECRESTATEMENTREASON_INVALID_LIMIT_PRICE   238
#define ENUM_EXECRESTATEMENTREASON_USER_DOES_NOT_EXIST   241
#define ENUM_EXECRESTATEMENTREASON_SESSION_DOES_NOT_EXIST 242
#define ENUM_EXECRESTATEMENTREASON_INVALID_STOP_PRICE    243
#define ENUM_EXECRESTATEMENTREASON_INSTRUMENT_DOES_NOT_EXIST 245
#define ENUM_EXECRESTATEMENTREASON_BUSINESS_UNIT_RISK_EVENT 246
#define ENUM_EXECRESTATEMENTREASON_PANIC_CANCEL          261
#define ENUM_EXECRESTATEMENTREASON_DIVIDEND_PAYMENT      292
#define ENUM_EXECRESTATEMENTREASON_LAST_TRADING_DAY      294
#define ENUM_EXECRESTATEMENTREASON_TRADING_PARAMETER_CHANGE 295
#define ENUM_EXECRESTATEMENTREASON_CURRENCY_CHANGE       296
#define ENUM_EXECRESTATEMENTREASON_PRODUCT_ASSIGNMENT_CHANGE 297
#define ENUM_EXECRESTATEMENTREASON_REFERENCE_PRICE_CHANGE 298
#define ENUM_EXECRESTATEMENTREASON_TICK_RULE_CHANGE      300
#define ENUM_EXECRESTATEMENTREASON_QRS_EXPIRY            316
#define ENUM_EXECRESTATEMENTREASON_CLIP_EXECUTION        340
#define ENUM_EXECRESTATEMENTREASON_CLIP_ARRANGEMENT_TIME_OUT 343
#define ENUM_EXECRESTATEMENTREASON_CLIP_ARRANGEMENT_VALIDATION 344
#define LEN_EXECTYPE                                     1
#define ENUM_EXECTYPE_NEW                                "0"
#define ENUM_EXECTYPE_CANCELED                           "4"
#define ENUM_EXECTYPE_REPLACED                           "5"
#define ENUM_EXECTYPE_PENDING_CANCEL_E                   "6"
#define ENUM_EXECTYPE_SUSPENDED                          "9"
#define ENUM_EXECTYPE_RESTATED                           "D"
#define ENUM_EXECTYPE_TRIGGERED                          "L"
#define ENUM_EXECTYPE_TRADE                              "F"
#define ENUM_EXECTYPE_PENDING_NEW                        "A"
#define ENUM_EXECTYPE_PENDING_REPLACE                    "E"
#define ENUM_EXECUTINGTRADERQUALIFIER_ALGO               22
#define ENUM_EXECUTINGTRADERQUALIFIER_HUMAN              24
#define ENUM_EXERCISESTYLE_EUROPEAN                      0
#define ENUM_EXERCISESTYLE_AMERICAN                      1
#define LEN_FIXCLORDID                                   20
#define LEN_FIXENGINENAME                                30
#define LEN_FIXENGINEVENDOR                              30
#define LEN_FIXENGINEVERSION                             30
#define ENUM_FILLLIQUIDITYIND_ADDED_LIQUIDITY            1
#define ENUM_FILLLIQUIDITYIND_REMOVED_LIQUIDITY          2
#define ENUM_FILLLIQUIDITYIND_AUCTION                    4
#define ENUM_FILLLIQUIDITYIND_TRIGGERED_STOP_ORDER       5
#define ENUM_FILLLIQUIDITYIND_TRIGGERED_OCO_ORDER        6
#define ENUM_FILLLIQUIDITYIND_TRIGGERED_MARKET_ORDER     7
#define LEN_FIRMNEGOTIATIONID                            20
#define LEN_FIRMTRADEID                                  20
#define LEN_FREETEXT1                                    12
#define LEN_FREETEXT2                                    12
#define LEN_FREETEXT3                                    12
#define LEN_FREETEXT5                                    132
#define ENUM_FREETEXT5DISCLOSUREINSTRUCTION_NO           0
#define ENUM_FREETEXT5DISCLOSUREINSTRUCTION_YES          1
#define LEN_FUNCCATEGORY                                 100
#define ENUM_FUNCCATEGORY_ORDER_HANDLING                 "Order Handling                                                                                      "
#define ENUM_FUNCCATEGORY_SESSION_LAYER                  "Session Layer                                                                                       "
#define ENUM_FUNCCATEGORY_QUOTE_HANDLING                 "Quote Handling                                                                                      "
#define ENUM_FUNCCATEGORY_BEST_QUOTE_HANDLING            "Best Quote Handling                                                                                 "
#define ENUM_FUNCCATEGORY_QUOTE_AND_CROSS_REQUEST        "Quote and Cross Request                                                                             "
#define ENUM_FUNCCATEGORY_STRATEGY_CREATION              "Strategy Creation                                                                                   "
#define ENUM_FUNCCATEGORY_FLEXIBLE_INSTRUMENT_CREATION   "Flexible Instrument Creation                                                                        "
#define ENUM_FUNCCATEGORY_TES_TRADING                    "TES Trading                                                                                         "
#define ENUM_FUNCCATEGORY_SRQS                           "Selective Request for Quote Service                                                                 "
#define ENUM_FUNCCATEGORY_OTHER                          "Other                                                                                               "
#define ENUM_FUNCCATEGORY_BROADCAST                      "Broadcast                                                                                           "
#define ENUM_FUNCCATEGORY_CLIP_TRADING                   "CLIP Trading                                                                                        "
#define ENUM_FUNCCATEGORY_ISSUER                         "Issuer Model                                                                                        "
#define ENUM_FUNCCATEGORY_BASKET_TRADING                 "Basket Trading                                                                                      "
#define ENUM_FUNCCATEGORY_SPECIALIST                     "Specialist Model                                                                                    "
#define ENUM_GATEWAYSTATUS_STANDBY                       0
#define ENUM_GATEWAYSTATUS_ACTIVE                        1
#define LEN_HEADLINE                                     256
#define ENUM_HEDGETYPE_DURATION_HEDGE                    0
#define ENUM_HEDGETYPE_NOMINAL_HEDGE                     1
#define ENUM_HEDGETYPE_PRICE_FACTOR_HEDGE                2
#define ENUM_IMPLIEDMARKETINDICATOR_NOT_IMPLIED          0
#define ENUM_IMPLIEDMARKETINDICATOR_IMPLIED_IN_OUT       3
#define ENUM_INPUTSOURCE_CLIENT_BROKER                   1
#define ENUM_INPUTSOURCE_PROPRIETARY_BROKER              2
#define ENUM_INSTRATTRIBTYPE_VARIABLE_RATE               5
#define ENUM_INSTRATTRIBTYPE_COUPON_RATE                 100
#define ENUM_INSTRATTRIBTYPE_OFFSET_TO_THE_VARIABLE_COUPON_RATE 101
#define ENUM_INSTRATTRIBTYPE_SWAP_CUSTOMER_1             102
#define ENUM_INSTRATTRIBTYPE_SWAP_CUSTOMER_2             103
#define ENUM_INSTRATTRIBTYPE_CASH_BASKET_REFERENCE       104
#define LEN_INSTRATTRIBVALUE                             32
#define LEN_LASTENTITYPROCESSED                          16
#define ENUM_LASTFRAGMENT_NOT_LAST_MESSAGE               0
#define ENUM_LASTFRAGMENT_LAST_MESSAGE                   1
#define ENUM_LASTMKT_XEUR                                1
#define ENUM_LASTMKT_XEEE                                2
#define ENUM_LASTMKT_XETR                                3
#define ENUM_LASTMKT_XVIE                                4
#define ENUM_LASTMKT_XMAL                                6
#define ENUM_LASTMKT_XBUL                                7
#define ENUM_LASTMKT_XBUD                                8
#define ENUM_LASTMKT_XLJU                                9
#define ENUM_LASTMKT_XPRA                                10
#define ENUM_LASTMKT_XZAG                                11
#define ENUM_LASTMKT_NODX                                12
#define ENUM_LASTMKT_XFRA                                13
#define ENUM_LASTPXDISCLOSUREINSTRUCTION_NO              0
#define ENUM_LASTPXDISCLOSUREINSTRUCTION_YES             1
#define ENUM_LASTQTYDISCLOSUREINSTRUCTION_NO             0
#define ENUM_LASTQTYDISCLOSUREINSTRUCTION_YES            1
#define ENUM_LEAVESQTYDISCLOSUREINSTRUCTION_NO           0
#define ENUM_LEAVESQTYDISCLOSUREINSTRUCTION_YES          1
#define LEN_LEGACCOUNT                                   2
#define ENUM_LEGINPUTSOURCE_CLIENT_BROKER                1
#define ENUM_LEGINPUTSOURCE_PROPRIETARY_BROKER           2
#define LEN_LEGPOSITIONEFFECT                            1
#define ENUM_LEGPOSITIONEFFECT_CLOSE                     "C"
#define ENUM_LEGPOSITIONEFFECT_OPEN                      "O"
#define ENUM_LEGSECURITYTYPE_MULTILEG_INSTRUMENT         1
#define ENUM_LEGSECURITYTYPE_UNDERLYING_LEG              2
#define ENUM_LEGSIDE_BUY                                 1
#define ENUM_LEGSIDE_SELL                                2
#define LEN_LISTUPDATEACTION                             1
#define ENUM_LISTUPDATEACTION_ADD                        "A"
#define ENUM_LISTUPDATEACTION_DELETE                     "D"
#define ENUM_MDBOOKTYPE_TOPOFBOOK                        1
#define ENUM_MDBOOKTYPE_PRICEDEPTH                       2
#define ENUM_MDSUBBOOKTYPE_IMPLIED                       1
#define ENUM_MDSUBBOOKTYPE_VOLUMEWEIGHTEDAVERAGE         2
#define ENUM_MARKETID_XEUR                               1
#define ENUM_MARKETID_XEEE                               2
#define ENUM_MARKETID_XETR                               3
#define ENUM_MARKETID_XVIE                               4
#define ENUM_MARKETID_XMAL                               6
#define ENUM_MARKETID_XBUL                               7
#define ENUM_MARKETID_XBUD                               8
#define ENUM_MARKETID_XLJU                               9
#define ENUM_MARKETID_XPRA                               10
#define ENUM_MARKETID_XZAG                               11
#define ENUM_MARKETID_NODX                               12
#define ENUM_MARKETID_XFRA                               13
#define ENUM_MASSACTIONREASON_NO_SPECIAL_REASON          0
#define ENUM_MASSACTIONREASON_STOP_TRADING               1
#define ENUM_MASSACTIONREASON_EMERGENCY                  2
#define ENUM_MASSACTIONREASON_MARKET_MAKER_PROTECTION    3
#define ENUM_MASSACTIONREASON_SESSION_LOSS               6
#define ENUM_MASSACTIONREASON_DUPLICATE_SESSION_LOGIN    7
#define ENUM_MASSACTIONREASON_CLEARING_RISK_CONTROL      8
#define ENUM_MASSACTIONREASON_INTERNAL_CONNECTION_LOSS   100
#define ENUM_MASSACTIONREASON_PRODUCT_STATE_HALT         105
#define ENUM_MASSACTIONREASON_PRODUCT_STATE_HOLIDAY      106
#define ENUM_MASSACTIONREASON_INSTRUMENT_SUSPENDED       107
#define ENUM_MASSACTIONREASON_COMPLEX_INSTRUMENT_DELETION 109
#define ENUM_MASSACTIONREASON_VOLATILITY_INTERRUPTION    110
#define ENUM_MASSACTIONREASON_PRODUCT_TEMPORARILY_NOT_TRADEABLE 111
#define ENUM_MASSACTIONREASON_INSTRUMENT_STOPPED         113
#define ENUM_MASSACTIONREASON_INSTRUMENT_KNOCK_OUT       115
#define ENUM_MASSACTIONREASON_INSTRUMENT_SOLD_OUT        116
#define ENUM_MASSACTIONREASON_MEMBER_DISABLE             117
#define ENUM_MASSACTIONSUBTYPE_BOOK_OR_CANCEL            1
#define ENUM_MASSACTIONTYPE_SUSPEND_QUOTES               1
#define ENUM_MASSACTIONTYPE_RELEASE_QUOTES               2
#define ENUM_MATCHSUBTYPE_OPENING_AUCTION                1
#define ENUM_MATCHSUBTYPE_CLOSING_AUCTION                2
#define ENUM_MATCHSUBTYPE_INTRADAY_AUCTION               3
#define ENUM_MATCHSUBTYPE_CIRCUIT_BREAKER_AUCTION        4
#define ENUM_MATCHSUBTYPE_OUTSIDE_BBO                    6
#define ENUM_MATCHTYPE_CONFIRMED_TRADE_REPORT            3
#define ENUM_MATCHTYPE_AUTO_MATCH_INCOMING               4
#define ENUM_MATCHTYPE_CROSS_AUCTION                     5
#define ENUM_MATCHTYPE_CALL_AUCTION                      7
#define ENUM_MATCHTYPE_SYSTEMATIC_INTERNALISER           9
#define ENUM_MATCHTYPE_AUTO_MATCH_RESTING                11
#define ENUM_MATCHTYPE_AUTO_MATCH_AT_MID_POINT           12
#define ENUM_MATCHTYPE_LIQUIDITY_IMPROVEMENT_CROSS       13
#define ENUM_MATCHTYPE_CONTINUOUS_AUCTION                14
#define ENUM_MATCHINGENGINESTATUS_UNAVAILABLE            0
#define ENUM_MATCHINGENGINESTATUS_AVAILABLE              1
#define LEN_MESSAGEEVENTSOURCE                           1
#define ENUM_MESSAGEEVENTSOURCE_BROADCAST_TO_INITIATOR   "I"
#define ENUM_MESSAGEEVENTSOURCE_BROADCAST_TO_APPROVER    "A"
#define ENUM_MESSAGEEVENTSOURCE_BROADCAST_TO_REQUESTER   "R"
#define ENUM_MESSAGEEVENTSOURCE_BROADCAST_TO_QUOTE_SUBMITTER "Q"
#define LEN_MSGTYPE                                      3
#define ENUM_MSGTYPE_HEARTBEAT                           "0  "
#define ENUM_MSGTYPE_REJECT                              "3  "
#define ENUM_MSGTYPE_LOGOUT                              "5  "
#define ENUM_MSGTYPE_EXECUTIONREPORT                     "8  "
#define ENUM_MSGTYPE_LOGON                               "A  "
#define ENUM_MSGTYPE_NEWORDERMULTILEG                    "AB "
#define ENUM_MSGTYPE_MULTILEGORDERCANCELREPLACE          "AC "
#define ENUM_MSGTYPE_TRADECAPTUREREPORT                  "AE "
#define ENUM_MSGTYPE_TRADECAPTUREREPORTACK               "AR "
#define ENUM_MSGTYPE_QUOTEREQUESTREJECT                  "AG "
#define ENUM_MSGTYPE_TRADEMATCHREPORT                    "DC "
#define ENUM_MSGTYPE_TRADEMATCHREPORTACK                 "DD "
#define ENUM_MSGTYPE_NEWS                                "B  "
#define ENUM_MSGTYPE_USERREQUEST                         "BE "
#define ENUM_MSGTYPE_USERRESPONSE                        "BF "
#define ENUM_MSGTYPE_APPLICATIONMESSAGEREQUEST           "BW "
#define ENUM_MSGTYPE_APPLICATIONMESSAGEREQUESTACK        "BX "
#define ENUM_MSGTYPE_APPLICATIONMESSAGEREPORT            "BY "
#define ENUM_MSGTYPE_ORDERMASSACTIONREPORT               "BZ "
#define ENUM_MSGTYPE_ORDERMASSACTIONREQUEST              "CA "
#define ENUM_MSGTYPE_USERNOTIFICATION                    "CB "
#define ENUM_MSGTYPE_PARTYRISKLIMITSUPDATEREPORT         "CR "
#define ENUM_MSGTYPE_PARTYRISKLIMITSREQUEST              "CL "
#define ENUM_MSGTYPE_PARTYRISKLIMITSDEFINITIONREQUEST    "CS "
#define ENUM_MSGTYPE_PARTYRISKLIMITSREPORT               "CM "
#define ENUM_MSGTYPE_PARTYENTITLEMENTSUPDATEREPORT       "CZ "
#define ENUM_MSGTYPE_NEWORDERSINGLE                      "D  "
#define ENUM_MSGTYPE_ORDERCANCELREQUEST                  "F  "
#define ENUM_MSGTYPE_ORDERCANCELREPLACEREQUEST           "G  "
#define ENUM_MSGTYPE_QUOTEREQUEST                        "R  "
#define ENUM_MSGTYPE_MARKETDATASNAPSHOTFULLREFRESH       "W  "
#define ENUM_MSGTYPE_MASSQUOTEACKNOWLEDGEMENT            "b  "
#define ENUM_MSGTYPE_SECURITYDEFINITIONREQUEST           "c  "
#define ENUM_MSGTYPE_SECURITYDEFINITION                  "d  "
#define ENUM_MSGTYPE_TRADINGSESSIONSTATUS                "h  "
#define ENUM_MSGTYPE_MASSQUOTE                           "i  "
#define ENUM_MSGTYPE_QUOTE                               "S  "
#define ENUM_MSGTYPE_QUOTEACK                            "CW "
#define ENUM_MSGTYPE_QUOTESTATUSREQUEST                  "a  "
#define ENUM_MSGTYPE_QUOTESTATUSREPORT                   "AI "
#define ENUM_MSGTYPE_QUOTERESPONSE                       "AJ "
#define ENUM_MSGTYPE_REQUESTACKNOWLEDGE                  "U1 "
#define ENUM_MSGTYPE_SESSIONDETAILSLISTREQUEST           "U5 "
#define ENUM_MSGTYPE_SESSIONDETAILSLISTRESPONSE          "U6 "
#define ENUM_MSGTYPE_QUOTEEXECUTIONREPORT                "U8 "
#define ENUM_MSGTYPE_MMPARAMETERDEFINITIONREQUEST        "U14"
#define ENUM_MSGTYPE_CROSSREQUEST                        "DS "
#define ENUM_MSGTYPE_CROSSREQUESTACK                     "DT "
#define ENUM_MSGTYPE_MMPARAMETERREQUEST                  "U17"
#define ENUM_MSGTYPE_MMPARAMETERRESPONSE                 "U18"
#define ENUM_MSGTYPE_SECURITYSTATUSDEFINITIONREQUEST     "U27"
#define ENUM_MSGTYPE_SECURITYSTATUS                      "f  "
#define ENUM_MSGTYPE_PARTYDETAILLISTREQUEST              "CF "
#define ENUM_MSGTYPE_PARTYDETAILLISTREPORT               "CG "
#define ENUM_MSGTYPE_TRADEENRICHMENTLISTREQUEST          "U7 "
#define ENUM_MSGTYPE_TRADEENRICHMENTLISTREPORT           "U9 "
#define ENUM_MSGTYPE_PARTYACTIONREPORT                   "DI "
#define ENUM_MSGTYPE_MARKETDATAINSTRUMENT                "U23"
#define ENUM_MULTILEGREPORTINGTYPE_SINGLE_SECURITY       1
#define ENUM_MULTILEGREPORTINGTYPE_INDIVIDUAL_LEG_OF_A_MULTILEG_SECURITY 2
#define ENUM_MULTILEGMODEL_PREDEFINED_MULTILEG_SECURITY  0
#define ENUM_MULTILEGMODEL_USER_DEFINED_MULTLEG          1
#define ENUM_MULTILEGPRICEMODEL_STANDARD                 0
#define ENUM_MULTILEGPRICEMODEL_USERDEFINED              1
#define ENUM_NEGOTIATEUNDERLYING_NO                      0
#define ENUM_NEGOTIATEUNDERLYING_YES                     1
#define LEN_NETWORKMSGID                                 8
#define ENUM_NUMBEROFRESPDISCLOSUREINSTRUCTION_NO        0
#define ENUM_NUMBEROFRESPDISCLOSUREINSTRUCTION_YES       1
#define LEN_ORDSTATUS                                    1
#define ENUM_ORDSTATUS_NEW                               "0"
#define ENUM_ORDSTATUS_PARTIALLY_FILLED                  "1"
#define ENUM_ORDSTATUS_FILLED                            "2"
#define ENUM_ORDSTATUS_CANCELED                          "4"
#define ENUM_ORDSTATUS_PENDING_CANCEL                    "6"
#define ENUM_ORDSTATUS_SUSPENDED                         "9"
#define ENUM_ORDSTATUS_PENDING_NEW                       "A"
#define ENUM_ORDSTATUS_PENDING_REPLACE                   "E"
#define ENUM_ORDTYPE_MARKET                              1
#define ENUM_ORDTYPE_LIMIT                               2
#define ENUM_ORDTYPE_STOP                                3
#define ENUM_ORDTYPE_STOP_LIMIT                          4
#define ENUM_ORDERATTRIBUTELIQUIDITYPROVISION_Y          1
#define ENUM_ORDERATTRIBUTELIQUIDITYPROVISION_N          0
#define ENUM_ORDERATTRIBUTERISKREDUCTION_Y               1
#define ENUM_ORDERATTRIBUTERISKREDUCTION_N               0
#define LEN_ORDERCATEGORY                                1
#define ENUM_ORDERCATEGORY_ORDER                         "1"
#define ENUM_ORDERCATEGORY_QUOTE                         "2"
#define ENUM_ORDERORIGINATION_DIRECT_ACCESS_OR_SPONSORED_ACCESS_CUSTOMER 5
#define ENUM_ORDERQTYDISCLOSUREINSTRUCTION_NO            0
#define ENUM_ORDERQTYDISCLOSUREINSTRUCTION_YES           1
#define LEN_ORDERROUTINGINDICATOR                        1
#define ENUM_ORDERROUTINGINDICATOR_YES                   "Y"
#define ENUM_ORDERROUTINGINDICATOR_NO                    "N"
#define ENUM_ORDERSIDE_BUY                               1
#define ENUM_ORDERSIDE_SELL                              2
#define ENUM_OWNERSHIPINDICATOR_NO_CHANGE_OF_OWNERSHIP   0
#define ENUM_OWNERSHIPINDICATOR_CHANGE_TO_EXECUTING_TRADER 1
#define LEN_PAD1                                         1
#define LEN_PAD1_1                                       1
#define LEN_PAD1_3                                       1
#define LEN_PAD2                                         2
#define LEN_PAD2_1                                       2
#define LEN_PAD2_2                                       2
#define LEN_PAD3                                         3
#define LEN_PAD3_2                                       3
#define LEN_PAD4                                         4
#define LEN_PAD4_2                                       4
#define LEN_PAD5                                         5
#define LEN_PAD5_1                                       5
#define LEN_PAD6                                         6
#define LEN_PAD6_1                                       6
#define LEN_PAD6_2                                       6
#define LEN_PAD7                                         7
#define LEN_PAD7_1                                       7
#define ENUM_PARTYACTIONTYPE_HALT_TRADING                1
#define ENUM_PARTYACTIONTYPE_REINSTATE                   2
#define LEN_PARTYDETAILDESKID                            3
#define LEN_PARTYDETAILEXECUTINGTRADER                   6
#define LEN_PARTYDETAILEXECUTINGUNIT                     5
#define ENUM_PARTYDETAILROLEQUALIFIER_TRADER             10
#define ENUM_PARTYDETAILROLEQUALIFIER_HEAD_TRADER        11
#define ENUM_PARTYDETAILROLEQUALIFIER_SUPERVISOR         12
#define ENUM_PARTYDETAILSTATUS_ACTIVE                    0
#define ENUM_PARTYDETAILSTATUS_SUSPEND                   1
#define LEN_PARTYENTERINGTRADER                          6
#define LEN_PARTYEXECUTINGFIRM                           5
#define LEN_PARTYEXECUTINGTRADER                         6
#define LEN_PARTYEXECUTINGUNIT                           5
#define LEN_PARTYIDBENEFICIARY                           9
#define ENUM_PARTYIDENTERINGFIRM_PARTICIPANT             1
#define ENUM_PARTYIDENTERINGFIRM_MARKETSUPERVISION       2
#define LEN_PARTYIDLOCATIONID                            2
#define LEN_PARTYIDORDERORIGINATIONFIRM                  7
#define ENUM_PARTYIDORIGINATIONMARKET_XKFE               1
#define LEN_PARTYIDPOSITIONACCOUNT                       32
#define ENUM_PARTYIDSETTLEMENTLOCATION_CLEARSTREM_BANKING_FRANKFURT 1
#define ENUM_PARTYIDSETTLEMENTLOCATION_CLEARSTREM_BANKING_LUXEMBURG 2
#define ENUM_PARTYIDSETTLEMENTLOCATION_CLS_GROUP         3
#define ENUM_PARTYIDSETTLEMENTLOCATION_EUROCLEAR         4
#define LEN_PARTYIDTAKEUPTRADINGFIRM                     5
#define ENUM_PARTYIDINVESTMENTDECISIONMAKERQUALIFIER_ALGO 22
#define ENUM_PARTYIDINVESTMENTDECISIONMAKERQUALIFIER_HUMAN 24
#define ENUM_PARTYSUBIDTYPE_BUYER                        14001
#define ENUM_PARTYSUBIDTYPE_SELLER                       14002
#define LEN_PASSWORD                                     32
#define LEN_POSITIONEFFECT                               1
#define ENUM_POSITIONEFFECT_CLOSE                        "C"
#define ENUM_POSITIONEFFECT_OPEN                         "O"
#define ENUM_PRICEDISCLOSUREINSTRUCTION_NO               0
#define ENUM_PRICEDISCLOSUREINSTRUCTION_YES              1
#define ENUM_PRICEVALIDITYCHECKTYPE_NONE                 0
#define ENUM_PRICEVALIDITYCHECKTYPE_OPTIONAL             1
#define ENUM_PRICEVALIDITYCHECKTYPE_MANDATORY            2
#define ENUM_PRODUCTCOMPLEX_SIMPLE_INSTRUMENT            1
#define ENUM_PRODUCTCOMPLEX_STANDARD_OPTION_STRATEGY     2
#define ENUM_PRODUCTCOMPLEX_NON_STANDARD_OPTION_STRATEGY 3
#define ENUM_PRODUCTCOMPLEX_VOLATILITY_STRATEGY          4
#define ENUM_PRODUCTCOMPLEX_FUTURES_SPREAD               5
#define ENUM_PRODUCTCOMPLEX_INTER_PRODUCT_SPREAD         6
#define ENUM_PRODUCTCOMPLEX_STANDARD_FUTURE_STRATEGY     7
#define ENUM_PRODUCTCOMPLEX_PACK_AND_BUNDLE              8
#define ENUM_PRODUCTCOMPLEX_STRIP                        9
#define ENUM_PRODUCTCOMPLEX_FLEXIBLE_SIMPLE_INSTRUMENT   10
#define ENUM_PUTORCALL_PUT                               0
#define ENUM_PUTORCALL_CALL                              1
#define ENUM_QUOTECANCELTYPE_CANCEL_ALL_QUOTES           4
#define ENUM_QUOTEENTRYREJECTREASON_UNKNOWN_SECURITY     1
#define ENUM_QUOTEENTRYREJECTREASON_DUPLICATE_QUOTE      6
#define ENUM_QUOTEENTRYREJECTREASON_INVALID_PRICE        8
#define ENUM_QUOTEENTRYREJECTREASON_NO_REFERENCE_PRICE_AVAILABLE 16
#define ENUM_QUOTEENTRYREJECTREASON_NO_SINGLE_SIDED_QUOTES 100
#define ENUM_QUOTEENTRYREJECTREASON_INVALID_QUOTING_MODEL 103
#define ENUM_QUOTEENTRYREJECTREASON_INVALID_SIZE         106
#define ENUM_QUOTEENTRYREJECTREASON_INVALID_UNDERLYING_PRICE 107
#define ENUM_QUOTEENTRYREJECTREASON_BID_PRICE_NOT_REASONABLE 108
#define ENUM_QUOTEENTRYREJECTREASON_ASK_PRICE_NOT_REASONABLE 109
#define ENUM_QUOTEENTRYREJECTREASON_BID_PRICE_EXCEEDS_RANGE 110
#define ENUM_QUOTEENTRYREJECTREASON_ASK_PRICE_EXCEEDS_RANGE 111
#define ENUM_QUOTEENTRYREJECTREASON_INSTRUMENT_STATE_FREEZE 115
#define ENUM_QUOTEENTRYREJECTREASON_DELETION_ALREADY_PENDING 116
#define ENUM_QUOTEENTRYREJECTREASON_PRE_TRADE_RISK_SESSION_LIMIT_EXCEEDED 117
#define ENUM_QUOTEENTRYREJECTREASON_PRE_TRADE_RISK_BU_LIMIT_EXCEEDED 118
#define ENUM_QUOTEENTRYREJECTREASON_ENTITLEMENT_NOT_ASSIGNED_FOR_UNDERLYING 119
#define ENUM_QUOTEENTRYREJECTREASON_BID_VALUE_EXCEEDS_LIMIT 120
#define ENUM_QUOTEENTRYREJECTREASON_ASK_VALUE_EXCEEDS_LIMIT 121
#define ENUM_QUOTEENTRYREJECTREASON_NOT_TRADEABLE_FOR_BUSINESSUNIT 122
#define ENUM_QUOTEENTRYREJECTREASON_CURRENTLY_NOT_TRADEABLE_ON_BOOK 124
#define ENUM_QUOTEENTRYREJECTREASON_QUANTITY_LIMIT_EXCEEDED 125
#define ENUM_QUOTEENTRYREJECTREASON_VALUE_LIMIT_EXCEEDED 126
#define ENUM_QUOTEENTRYREJECTREASON_INVALID_QUOTE_SPREAD 127
#define ENUM_QUOTEENTRYREJECTREASON_BOOK_OR_CANCEL       128
#define ENUM_QUOTEENTRYREJECTREASON_CANT_PROC_IN_CURR_INSTR_STATE 131
#define ENUM_QUOTEENTRYREJECTREASON_INVALID_QUOTE_TYPE   134
#define ENUM_QUOTEENTRYREJECTREASON_PWT_QUOTE_NOT_ALLOWED_IN_CURRENT_STATE 135
#define ENUM_QUOTEENTRYREJECTREASON_STANDARD_QUOTE_NOT_ALLOWED_IN_CURRENT_STATE 136
#define ENUM_QUOTEENTRYREJECTREASON_PWT_QUOTE_NOT_ALLOWED_WITH_CROSSED_BOOK 137
#define ENUM_QUOTEENTRYREJECTREASON_ASK_SIDE_QUOTE_NOT_ALLOWED 138
#define ENUM_QUOTEENTRYREJECTREASON_ASK_SIDE_QUOTE_WITH_QTY_NOT_ALLOWED 139
#define ENUM_QUOTEENTRYREJECTREASON_INVALID_CHANGE_LP_SESSION 140
#define ENUM_QUOTEENTRYREJECTREASON_TRADING_INDICATION_RUNNING_FOR_TRADER 143
#define ENUM_QUOTEENTRYREJECTREASON_ON_BOOK_TRADING_DISABLED_FOR_INSTRUMENT_TYPE 144
#define ENUM_QUOTEENTRYREJECTREASON_LP_LICENCE_NOT_ASSIGNED 145
#define ENUM_QUOTEENTRYREJECTREASON_SP_LICENCE_NOT_ASSIGNED 146
#define ENUM_QUOTEENTRYREJECTREASON_LIQUIDITY_PROVIDER_PROTECTION_BID_SIDE_CANCELLED 147
#define ENUM_QUOTEENTRYREJECTREASON_LIQUIDITY_PROVIDER_PROTECTION_ASK_SIDE_CANCELLED 148
#define ENUM_QUOTEENTRYREJECTREASON_QUANTITY_LIMIT_EXCEEDED_INSTRUMENT 149
#define ENUM_QUOTEENTRYREJECTREASON_VALUE_LIMIT_EXCEEDED_INSTRUMENT 150
#define ENUM_QUOTEENTRYREJECTREASON_ISSUER_STOPPED       151
#define ENUM_QUOTEENTRYREJECTREASON_PARTIAL_EXEC_OF_QRS_ORDER 152
#define ENUM_QUOTEENTRYREJECTREASON_MATCHING_QUOTE_NOT_ALLOWED_IN_CURRENT_STATE 153
#define ENUM_QUOTEENTRYREJECTREASON_OUTSIDE_QUOTING_PERIOD 155
#define ENUM_QUOTEENTRYREJECTREASON_MATCH_PRICE_NOT_ON_PRICE_STEP 156
#define ENUM_QUOTEENTRYREJECTREASON_QUANTITY_LIMIT_EXCEEDS_TSL 161
#define ENUM_QUOTEENTRYREJECTREASON_INVALID_TRADINGSESSIONSUBID_FOR_INSTRUMENT 162
#define ENUM_QUOTEENTRYSTATUS_ACCEPTED                   0
#define ENUM_QUOTEENTRYSTATUS_REJECTED                   5
#define ENUM_QUOTEENTRYSTATUS_REMOVED_AND_REJECTED       6
#define ENUM_QUOTEENTRYSTATUS_PENDING                    10
#define ENUM_QUOTEEVENTLIQUIDITYIND_ADDED_LIQUIDITY      1
#define ENUM_QUOTEEVENTLIQUIDITYIND_REMOVED_LIQUIDITY    2
#define ENUM_QUOTEEVENTLIQUIDITYIND_AUCTION              4
#define ENUM_QUOTEEVENTREASON_PENDING_CANCELLATION_EXECUTED 14
#define ENUM_QUOTEEVENTREASON_INVALID_PRICE              15
#define ENUM_QUOTEEVENTREASON_CROSS_REJECTED             16
#define ENUM_QUOTEEVENTREASON_BOOK_OR_CANCEL             17
#define ENUM_QUOTEEVENTREASON_PLP                        18
#define ENUM_QUOTEEVENTREASON_PRICE_NOT_TOP_OF_BOOK      19
#define ENUM_QUOTEEVENTREASON_RANDOM_SELECTION           20
#define ENUM_QUOTEEVENTREASON_MANUAL_SELECTION           21
#define ENUM_QUOTEEVENTSIDE_BUY                          1
#define ENUM_QUOTEEVENTSIDE_SELL                         2
#define ENUM_QUOTEEVENTTYPE_MODIFIED_QUOTE_SIDE          2
#define ENUM_QUOTEEVENTTYPE_REMOVED_QUOTE_SIDE           3
#define ENUM_QUOTEEVENTTYPE_PARTIALLY_FILLED             4
#define ENUM_QUOTEEVENTTYPE_FILLED                       5
#define ENUM_QUOTEINSTRUCTION_DO_NOT_QUOTE               0
#define ENUM_QUOTEINSTRUCTION_QUOTE                      1
#define ENUM_QUOTEQUALIFIER_NONE                         0
#define ENUM_QUOTEQUALIFIER_STP                          4
#define ENUM_QUOTEREFPRICESOURCE_UNDERLYING              1
#define ENUM_QUOTEREFPRICESOURCE_CUSTOM_UNDERLYING_PRICE 2
#define ENUM_QUOTEREFPRICESOURCE_REF_PRICE               3
#define LEN_QUOTEREQID                                   20
#define ENUM_QUOTESIZETYPE_TOTALSIZE                     1
#define ENUM_QUOTESIZETYPE_OPENSIZE                      2
#define ENUM_QUOTESTATUS_REMOVED                         6
#define ENUM_QUOTESTATUS_EXPIRED                         7
#define ENUM_QUOTESTATUS_ACTIVE                          16
#define ENUM_QUOTESUBTYPE_WORKINGDELTA                   1
#define ENUM_QUOTETYPE_INDICATIVE                        0
#define ENUM_QUOTETYPE_TRADEABLE                         1
#define ENUM_QUOTETYPE_TRADEABLE_BOC                     100
#define ENUM_QUOTETYPE_TRADEABLE_MATCHING                101
#define ENUM_QUOTETYPE_TRADEABLE_PWT                     102
#define ENUM_QUOTETYPE_SPECIAL_AUCTION                   103
#define ENUM_QUOTINGSTATUS_OPEN_ACTIVE                   1
#define ENUM_QUOTINGSTATUS_OPEN_IDLE                     2
#define ENUM_QUOTINGSTATUS_CLOSED_INACTIVE               3
#define ENUM_QUOTINGSTATUS_OPEN_NOT_RESPONDED            4
#define ENUM_REFAPPLID_TRADE                             1
#define ENUM_REFAPPLID_NEWS                              2
#define ENUM_REFAPPLID_SERVICE_AVAILABILITY              3
#define ENUM_REFAPPLID_SESSION_DATA                      4
#define ENUM_REFAPPLID_LISTENER_DATA                     5
#define ENUM_REFAPPLID_RISKCONTROL                       6
#define ENUM_REFAPPLID_TES_MAINTENANCE                   7
#define ENUM_REFAPPLID_TES_TRADE                         8
#define ENUM_REFAPPLID_SRQS_MAINTENANCE                  9
#define ENUM_REFAPPLID_SERVICE_AVAILABILITY_MARKET       10
#define ENUM_REFAPPLID_SPECIALIST_DATA                   11
#define LEN_REFAPPLLASTMSGID                             16
#define LEN_REGULATORYTRADEID                            52
#define ENUM_RELATEDPRODUCTCOMPLEX_STANDARD_OPTION_STRATEGY 2
#define ENUM_RELATEDPRODUCTCOMPLEX_NON_STANDARD_OPTION_STRATEGY 3
#define ENUM_RELATEDPRODUCTCOMPLEX_VOLATILITY_STRATEGY   4
#define ENUM_RELATEDPRODUCTCOMPLEX_FUTURES_SPREAD        5
#define ENUM_RELATEDPRODUCTCOMPLEX_INTER_PRODUCT_SPREAD  6
#define ENUM_RELATEDPRODUCTCOMPLEX_STANDARD_FUTURE_STRATEGY 7
#define ENUM_RELATEDPRODUCTCOMPLEX_PACK_AND_BUNDLE       8
#define ENUM_RELATEDPRODUCTCOMPLEX_STRIP                 9
#define LEN_REQUESTINGPARTYCLEARINGFIRM                  9
#define LEN_REQUESTINGPARTYENTERINGFIRM                  9
#define ENUM_REQUESTINGPARTYIDENTERINGFIRM_PARTICIPANT   1
#define ENUM_REQUESTINGPARTYIDENTERINGFIRM_MARKETSUPERVISION 2
#define ENUM_REQUESTINGPARTYIDEXECUTINGSYSTEM_EUREXCLEARING 1
#define ENUM_REQUESTINGPARTYIDEXECUTINGSYSTEM_T7         2
#define ENUM_REQUESTINGPARTYSUBIDTYPE_REQUESTER          5000
#define ENUM_REQUESTINGPARTYSUBIDTYPE_RESPONDER          5001
#define ENUM_REQUESTINGPARTYSUBIDTYPE_BOTH               5002
#define ENUM_REQUESTINGPARTYSUBIDTYPE_SYSTEM             5003
#define ENUM_RISKLIMITACTION_QUEUEINBOUND                0
#define ENUM_RISKLIMITACTION_REJECT                      2
#define ENUM_RISKLIMITACTION_WARNING                     4
#define LEN_RISKLIMITGROUP                               3
#define ENUM_RISKLIMITPLATFORM_ON_BOOK                   0
#define ENUM_RISKLIMITPLATFORM_OFF_BOOK                  1
#define ENUM_RISKLIMITREQUESTINGPARTYROLE_CLEARING_FIRM  4
#define ENUM_RISKLIMITREQUESTINGPARTYROLE_EXCHANGE       22
#define ENUM_RISKLIMITREQUESTINGPARTYROLE_EXECUTING_UNIT 59
#define ENUM_RISKLIMITTYPE_LONG_LIMIT                    4
#define ENUM_RISKLIMITTYPE_SHORT_LIMIT                   5
#define ENUM_RISKLIMITVIOLATIONINDICATOR_NO              0
#define ENUM_RISKLIMITVIOLATIONINDICATOR_YES             1
#define LEN_ROOTPARTYCLEARINGFIRM                        5
#define LEN_ROOTPARTYCLEARINGORGANIZATION                4
#define LEN_ROOTPARTYCONTRAFIRM                          5
#define LEN_ROOTPARTYCONTRATRADER                        6
#define LEN_ROOTPARTYENTERINGTRADER                      6
#define LEN_ROOTPARTYEXECUTINGFIRM                       5
#define LEN_ROOTPARTYEXECUTINGTRADER                     6
#define LEN_ROOTPARTYIDBENEFICIARY                       9
#define LEN_ROOTPARTYIDEXECUTIONVENUE                    4
#define ENUM_ROOTPARTYIDINVESTMENTDECISIONMAKERQUALIFIER_ALGO 22
#define ENUM_ROOTPARTYIDINVESTMENTDECISIONMAKERQUALIFIER_HUMAN 24
#define LEN_ROOTPARTYIDORDERORIGINATIONFIRM              7
#define LEN_ROOTPARTYIDPOSITIONACCOUNT                   32
#define LEN_ROOTPARTYIDTAKEUPTRADINGFIRM                 5
#define ENUM_ROOTPARTYSUBIDTYPE_BUYER                    4001
#define ENUM_ROOTPARTYSUBIDTYPE_SELLER                   4002
#define ENUM_ROOTPARTYSUBIDTYPE_BROKER                   4003
#define ENUM_SECONDARYGATEWAYSTATUS_STANDBY              0
#define ENUM_SECONDARYGATEWAYSTATUS_ACTIVE               1
#define ENUM_SELECTIVEREQUESTFORQUOTESERVICESTATUS_UNAVAILABLE 0
#define ENUM_SELECTIVEREQUESTFORQUOTESERVICESTATUS_AVAILABLE 1
#define ENUM_SESSIONMODE_HF                              1
#define ENUM_SESSIONMODE_LF                              2
#define ENUM_SESSIONMODE_GUI                             3
#define ENUM_SESSIONREJECTREASON_REQUIRED_TAG_MISSING    1
#define ENUM_SESSIONREJECTREASON_VALUE_IS_INCORRECT      5
#define ENUM_SESSIONREJECTREASON_DECRYPTION_PROBLEM      7
#define ENUM_SESSIONREJECTREASON_INVALID_MSGID           11
#define ENUM_SESSIONREJECTREASON_INCORRECT_NUMINGROUP_COUNT 16
#define ENUM_SESSIONREJECTREASON_OTHER                   99
#define ENUM_SESSIONREJECTREASON_THROTTLE_LIMIT_EXCEEDED 100
#define ENUM_SESSIONREJECTREASON_EXPOSURE_LIMIT_EXCEEDED 101
#define ENUM_SESSIONREJECTREASON_SERVICE_TEMPORARILY_NOT_AVAILABLE 102
#define ENUM_SESSIONREJECTREASON_SERVICE_NOT_AVAILABLE   103
#define ENUM_SESSIONREJECTREASON_OUTBOUND_CONVERSION_ERROR 105
#define ENUM_SESSIONREJECTREASON_HEARTBEAT_VIOLATION     152
#define ENUM_SESSIONREJECTREASON_INTERNAL_TECHNICAL_ERROR 200
#define ENUM_SESSIONREJECTREASON_VALIDATION_ERROR        210
#define ENUM_SESSIONREJECTREASON_USER_ALREADY_LOGGED_IN  211
#define ENUM_SESSIONREJECTREASON_GATEWAY_IS_STANDBY      216
#define ENUM_SESSIONREJECTREASON_SESSION_LOGIN_LIMIT_REACHED 217
#define ENUM_SESSIONREJECTREASON_USER_ENTITLEMENT_DATA_TIMEOUT 223
#define ENUM_SESSIONREJECTREASON_PSGATEWAY_SESSION_LIMIT_REACHED 224
#define ENUM_SESSIONREJECTREASON_USER_LOGIN_LIMIT_REACHED 225
#define ENUM_SESSIONREJECTREASON_OUTSTANDING_LOGINS_BU_LIMIT_REACHED 226
#define ENUM_SESSIONREJECTREASON_OUTSTANDING_LOGINS_SESSION_LIMIT_REACHED 227
#define ENUM_SESSIONREJECTREASON_ORDER_NOT_FOUND         10000
#define ENUM_SESSIONREJECTREASON_PRICE_NOT_REASONABLE    10001
#define ENUM_SESSIONREJECTREASON_CLIENTORDERID_NOT_UNIQUE 10002
#define ENUM_SESSIONREJECTREASON_QUOTE_ACTIVATION_IN_PROGRESS 10003
#define ENUM_SESSIONREJECTREASON_BU_BOOK_ORDER_LIMIT_EXCEEDED 10004
#define ENUM_SESSIONREJECTREASON_SESSION_BOOK_ORDER_LIMIT_EXCEEDED 10005
#define ENUM_SESSIONREJECTREASON_STOP_BID_PRICE_NOT_REASONABLE 10006
#define ENUM_SESSIONREJECTREASON_STOP_ASK_PRICE_NOT_REASONABLE 10007
#define ENUM_SESSIONREJECTREASON_ORDER_NOT_EXECUTABLE_WITHIN_VALIDITY 10008
#define ENUM_SESSIONREJECTREASON_INVALID_TRADING_RESTRICTION_FOR_INSTRUMENT_STATE 10009
#define ENUM_SESSIONREJECTREASON_CREATE_CI_THROTTLE_EXCEEDED 10010
#define ENUM_SESSIONREJECTREASON_TRANSACTION_NOT_ALLOWED_IN_CURRENT_STATE 10011
#define ENUM_SESSIONSTATUS_ACTIVE                        0
#define ENUM_SESSIONSTATUS_LOGOUT                        4
#define ENUM_SESSIONSUBMODE_REGULAR_TRADING_SESSION      0
#define ENUM_SESSIONSUBMODE_FIX_TRADING_SESSION          1
#define ENUM_SESSIONSUBMODE_REGULAR_BACK_OFFICE_SESSION  2
#define ENUM_SESSIONSUBMODE_XQS_SESSION                  3
#define LEN_SETTLMETHOD                                  1
#define ENUM_SETTLMETHOD_CASH_SETTLEMENT                 "C"
#define ENUM_SETTLMETHOD_PHYSICAL_SETTLEMENT             "P"
#define ENUM_SIDE_BUY                                    1
#define ENUM_SIDE_SELL                                   2
#define LEN_SIDECOMPLIANCETEXT                           20
#define ENUM_SIDEDISCLOSUREINSTRUCTION_NO                0
#define ENUM_SIDEDISCLOSUREINSTRUCTION_YES               1
#define ENUM_SIDELIQUIDITYIND_ADDED_LIQUIDITY            1
#define ENUM_SIDELIQUIDITYIND_REMOVED_LIQUIDITY          2
#define ENUM_SIDELIQUIDITYIND_AUCTION                    4
#define ENUM_SIDETRDSUBTYP_BLOCK_TRADE                   2001
#define ENUM_SIDETRDSUBTYP_TRADE_AT_MARKET               2004
#define ENUM_SKIPVALIDATIONS_FALSE                       0
#define ENUM_SKIPVALIDATIONS_TRUE                        1
#define LEN_SYMBOL                                       4
#define ENUM_T7ENTRYSERVICERTMSTATUS_UNAVAILABLE         0
#define ENUM_T7ENTRYSERVICERTMSTATUS_AVAILABLE           1
#define ENUM_T7ENTRYSERVICESTATUS_UNAVAILABLE            0
#define ENUM_T7ENTRYSERVICESTATUS_AVAILABLE              1
#define LEN_TARGETPARTYENTERINGTRADER                    6
#define LEN_TARGETPARTYEXECUTINGFIRM                     5
#define LEN_TARGETPARTYEXECUTINGTRADER                   6
#define LEN_TARGETPARTYIDDESKID                          3
#define LEN_TEXT                                         12
#define ENUM_TIMEINFORCE_DAY                             0
#define ENUM_TIMEINFORCE_GTC                             1
#define ENUM_TIMEINFORCE_IOC                             3
#define ENUM_TIMEINFORCE_FOK                             4
#define ENUM_TIMEINFORCE_GTX                             5
#define ENUM_TIMEINFORCE_GTD                             6
#define ENUM_TRADSESEVENT_START_OF_SERVICE               101
#define ENUM_TRADSESEVENT_MARKET_RESET                   102
#define ENUM_TRADSESEVENT_END_OF_RESTATEMENT             103
#define ENUM_TRADSESEVENT_END_OF_DAY_SERVICE             104
#define ENUM_TRADSESEVENT_SERVICE_RESUMED                105
#define ENUM_TRADSESMODE_TESTING                         1
#define ENUM_TRADSESMODE_SIMULATED                       2
#define ENUM_TRADSESMODE_PRODUCTION                      3
#define ENUM_TRADSESMODE_ACCEPTANCE                      4
#define ENUM_TRADSESMODE_DISASTER_RECOVERY               5
#define ENUM_TRADEALLOCSTATUS_PENDING                    1
#define ENUM_TRADEALLOCSTATUS_APPROVED                   2
#define ENUM_TRADEALLOCSTATUS_AUTO_APPROVED              3
#define ENUM_TRADEALLOCSTATUS_UPLOADED                   4
#define ENUM_TRADEALLOCSTATUS_CANCELED                   5
#define ENUM_TRADEMANAGERSTATUS_UNAVAILABLE              0
#define ENUM_TRADEMANAGERSTATUS_AVAILABLE                1
#define ENUM_TRADEPLATFORM_OFF_BOOK                      0
#define ENUM_TRADEPLATFORM_ON_BOOK                       1
#define ENUM_TRADEPUBLISHINDICATOR_DO_NOT_PUBLISH_TRADE  0
#define ENUM_TRADEPUBLISHINDICATOR_PUBLISH_TRADE         1
#define ENUM_TRADEPUBLISHINDICATOR_DEFERRED_PUBLICATION  2
#define ENUM_TRADEPUBLISHINDICATOR_PUBLISHED             3
#define LEN_TRADEREPORTID                                20
#define LEN_TRADEREPORTTEXT                              20
#define ENUM_TRADEREPORTTYPE_SUBMIT                      0
#define ENUM_TRADEREPORTTYPE_ALLEGED                     1
#define ENUM_TRADEREPORTTYPE_ACCEPT                      2
#define ENUM_TRADEREPORTTYPE_DECLINE                     3
#define ENUM_TRADEREPORTTYPE_NO_WAS_REPLACED             5
#define ENUM_TRADEREPORTTYPE_TRADE_REPORT_CANCEL         6
#define ENUM_TRADEREPORTTYPE_TRADE_BREAK                 7
#define ENUM_TRADEREPORTTYPE_ALLEGED_NEW                 11
#define ENUM_TRADEREPORTTYPE_ALLEGED_NO_WAS              13
#define ENUM_TRADEREQUESTRESULT_CANCEL_PENDING           100
#define ENUM_TRADEREQUESTRESULT_CANCEL_DECLINED          101
#define ENUM_TRADEREQUESTRESULT_CANCEL_APPROVED          102
#define ENUM_TRADEUNDERLYING_NO                          1
#define ENUM_TRADEUNDERLYING_YES                         2
#define ENUM_TRADINGCAPACITY_CUSTOMER                    1
#define ENUM_TRADINGCAPACITY_BROKER_DEALER               3
#define ENUM_TRADINGCAPACITY_PRINCIPAL                   5
#define ENUM_TRADINGCAPACITY_MARKET_MAKER                6
#define ENUM_TRADINGCAPACITY_SYSTEMATIC_INTERNALISER     8
#define ENUM_TRADINGCAPACITY_RISKLESS_PRINCIPAL          9
#define ENUM_TRADINGSESSIONSUBID_OPENING_AUCTION         2
#define ENUM_TRADINGSESSIONSUBID_CLOSING_AUCTION         4
#define ENUM_TRADINGSESSIONSUBID_INTRADAY_AUCTION        6
#define ENUM_TRADINGSESSIONSUBID_ANY_AUCTION             8
#define ENUM_TRADINGSESSIONSUBID_SPECIAL_AUCTION         105
#define ENUM_TRANSACTIONDELAYINDICATOR_NOT_DELAYED       0
#define ENUM_TRANSACTIONDELAYINDICATOR_DELAYED           1
#define ENUM_TRANSFERREASON_OWNER                        1
#define ENUM_TRANSFERREASON_CLEARER                      2
#define ENUM_TRDRPTSTATUS_ACCEPTED                       0
#define ENUM_TRDRPTSTATUS_REJECTED                       1
#define ENUM_TRDRPTSTATUS_CANCELLED                      2
#define ENUM_TRDRPTSTATUS_PENDING_NEW                    4
#define ENUM_TRDRPTSTATUS_TERMINATED                     7
#define ENUM_TRDRPTSTATUS_DEEMED_VERIFIED                9
#define ENUM_TRDTYPE_BLOCK_TRADE                         1
#define ENUM_TRDTYPE_EXCHANGE_FOR_SWAP                   12
#define ENUM_TRDTYPE_OTC                                 54
#define ENUM_TRDTYPE_VOLA_TRADE                          1000
#define ENUM_TRDTYPE_EFP_FIN_TRADE                       1001
#define ENUM_TRDTYPE_EFP_INDEX_FUTURES_TRADE             1002
#define ENUM_TRDTYPE_TRADE_AT_MARKET                     1004
#define ENUM_TRDTYPE_LIS                                 1005
#define ENUM_TRDTYPE_ENLIGHT                             1006
#define ENUM_TRDTYPE_BTRF                                1010
#define ENUM_TRIGGERED_NOT_TRIGGERED                     0
#define ENUM_TRIGGERED_TRIGGERED_STOP                    1
#define ENUM_TRIGGERED_TRIGGERED_OCO                     2
#define LEN_UNDERLYINGCURRENCY                           3
#define LEN_UNDERLYINGISSUER                             30
#define LEN_UNDERLYINGSECURITYDESC                       30
#define LEN_UNDERLYINGSECURITYID                         12
#define LEN_UNDERLYINGSTIPTYPE                           7
#define ENUM_UNDERLYINGSTIPTYPE_PAY_FREQUENCY            "PAYFREQ"
#define LEN_UNDERLYINGSTIPVALUE                          32
#define ENUM_USERSTATUS_USER_FORCED_LOGOUT               7
#define ENUM_USERSTATUS_USER_STOPPED                     10
#define ENUM_USERSTATUS_USER_RELEASED                    11
#define ENUM_VALUECHECKTYPEMINLOTSIZE_DO_NOT_CHECK       0
#define ENUM_VALUECHECKTYPEMINLOTSIZE_CHECK              1
#define ENUM_VALUECHECKTYPEQUANTITY_DO_NOT_CHECK         0
#define ENUM_VALUECHECKTYPEQUANTITY_CHECK                1
#define ENUM_VALUECHECKTYPEVALUE_DO_NOT_CHECK            0
#define ENUM_VALUECHECKTYPEVALUE_CHECK                   1
#define LEN_VARTEXT                                      2000

/*
 * End of DEPRECATED defines
 */

#if defined(__cplusplus) || defined(c_plusplus)
} /* close scope of 'extern "C"' declaration. */
} /* close namespace Derivatives */
#endif

#endif
