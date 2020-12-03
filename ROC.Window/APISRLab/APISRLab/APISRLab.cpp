#include "stdafx.h"
#include "APISRLab.h"
#include "CSync.h"

#include <md-core/mdapplication.h>
#include <md-framework/config/configexception.h>
#include <md-framework/resource/resourceexception.h>
#include <md-core/MDSessionMgr.h>
#include <md-core/MDPriceLevel.h>

#include <iomanip>


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

CSync GlobalSync;

bool isShutdown;

#pragma region - Call Back Pointer -

ptrOnStatusMessage gOutStatusMessage;

ptrOnSRLabMessage gOutSRLabMessage;
ptrOnSRLabBookWithOrderReplace gOutSRLabBookWithOrderReplace;
ptrOnSRLabBook gOutSRLabBook;
ptrOnSRLabOrderReplace gOutSRLabOrderReplace;
ptrOnSRLabOrder gOutSRLabOrder;
ptrOnSRLabQuote gOutSRLabQuote;
ptrOnSRLabTrade gOutSRLabTrade;
ptrOnSRLabImbalance gOutSRLabImbalance;
ptrOnSRLabBestPrice gOutSRLabBestPrice;
ptrOnSRLabCustomEvent gOutSRLabCustomEvent;
ptrOnSRLabSecurityStatusChange gOutSRLabSecurityStatusChange;
ptrOnSRLabSubscriptionInvalid gOutSRLabSubscriptionInvalid;

SRLab* ptr_srmd;

md_core::MDApplication *_application;
md_framework::ConfigRP _applicationConfig;
md_framework::ResourceManager* _resourceManager;

// Key=smName
std::map<std::string,md_core::MDSessionMgr*> _mdSessionMgrs;
// Key=symbol,source
std::map<std::string,md_core::InstrumentDef> _mdInstruments;
// KEy=symbol,source Value=""
std::map<std::string,std::string> _symbolInfo;

std::string _configlocation;
std::string _configFile;
std::string _smNames;
std::string _smType;
int _depth;

#pragma endregion 

#pragma region - To Managed -

void OutPutStatusMsg(System::String^ msg)
{
	CLockGuard Gate(GlobalSync);

	if (!isShutdown && msg)
	{
		gOutStatusMessage(msg);
	}
}

#pragma endregion

#pragma region - Init -

