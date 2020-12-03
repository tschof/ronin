#include "stdafx.h"
#include "MutiTypedMap.h"

#include <md-core/mdapplication.h>
#include <md-framework/config/configexception.h>
#include <md-framework/resource/resourceexception.h>
#include <md-core/MDSessionMgr.h>
#include <md-core/MDPriceLevel.h>

#include <md-framework/util/SimpleMutex.h>

#include <iomanip>

#ifdef APISRLAB_EXPORTS
#define MARKETDATA_API __declspec(dllexport)
#else
#define MARKETDATA_API __declspec(dllimport)
#endif

extern MARKETDATA_API int returnCode;

class SRLab : public md_core::MDListener, public md_core::MDAdminListener
{
public:
	md_framework::SimpleMutex _srMutex;

	virtual ~SRLab(){};
	void onBookWithOrderReplace (const md_core::MDSubscription* sub, const md_core::MDOrderBook* book,const md_core::MDOrder* oldorder, const md_core::MDOrder* neworder);
	void onBook (const md_core::MDSubscription* sub, const md_core::MDOrderBook* book,const md_core::MDOrder* cause, const md_core::MDTrade * trade);
	void onOrderReplace (const md_core::MDSubscription *sub, const md_core::MDOrder *oldOrder, const md_core::MDOrder *newOrder);
	void onOrder (const md_core::MDSubscription *sub, const md_core::MDOrder *o, const md_core::MDTrade *trade);
	void onQuote (const md_core::MDSubscription* sub,const md_core::MDQuote* q);
	void onTrade (const md_core::MDSubscription* sub,const md_core::MDTrade* t);
	void onImbalance (const md_core::MDSubscription *sub, const md_core::MDImbalance *i);
	void onBestPrice (const md_core::MDSubscription* sub,const md_core::MDQuote* q);
	void onCustomEvent(const md_core::MDSubscription* sub, const md_core::MDCustomEvent* c);
	void onSecurityStatusChange(const md_core::InstrumentDef* instrument);
	void onSubscriptionInvalid (const md_core::MDSubscription *sub, const char *reason);
	//Alerts
	void onTradingIndicationAlert (const md_core::MDSubscription *sub,const md_core::MDMktTradingIndicationAlert* alert);
	void onTradeDisseminationTimeAlert (const md_core::MDSubscription *sub,const md_core::MDMktTradeDisseminationTimeAlert* alert);
	//Admin Listener
	void onPlaybackComplete(md_core::MDSessionMgr* sm);
	void onGap (md_core::MDSessionMgr* sm,const char* connection,const char* line, unsigned long long prevSNO, unsigned long long curSNO,bool isRecoveryEnabled);
	void onGapFill(md_core::MDSessionMgr* sm,const char* connection,const char* line, unsigned long long fromSNO, unsigned long long curSNO, bool isFullyFilled);
	void onSlowConsumer(md_core::MDSessionMgr* sm,const char* connection,const char* line, int numEventsSampled,int numEventsAboveThreshold,
					   long long maxUserTimeInMicros, long long minUserTimeInMicros);
	void onMDRCForcedDisconnect(md_core::MDSessionMgr*, const char* connection,const char* line, const char* reason);

	void DecodeSub(System::String^ msg, System::String^ symbol, System::String^ source, const md_core::MDSubscription* sub);
	void DecodeTrade(System::String^ msg, System::String^ symbol, System::String^ source, const md_core::MDTrade* t);
	void DecodeQuote(System::String^ msg, System::String^ symbol,System::String^ source, const md_core::MDQuote* q);
	void DecodeBook(System::String^ msg, System::String^ symbol, System::String^ source, const md_core::MDOrderBook* book);
	void DecodeCustom(System::String^ msg, System::String^ symbol, System::String^ source, const md_core::MDCustomEvent* c);
	void DecodeImbalance(System::String^ msg, System::String^ symbol, System::String^ source, const md_core::MDImbalance *i);
};

extern "C"
{
	MARKETDATA_API int Initialize(const char *smNames, const char *smType, const char *configlocation, const char *configFile, int depth);
	MARKETDATA_API int Start();
	MARKETDATA_API int Stop();
	MARKETDATA_API int Shutdown();

	MARKETDATA_API int Version();

	MARKETDATA_API int StartSessionManager(const char *smName);

	MARKETDATA_API int Subscribe(const char *symbol, const char *source, bool reload);
	MARKETDATA_API int SubscribeL2(const char *symbol, const char *source, bool reload);
	MARKETDATA_API int SubscribeBook(const char *symbol, const char *source, bool reload);

	MARKETDATA_API int Unsubscribe(const char *symbol); 

	// Callbacks
	typedef void (__stdcall *ptrOnStatusMessage) (System::String^);
	MARKETDATA_API void ReturnStatusMessage(ptrOnStatusMessage out);

	typedef void (__stdcall *ptrOnSRLabMessage) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabMessage(ptrOnSRLabMessage out);

	typedef void (__stdcall *ptrOnSRLabBookWithOrderReplace) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabBookWithOrderReplace(ptrOnSRLabBookWithOrderReplace out);

	typedef void (__stdcall *ptrOnSRLabBook) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabBook(ptrOnSRLabBook out);

	typedef void (__stdcall *ptrOnSRLabOrderReplace) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabOrderReplace(ptrOnSRLabOrderReplace out);

	typedef void (__stdcall *ptrOnSRLabOrder) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabOrder(ptrOnSRLabOrder out);

	typedef void (__stdcall *ptrOnSRLabQuote) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabQuote(ptrOnSRLabQuote out);

	typedef void (__stdcall *ptrOnSRLabTrade) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabTrade(ptrOnSRLabTrade out);

	typedef void (__stdcall *ptrOnSRLabImbalance) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabImbalance(ptrOnSRLabImbalance out);

	typedef void (__stdcall *ptrOnSRLabBestPrice) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabBestPrice(ptrOnSRLabBestPrice out);

	typedef void (__stdcall *ptrOnSRLabCustomEvent) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabCustomEvent(ptrOnSRLabCustomEvent out);

	typedef void (__stdcall *ptrOnSRLabSecurityStatusChange) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabSecurityStatusChange(ptrOnSRLabSecurityStatusChange out);

	typedef void (__stdcall *ptrOnSRLabSubscriptionInvalid) (System::String^, ManagedTypedMap^);
	MARKETDATA_API void ReturnSRLabSubscriptionInvalid(ptrOnSRLabSubscriptionInvalid out);
}
