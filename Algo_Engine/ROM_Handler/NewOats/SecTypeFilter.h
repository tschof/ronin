#if !defined(_SECTYPEFILTER_H)
#define _SECTYPEFILTER_H

#include "OrderMsg.h"
#include "RomCsv.h"
#include "RomValueDefs.h"

class CSecTypeFilter
{
public:

	enum MODE
	{
		ALL,					//0
		EQUITY,					//1
		OPTION,					//2
		FUTURE,					//3
		FOREX,					//4
		EQUITY_FUTURE,			//5
		EQUITY_OPTION,			//6
		EQUITY_FOREX,			//7
		OPTION_FUTURE,			//8
		OPTION_FOREX,			//9
		FUTURE_FOREX,			//10
		EQUITY_FUTURE_FOREX,	//11
		EQUITY_OPTION_FOREX,	//12
		FUTURE_OPTION_FOREX,	//13
		EQUITY_FUTURE_OPTION,	//14
		NOTHING,				//15
	};

	static bool IsComplexOrder(const CRomCsv& csv)
	{
		bool rc = false;

		if (csv.IsFieldEmpty(RFIX::CSV_PARENT_STAFFORD_TAG) == false ||
			csv.nGet(RFIX::CSV_CPLXORDER_TYPE) != ROMVALUE::CPLXTYPE_SINGLE)
		{
			rc = true;
		}

		return rc;
	}

	static bool Block(char secType, int mode)
	{
		bool hit = false;

		switch (mode)
		{
		case EQUITY:
			hit = COrderMsg::IsEquityType(secType);
			break;
		case OPTION:
			hit = COrderMsg::IsOptionType(secType);
			break;
		case FUTURE:
			hit = COrderMsg::IsFutureType(secType);
			break;
		case FOREX:
			hit = COrderMsg::IsCurrencyType(secType);
			break;
		case EQUITY_FUTURE:
			hit = COrderMsg::IsEquityType(secType) || 
				 COrderMsg::IsFutureType(secType);
			break;
		case EQUITY_OPTION:
			hit = COrderMsg::IsEquityType(secType) || 
				 COrderMsg::IsOptionType(secType);
			break;
		case EQUITY_FOREX:
			hit = COrderMsg::IsEquityType(secType) || 
				 COrderMsg::IsOptionType(secType);
			break;
		case OPTION_FUTURE:
			hit = COrderMsg::IsOptionType(secType) || 
				 COrderMsg::IsFutureType(secType);
			break;
		case OPTION_FOREX:
			hit = COrderMsg::IsOptionType(secType) ||
				 COrderMsg::IsCurrencyType(secType);
			break;
		case FUTURE_FOREX:
			hit = COrderMsg::IsFutureType(secType) || 
				 COrderMsg::IsCurrencyType(secType);
			break;
		case EQUITY_FUTURE_FOREX:
			hit = COrderMsg::IsEquityType(secType) || 
				 COrderMsg::IsFutureType(secType) ||
				 COrderMsg::IsCurrencyType(secType);
			break;
		case EQUITY_OPTION_FOREX:
			hit = COrderMsg::IsEquityType(secType) || 
				 COrderMsg::IsOptionType(secType) ||
				 COrderMsg::IsCurrencyType(secType);
			break;
		case FUTURE_OPTION_FOREX:
			hit = COrderMsg::IsOptionType(secType) || 
				 COrderMsg::IsFutureType(secType) ||
				 COrderMsg::IsCurrencyType(secType);
			break;
		case EQUITY_FUTURE_OPTION:
			hit = COrderMsg::IsOptionType(secType) || 
				 COrderMsg::IsFutureType(secType) ||
				 COrderMsg::IsEquityType(secType);
			break;
		case ALL:
			hit = true;
			break;
		default:
			break;
		}

		return !hit;
	}
};


	


#endif // _SECTYPEFILTER_H