int SRLab_Init()
{
	int result = 0;

	OutPutStatusMsg("SRLab_Init : Start...");
	
	try
	{
		_application=md_core::MDApplication::instance();
	}
	catch (const md_framework::ConfigException& e)
	{
		result=1;
		std::string msg="SRLab_Init : md_framework::ConfigException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
		return result;
	}
	catch (const md_framework::ResourceException& e)
	{
		result=1;
		std::string msg="SRLab_Init : md_framework::ResourceException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
		return result;
	}
	catch (...)
	{
		result=1;
		std::string msg="SRLab_Init : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
		return result;
	}

	if (!_application)
	{
		result=1;
		std::string msg="SRLab_Init : Unable to get instance of MDApplication.";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
		return result;
	}

	try
	{
		OutPutStatusMsg("SRLab_Init : ApplicationConfig Loading...");
		_applicationConfig=_application->load(_configlocation,_configFile,__MD_VERSION__);

		//OutPutStatusMsg("SRLab_Init : Logger Getting...");
		//SRLab::_logger = SRLab::_application->getLogger();

		if (_applicationConfig)
		{
			OutPutStatusMsg("SRLab_Init : ResourceManager Getting...");
			_resourceManager = md_core::MDApplication::instance()->getResourceManager();
			if (_resourceManager)
			{
				OutPutStatusMsg("SRLab_Init : Initialized.");
			}
		}
	}
	catch (const md_framework::ConfigException& e)
	{
		result = 1;
		std::string msg="SRLab_Init : md_framework::ConfigException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (const md_framework::ResourceException& e)
	{
		result = 1;
		std::string msg="SRLab_Init : md_framework::ResourceException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (...)
	{
		result = 1;
		std::string msg="SRLab_Init : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	return result;
}

#pragma endregion

#pragma region - Start Session Manager -

int SRLab_StartSessionManager(const char* smName)
{
	int result = 0;

	try
	{
		md_framework::Resource* resource = const_cast<md_framework::Resource*>(_application->getResourceManager()->getResource(_smType, smName));
		if (!resource)
		{
			result=1;
			std::string msg="SRLab_StartSessionManager: No resource defined for type=";
			msg.append(_smType).append(", name=").append(smName);
			OutPutStatusMsg(gcnew System::String(msg.c_str()));
		}
		else
		{
			md_core::MDSessionMgr* mdSM = dynamic_cast<md_core::MDSessionMgr*>(const_cast<md_framework::Resource*> (resource));
			mdSM->appNotify();
			if (mdSM->getAdminMgr())
			{
				mdSM->getAdminMgr()->appNotify();
				std::string msg="SRLab_Start: getAdminMgr->appNotify for name=";
				msg.append(smName);
				OutPutStatusMsg(gcnew System::String(msg.c_str()));
			}

			if (!mdSM)
			{
				result=1;
				std::string msg="SRLab_StartSessionManager: Resource for type=";
				msg.append(_smType).append(", name=").append(smName).append(" should be of Type MDSessionMgr");
				OutPutStatusMsg(gcnew System::String(msg.c_str()));
			}
			else
			{
				std::map<string,md_core::MDSessionMgr*>::iterator it;
				it=_mdSessionMgrs.find(smName);
				if (it==_mdSessionMgrs.end())
				{
					// Key=smName
					_mdSessionMgrs.insert(_mdSessionMgrs.begin(),pair<string,md_core::MDSessionMgr*>(smName,mdSM));

					std::string msg="SRLab_StartSessionManager: SessionMgr for type=";
					msg.append(_smType).append(", name=").append(smName).append(" added");
					OutPutStatusMsg(gcnew System::String(msg.c_str()));
				}
				else
				{
					result=1;
					std::string msg="SRLab_StartSessionManager: SessionMgr for type=";
					msg.append(_smType).append(", name=").append(smName).append(" already existed");
					OutPutStatusMsg(gcnew System::String(msg.c_str()));
				}
			}
		}
	}
	catch (const md_framework::ConfigException &e)
	{
		result = 1;
		std::string msg="SRLab_StartSessionManager : md_framework::ConfigException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (const md_framework::ResourceException &e)
	{
		result = 1;
		std::string msg="SRLab_StartSessionManager : md_framework::ResourceException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (...)
	{
		result = 1;
		std::string msg="SRLab_StartSessionManager : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	return result;
}

#pragma endregion

#pragma region - Start -

int SRLab_Start()
{
	int result = 0;

	try
	{
		std::map<string,md_core::MDSessionMgr*>::iterator it;
		for (it=_mdSessionMgrs.begin();it!=_mdSessionMgrs.end();it++)
		{
			md_core::MDSessionMgr* mdSM=it->second;
			//mdSM->registerAdminListener(ptr_srmd);
			mdSM->start();
			std::string msg="SRLab_Start: for name=";
			msg.append(it->first);
			OutPutStatusMsg(gcnew System::String(msg.c_str()));
		}
	}
	catch (const md_framework::ConfigException &e)
	{
		result = 1;
		std::string msg="SRLab_Start : md_framework::ConfigException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (const md_framework::ResourceException &e)
	{
		result = 1;
		std::string msg="SRLab_Start : md_framework::ResourceException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (...)
	{
		result = 1;
		std::string msg="SRLab_Start : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	return result;
}

#pragma endregion

#pragma region - Unsubscribe -

int SRLab_Unsubscribe(std::string symbol, std::string source)
{
	int result = 0;

	bool scanAllSMs = false;
	bool scanAllIDs = false;
	bool shouldUnsub = false;

	try
	{
		//CLockGuard Gate(GlobalSync);

		if (_mdSessionMgrs.empty())
		{
			result = 1;
			std::string msg="SRLab_Unsubscribe: No Sessions Found";
			OutPutStatusMsg(gcnew System::String(msg.c_str()));
			return result;
		}

		if (_mdInstruments.empty())
		{
			result = 1;
			std::string msg="SRLab_Unsubscribe: No Instruments Found";
			OutPutStatusMsg(gcnew System::String(msg.c_str()));
			return result;
		}

		std::map<string,md_core::MDSessionMgr*>::iterator itSM;
		if (source=="")
		{
			scanAllSMs = true;
		}
		else
		{
			itSM=_mdSessionMgrs.find(source);
			if (itSM==_mdSessionMgrs.end())
			{
				scanAllSMs = true;
			}
		}

		std::string key="";
		std::map<string,md_core::InstrumentDef>::iterator itID;
		if (scanAllSMs)
		{
			for (itSM=_mdSessionMgrs.begin();itSM!=_mdSessionMgrs.end();itSM++)
			{
				key=symbol;
				key.append(",").append(itSM->first);
				itID=_mdInstruments.find(key);

				if (itID!=_mdInstruments.end())
				{
					source = itSM->first;
					shouldUnsub = true;
					break;
				}
			}
		}
		else
		{
			key=symbol;
			key.append(",").append(source);
			itID=_mdInstruments.find(key);
			if (itID!=_mdInstruments.end())
			{
				shouldUnsub = true;
			}
		}

		if (shouldUnsub && source != "")
		{
			itSM=_mdSessionMgrs.find(source);
			if (itSM!=_mdSessionMgrs.end())
			{
				md_core::MDSessionMgr* mdSM=itSM->second;
				mdSM->unsubscribe(itID->second);
				_mdInstruments.erase(itID);

				std::map<std::string,std::string>::iterator itSI=_symbolInfo.find(key);
				if (itSI!=_symbolInfo.end())
				{
					_symbolInfo.erase(itSI);
				}		

				std::string msg="SRLab_Unsubscribe: for key=";
				msg.append(key).append(" Removed");
				OutPutStatusMsg(gcnew System::String(msg.c_str()));
			}
		}
		else
		{
			result = 1;
			std::string msg="SRLab_Unsubscribe: for symbol=";
			msg.append(symbol).append( "source=").append(source).append(" Not found");
			OutPutStatusMsg(gcnew System::String(msg.c_str()));
		}
	}
	catch (const md_framework::ConfigException &e)
	{
		result = 1;
		std::string msg="SRLab_Unsubscribe : md_framework::ConfigException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (const md_framework::ResourceException &e)
	{
		result = 1;
		std::string msg="SRLab_Unsubscribe : md_framework::ResourceException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (...)
	{
		result = 1;
		std::string msg="SRLab_Unsubscribe : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	return result;
}
int SRLab_Unsubscribe(std::string symbol)
{
	return SRLab_Unsubscribe(symbol, "");
}

#pragma endregion

#pragma region - Version -

int SRLab_Version()
{
	int result = 0;

	try
	{
		std::string msg="SRLab_Init : Version ";
		msg.append(__MD_VERSION__);
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch(...)
	{
		result = 1;
		std::string msg="SRLab_Init : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	return result;
}

#pragma endregion

#pragma region - Subscribe -

int SRLab_Subscribe(std::string symbol, std::string source, bool reload)
{
	int result = 0;
	bool shouldSub = false;

	try
	{
		//CLockGuard Gate(GlobalSync);

		if (_mdSessionMgrs.empty())
		{
			result = 1;
			std::string msg="SRLab_Subscribe: No Sessions Found";
			OutPutStatusMsg(gcnew System::String(msg.c_str()));
			return result;
		}

		map<string,md_core::MDSessionMgr*>::iterator itSM;
		itSM=_mdSessionMgrs.find(source);
		if (itSM==_mdSessionMgrs.end())
		{
			result=1;
			std::string msg="SRLab_Subscribe: for source=";
			msg.append(source).append(", name=").append(symbol).append(" missing source");
			OutPutStatusMsg(gcnew System::String(msg.c_str()));
		}
		else
		{
			// Have Source
			md_core::MDSessionMgr* mdSM=itSM->second;
			
			map<string,md_core::InstrumentDef>::iterator itID;
			std::string key=symbol;
			key.append(",").append(source);
			itID=_mdInstruments.find(key);
			if (itID==_mdInstruments.end())
			{
				shouldSub = true;
			}
			else
			{
				if (reload)
				{
					// Reload
					SRLab_Unsubscribe(symbol, source);
					shouldSub = true;
				}
				else
				{
					result=1;
					std::string msg="SRLab_Subscribe: for key=";
					msg.append(key).append(" already exist");
					OutPutStatusMsg(gcnew System::String(msg.c_str()));
				}
			}

			if (shouldSub)
			{
				md_core::InstrumentDef inst(md_core::InstrumentDef::EXCH_UNKNOWN, source.c_str(), symbol.c_str(), "");
				_mdInstruments.insert(_mdInstruments.begin(),pair<string,md_core::InstrumentDef>(key,inst));
				
				std::string msg="SRLab_Subscribe: for key=";
				msg.append(key).append(" Added");
				OutPutStatusMsg(gcnew System::String(msg.c_str()));

				mdSM->subscribe(inst, ptr_srmd, 0);
			}
		}
	}
	catch (const md_framework::ConfigException &e)
	{
		result = 1;
		std::string msg="SRLab_Subscribe : md_framework::ConfigException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (const md_framework::ResourceException &e)
	{
		result = 1;
		std::string msg="SRLab_Subscribe : md_framework::ResourceException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (...)
	{
		result = 1;
		std::string msg="SRLab_Subscribe : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	return result;
}

#pragma endregion

#pragma region - Stop -

int SRLab_Stop()
{
	int result = 0;

	try
	{
		if (_mdSessionMgrs.empty())
		{
			result = 1;
			std::string msg="SRLab_Stop: No Sessions Found";
			OutPutStatusMsg(gcnew System::String(msg.c_str()));
			return result;
		}
		else
		{
			map<string,md_core::MDSessionMgr*>::iterator itSM;
			if (_mdInstruments.empty())
			{
				std::string msg="SRLab_Stop: No Instruments Found";
				OutPutStatusMsg(gcnew System::String(msg.c_str()));
			}
			else
			{
				map<string,md_core::InstrumentDef>::iterator itID;
				for (itID=_mdInstruments.begin();itID!=_mdInstruments.end();itID++)
				{
					md_core::InstrumentDef mdID=itID->second;
					if (!isShutdown)
					{
						SRLab_Unsubscribe(mdID.symbol(), mdID.source());
					}
				}
			}

			for (itSM=_mdSessionMgrs.begin();itSM!=_mdSessionMgrs.end();itSM++)
			{
				md_core::MDSessionMgr* mdSM=itSM->second;
				mdSM->stop();

				std::string msg="SRLab_Stop: souce=";
				msg.append(itSM->first);
				OutPutStatusMsg(gcnew System::String(msg.c_str()));
			}
		}
	}
	catch (const md_framework::ConfigException &e)
	{
		result = 1;
		std::string msg="SRLab_Stop : md_framework::ConfigException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (const md_framework::ResourceException &e)
	{
		result = 1;
		std::string msg="SRLab_Stop : md_framework::ResourceException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (...)
	{
		result = 1;
		std::string msg="SRLab_Stop : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	return result;
}

#pragma endregion

#pragma region - Shutdown -

int SRLab_Shutdown()
{
	int result = 0;

	result = SRLab_Stop();
	try
	{
		if (_application)
		{
			_application->getResourceManager()->shutdown();
			std::string msg="SRLab_Shutdown : Application.Shutdown ";
			OutPutStatusMsg(gcnew System::String(msg.c_str()));
		}
	}
	catch (const md_framework::ConfigException &e)
	{
		result = 1;
		std::string msg="SRLab_Shutdown : md_framework::ConfigException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (const md_framework::ResourceException &e)
	{
		result = 1;
		std::string msg="SRLab_Shutdown : md_framework::ResourceException ";
		msg.append(e.what());
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}
	catch (...)
	{
		result = 1;
		std::string msg="SRLab_Shutdown : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	return result;
}

#pragma endregion

#pragma region - Decode -

void SRLab::DecodeSub(System::String^ msg, System::String^ symbol, System::String^ source, const md_core::MDSubscription* sub)
{
	//CLockGuard Gate(GlobalSync);
	//ManagedTypedMap^ delta;

	try
	{
		ManagedTypedMap^ delta = gcnew ManagedTypedMap();
		delta->Update(TypedFieldID::SubscriptionSymbol, symbol);
		delta->Update(TypedFieldID::SubscriptionSoruce, source);

		md_core::MDTrade *t=sub->trade();
		if (t)
		{
			delta->Update(TypedFieldID::TradePrice, t->_last);
			delta->Update(TypedFieldID::TradeVolume, t->_lastSize);
			delta->Update(TypedFieldID::TradePartId, t->_compositeParticipantID);
		}

		md_core::MDQuote *q=sub->quote();
		if (q)
		{
			delta->Update(TypedFieldID::BidPrice, q->_bid);
			delta->Update(TypedFieldID::BidSize, q->_bidSize);
			delta->Update(TypedFieldID::BidPartId, q->_bexchange);

			delta->Update(TypedFieldID::AskPrice, q->_ask);
			delta->Update(TypedFieldID::AskSize, q->_askSize);
			delta->Update(TypedFieldID::AskPartId, q->_oexchange);
		}

		if(delta->InstrumentType=="F")
		{
			md_core::MDOrderBook *book=sub->book();

			if (book)
			{
				int buySize = book->getBuys()->getCurrentSize();
				for (int i = 0; i < buySize; i++)
				{
					const md_core::MDPriceLevelInternal *buy=book->getBuys()->book()->getBuyLevel(i);
					if (i==0)
					{
						delta->Update(TypedFieldID::BidPrice,buy->_px);
						delta->Update(TypedFieldID::BidSize,buy->_size);
					}
				}

				int sellsize = book->getSells()->getCurrentSize();
				for (int i = 0; i < sellsize; i++)
				{
					const md_core::MDPriceLevelInternal *sell=book->getSells()->book()->getSellLevel(i);
					if (i==0)
					{
						delta->Update(TypedFieldID::AskPrice,sell->_px);
						delta->Update(TypedFieldID::AskSize,sell->_size);
					}
				}
			}
		}

		gOutSRLabMessage(msg, delta);
		delete(delta);
	}
	catch(...)
	{
		std::string msg="DecodeSub : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	//delete(delta);
	//return delta;
}
void SRLab::DecodeTrade(System::String^ msg, System::String^ symbol, System::String^ source, const md_core::MDTrade* t)
{
	//CLockGuard Gate(GlobalSync);
	//ManagedTypedMap^ delta;

	try
	{
		if (t)
		{
			ManagedTypedMap^ delta = gcnew ManagedTypedMap();
			delta->Update(TypedFieldID::SubscriptionSymbol, symbol);
			delta->Update(TypedFieldID::SubscriptionSoruce, source);

			delta->Update(TypedFieldID::TradePrice, t->_last);
			delta->Update(TypedFieldID::TradeVolume, t->_lastSize);
			delta->Update(TypedFieldID::TradePartId, t->_compositeParticipantID);

			gOutSRLabTrade(msg, delta);
			delete(delta);
		}
	}
	catch(...)
	{
		std::string msg="DecodeTrade : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	//delete(delta);
	//return delta;
}
void SRLab::DecodeQuote(System::String^ msg, System::String^ symbol, System::String^ source, const md_core::MDQuote* q)
{
	//CLockGuard Gate(GlobalSync);
	//ManagedTypedMap^ delta;
	
	try
	{
		if (q)
		{
			ManagedTypedMap^ delta = gcnew ManagedTypedMap();
			delta->Update(TypedFieldID::SubscriptionSymbol, symbol);
			delta->Update(TypedFieldID::SubscriptionSoruce, source);

			delta->Update(TypedFieldID::BidPrice, q->_bid);
			delta->Update(TypedFieldID::BidSize, q->_bidSize);
			delta->Update(TypedFieldID::BidPartId, q->_bexchange);

			delta->Update(TypedFieldID::AskPrice, q->_ask);
			delta->Update(TypedFieldID::AskSize, q->_askSize);
			delta->Update(TypedFieldID::AskPartId, q->_oexchange);

			gOutSRLabQuote(msg, delta);
			delete(delta);
		}
	}
	catch(...)
	{
		std::string msg="DecodeTrade : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	//delete(delta);
	//return delta;
}
void SRLab::DecodeBook(System::String^ msg, System::String^ symbol,System::String^ source, const md_core::MDOrderBook* book)
{
	//CLockGuard Gate(GlobalSync);
	//ManagedTypedMap^ delta;

	try
	{
		if (book)
		{
			ManagedTypedMap^ delta = gcnew ManagedTypedMap();
			delta->Update(TypedFieldID::SubscriptionSymbol, symbol);
			delta->Update(TypedFieldID::SubscriptionSoruce, source);

			if (book->hasBuyUpdates())
			{
				int buySize = book->getBuys()->getCurrentSize();
				for (int i = 0; i < buySize; i++)
				{
					const md_core::MDPriceLevelInternal *buy=book->getBuys()->book()->getBuyLevel(i);
					if (i==0)
					{
						delta->Update(TypedFieldID::BidPrice,buy->_px);
						delta->Update(TypedFieldID::BidSize,buy->_size);
					}
				}
			}

			if (book->hasSellUpdates())
			{
				int sellsize = book->getSells()->getCurrentSize();
				for (int i = 0; i < sellsize; i++)
				{
					const md_core::MDPriceLevelInternal *sell=book->getSells()->book()->getSellLevel(i);
					if (i==0)
					{
						delta->Update(TypedFieldID::AskPrice,sell->_px);
						delta->Update(TypedFieldID::AskSize,sell->_size);
					}
				}
			}

			gOutSRLabBook(msg, delta);
			delete(delta);
		}
	}
	catch(...)
	{
		std::string msg="DecodeTrade : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	//delete(delta);
	//return delta;
}
void SRLab::DecodeCustom(System::String^ msg, System::String^ symbol, System::String^ source, const md_core::MDCustomEvent* c)
{
}
void SRLab::DecodeImbalance(System::String^ msg, System::String^ symbol, System::String^ source, const md_core::MDImbalance *i)
{
}
static bool CriticalCheck(const std::string symbol, const std::string source)
{
	CLockGuard Gate(GlobalSync);

	bool isCritical=false;

	try
	{
		std::string key=symbol;
		key.append(",").append(source);

		std::map<std::string,std::string>::iterator itSI=_symbolInfo.find(key);
		if (itSI==_symbolInfo.end())
		{
			isCritical=true;
			_symbolInfo.insert(_symbolInfo.begin(), pair<std::string,std::string>(key, symbol));
		}
	}
	catch(...)
	{
		std::string msg="CriticalCheck : Unknown exception ";
		OutPutStatusMsg(gcnew System::String(msg.c_str()));
	}

	return isCritical;
}

#pragma endregion

#pragma endregion

#pragma region - MDListener Call Back -

void SRLab::onBookWithOrderReplace (const md_core::MDSubscription* sub, const md_core::MDOrderBook* book,const md_core::MDOrder* oldorder, const md_core::MDOrder* neworder)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string symbol = sub->instrument().symbol();
	std::string source = sub->instrument().source();
	bool isCritical=CriticalCheck(symbol, source);

	std::string msg="SRLab_Listener::onBookWithOrderReplace symbol=";
	msg.append(symbol).append(" source=").append(source);

	if (isCritical)
	{
		DecodeSub(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			sub);
	}
	else
	{
		DecodeBook(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			book);
	}
}

void SRLab::onBook (const md_core::MDSubscription* sub, const md_core::MDOrderBook* book,const md_core::MDOrder* cause, const md_core::MDTrade * trade)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string symbol = sub->instrument().symbol();
	std::string source = sub->instrument().source();
	bool isCritical=CriticalCheck(symbol, source);

	std::string msg="SRLab_Listener::onBook symbol=";
	msg.append(symbol).append(" source=").append(source);

	if (isCritical)
	{
		DecodeSub(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			sub);
	}
	else
	{
		DecodeBook(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			book);
	}
}

void SRLab::onOrderReplace (const md_core::MDSubscription *sub, const md_core::MDOrder *oldOrder, const md_core::MDOrder *newOrder)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string symbol = sub->instrument().symbol();
	std::string source = sub->instrument().source();
	bool isCritical=CriticalCheck(symbol, source);

	std::string msg="SRLab_Listener::onOrderReplace symbol=";
	msg.append(symbol).append(" source=").append(source);

	if (isCritical)
	{
		DecodeSub(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			sub);
	}
	else
	{
	}
}

void SRLab::onOrder (const md_core::MDSubscription *sub, const md_core::MDOrder *o, const md_core::MDTrade *trade)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string symbol = sub->instrument().symbol();
	std::string source = sub->instrument().source();
	bool isCritical=CriticalCheck(symbol, source);

	std::string msg="SRLab_Listener::onOrder symbol=";
	msg.append(symbol).append(" source=").append(source);

	if (isCritical)
	{
		DecodeSub(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			sub);
	}
	else
	{
		DecodeTrade(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			trade);
	}
}

void SRLab::onQuote (const md_core::MDSubscription* sub,const md_core::MDQuote* q)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string symbol = sub->instrument().symbol();
	std::string source = sub->instrument().source();
	bool isCritical=CriticalCheck(symbol, source);

	std::string msg="SRLab_Listener::onQuote symbol=";
	msg.append(symbol).append(" source=").append(source);

	if (isCritical)
	{
		DecodeSub(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			sub);
	}
	else
	{
		DecodeQuote(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			q);
	}
}

void SRLab::onTrade (const md_core::MDSubscription* sub,const md_core::MDTrade* t)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string symbol = sub->instrument().symbol();
	std::string source = sub->instrument().source();
	bool isCritical=CriticalCheck(symbol, source);
	
	std::string msg="SRLab_Listener::onTrade symbol=";
	msg.append(symbol).append(" source=").append(source);

	if (isCritical)
	{
		DecodeSub(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			sub);
	}
	else
	{
		DecodeTrade(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			t);
	}
}

void SRLab::onImbalance (const md_core::MDSubscription *sub, const md_core::MDImbalance *i)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string symbol = sub->instrument().symbol();
	std::string source = sub->instrument().source();
	bool isCritical=CriticalCheck(symbol, source);

	std::string msg="SRLab_Listener::onImbalance symbol=";
	msg.append(symbol).append(" source=").append(source);

	if (isCritical)
	{
		DecodeSub(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			sub);
	}
	else
	{
	}
}

void SRLab::onBestPrice (const md_core::MDSubscription* sub,const md_core::MDQuote* q)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string symbol = sub->instrument().symbol();
	std::string source = sub->instrument().source();
	bool isCritical=CriticalCheck(symbol, source);

	std::string msg="SRLab_Listener::onBestPrice symbol=";
	msg.append(symbol).append(" source=").append(source);

	if (isCritical)
	{
		DecodeSub(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			sub);
	}
	else
	{
		DecodeQuote(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			q);
	}
}

void SRLab::onCustomEvent(const md_core::MDSubscription* sub, const md_core::MDCustomEvent* c)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string symbol = sub->instrument().symbol();
	std::string source = sub->instrument().source();
	bool isCritical=CriticalCheck(symbol, source);

	std::string msg="SRLab_Listener::onCustomEvent symbol=";
	msg.append(symbol).append(" source=").append(source);

	if (isCritical)
	{
		DecodeSub(
			gcnew System::String(msg.c_str()), 
			gcnew System::String(symbol.c_str()), 
			gcnew System::String(source.c_str()), 
			sub);
	}
	else
	{
	}
}

void SRLab::onSecurityStatusChange(const md_core::InstrumentDef* instrument)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string msg="SRLab_Listener::onSecurityStatusChange ";
	gOutSRLabMessage(
		gcnew System::String(msg.c_str()), 
		gcnew ManagedTypedMap());
}

void SRLab::onSubscriptionInvalid (const md_core::MDSubscription *sub, const char *reason)
{
	md_framework::SimpleLock lk(_srMutex);
	SRLab_Unsubscribe(sub->instrument().symbol(), sub->instrument().source());
}

//Alerts
void SRLab::onTradingIndicationAlert (const md_core::MDSubscription *sub,const md_core::MDMktTradingIndicationAlert* alert)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string msg="SRLab_Listener::onTradingIndicationAlert ";
	gOutSRLabMessage(
		gcnew System::String(msg.c_str()), 
		gcnew ManagedTypedMap());
}

void SRLab::onTradeDisseminationTimeAlert (const md_core::MDSubscription *sub,const md_core::MDMktTradeDisseminationTimeAlert* alert)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string msg="SRLab_Listener::onTradeDisseminationTimeAlert ";
	gOutSRLabMessage(
		gcnew System::String(msg.c_str()), 
		gcnew ManagedTypedMap());
}

//Admin Listener
void SRLab::onPlaybackComplete(md_core::MDSessionMgr* sm)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string msg="SRLab_Listener::onPlaybackComplete ";
	gOutSRLabMessage(
		gcnew System::String(msg.c_str()), 
		gcnew ManagedTypedMap());
}

void SRLab::onGap (md_core::MDSessionMgr* sm,const char* connection,const char* line, unsigned long long prevSNO, unsigned long long curSNO,bool isRecoveryEnabled)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string msg="SRLab_Listener::onGap ";
	gOutSRLabMessage(
		gcnew System::String(msg.c_str()), 
		gcnew ManagedTypedMap());
}

void SRLab::onGapFill(md_core::MDSessionMgr* sm,const char* connection,const char* line, unsigned long long fromSNO, unsigned long long curSNO, bool isFullyFilled)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string msg="SRLab_Listener::onGapFill ";
	gOutSRLabMessage(
		gcnew System::String(msg.c_str()), 
		gcnew ManagedTypedMap());
}

void SRLab::onSlowConsumer(md_core::MDSessionMgr* sm,const char* connection,const char* line, int numEventsSampled,int numEventsAboveThreshold,
					   long long maxUserTimeInMicros, long long minUserTimeInMicros)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string msg="SRLab_Listener::onSlowConsumer ";
	gOutSRLabMessage(
		gcnew System::String(msg.c_str()), 
		gcnew ManagedTypedMap());
}

void SRLab::onMDRCForcedDisconnect(md_core::MDSessionMgr*, const char* connection,const char* line, const char* reason)
{
	md_framework::SimpleLock lk(_srMutex);

	std::string msg="SRLab_Listener::onMDRCForcedDisconnect ";
	gOutSRLabMessage(
		gcnew System::String(msg.c_str()), 
		gcnew ManagedTypedMap());
}

#pragma endregion

extern "C"
{
	MARKETDATA_API int Initialize(const char *smNames, const char *smType, const char *configlocation, const char *configFile, const int depth)
	{
		if (!ptr_srmd)
		{
			ptr_srmd=new SRLab();
		}
		_configlocation = configlocation;
		_configFile = configFile;
		_smNames = smNames;
		_smType = smType;
		_depth = depth;

		return SRLab_Init();
	}

	MARKETDATA_API int Version()
	{
		return SRLab_Version();
	}

	MARKETDATA_API int StartSessionManager(const char *smName)
	{
		return SRLab_StartSessionManager(smName);
	}

	MARKETDATA_API int Start()
	{
		return SRLab_Start();
	}

	MARKETDATA_API int Subscribe(const char *symbol, const char *source, bool reload)
	{
		return SRLab_Subscribe(symbol, source, reload);
	}

	MARKETDATA_API int SubscribeL2(const char *symbol, const char *source, bool reload)
	{
		return SRLab_Subscribe(symbol, source, reload);
	}

	MARKETDATA_API int SubscribeBook(const char *symbol, const char *source, bool reload)
	{
		return SRLab_Subscribe(symbol, source, reload);
	}

	MARKETDATA_API int Unsubscribe(const char *symbol)
	{
		return SRLab_Unsubscribe(symbol);
	}

	MARKETDATA_API int Stop()
	{
		return SRLab_Stop();
	}

	MARKETDATA_API int Shutdown()
	{
		isShutdown=true;
		return SRLab_Shutdown();
	}

	MARKETDATA_API void ReturnStatusMessage(ptrOnStatusMessage out)
	{
		gOutStatusMessage=out;
	}

	MARKETDATA_API void ReturnSRLabMessage(ptrOnSRLabMessage out)
	{
		gOutSRLabMessage=out;
	}

	MARKETDATA_API void ReturnSRLabBookWithOrderReplace(ptrOnSRLabBookWithOrderReplace out)
	{
		gOutSRLabBookWithOrderReplace=out;
	}

	MARKETDATA_API void ReturnSRLabBook(ptrOnSRLabBook out)
	{
		gOutSRLabBook=out;
	}

	MARKETDATA_API void ReturnSRLabOrderReplace(ptrOnSRLabOrderReplace out)
	{
		gOutSRLabOrderReplace=out;
	}

	MARKETDATA_API void ReturnSRLabOrder(ptrOnSRLabOrder out)
	{
		gOutSRLabOrder=out;
	}

	MARKETDATA_API void ReturnSRLabQuote(ptrOnSRLabQuote out)
	{
		gOutSRLabQuote=out;
	}

	MARKETDATA_API void ReturnSRLabTrade(ptrOnSRLabTrade out)
	{
		gOutSRLabTrade=out;
	}

	MARKETDATA_API void ReturnSRLabImbalance(ptrOnSRLabImbalance out)
	{
		gOutSRLabImbalance=out;
	}

	MARKETDATA_API void ReturnSRLabBestPrice(ptrOnSRLabBestPrice out)
	{
		gOutSRLabBestPrice=out;
	}

	MARKETDATA_API void ReturnSRLabCustomEvent(ptrOnSRLabCustomEvent out)
	{
		gOutSRLabCustomEvent=out;
	}

	MARKETDATA_API void ReturnSRLabSecurityStatusChange(ptrOnSRLabSecurityStatusChange out)
	{
		gOutSRLabSecurityStatusChange=out;
	}

	MARKETDATA_API void ReturnSRLabSubscriptionInvalid(ptrOnSRLabSubscriptionInvalid out)
	{
		gOutSRLabSubscriptionInvalid=out;
	}
}