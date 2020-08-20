// OMOrder.h: interface for the OMOrder class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_OMORDER_H__DEBA50E7_9575_4939_9EA0_242398D047B6__INCLUDED_)
#define AFX_OMORDER_H__DEBA50E7_9575_4939_9EA0_242398D047B6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "FIX42_Messages.h"
#include "Field.h"
#include "Values.h"

namespace FIX
{
	USER_DEFINE_INT(OMExchange,10001);
	USER_DEFINE_STRING(SSFirm, 5700);
}


namespace FIX42
{
	class OMOrder : public NewOrderSingle  
	{
	public:
		OMOrder();
		virtual ~OMOrder();

		void setExchange( const FIX::OMExchange & field )             
		{ 
			setField(field);
		}               

		void UpdateStatus( const ExecutionReport & report );

		FIX::OrdStatus	getStatus	() { return m_Status; }
		void			setStatus	( const FIX::OrdStatus & status ) { m_Status = status; }

	private:
		FIX::OrdStatus m_Status;
	};
}
#endif // !defined(AFX_OMORDER_H__DEBA50E7_9575_4939_9EA0_242398D047B6__INCLUDED_)
