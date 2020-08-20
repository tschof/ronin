#ifndef _ISE_HANDLER_
#define _ISE_HANDLER_

#include "Exchangehandler.h"
#include "optfixsession.h"
#include "time.h"

struct structSenderSubID{
	char	strSenderSubID[10];
	int		nOrdersPerSecond;
	char	strSecondarySenderSubID[10];
	clock_t	timestamp;
	int		nPrimarySenderSubIDCnt;
	int		nSecondarySenderSubIDCnt;
};

typedef std::map<CString, structSenderSubID> SenderSubIDMap;

class CISEHandler : public CExchangeHandler
{
public:
	CISEHandler();

	virtual ~CISEHandler()
	{
		delete m_pTags;
		delete m_orderTypeValidator;
		m_clrFirmSenderIDMap.clear();
	}

protected:
	//spread order processing
	inline BOOL OnProcessSpread(OrderVector& orderVector)
	{
		/*int nLength = orderVector.size();
		for(int i = 0; (i < nLength) && (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, orderVector[i]->OrderTag())); i++)
		{
			orderVector[i]->SetSourceId(m_senderSubID);
			orderVector[i]->SetSecurityType(COrder::Option);
		}
		
		BOOL result = ((COptFixSession*)&(m_pApp->Exchange()))->ProcessSpread(orderVector);
		
		for(i = 0; (i < nLength) && (m_pTags->GetTags(MAP_STAFFORD_TAG_QUERY, orderVector[i]->OrderTag())); i++)
		{
			orderVector[i]->SetSourceId(m_pTags->Client());
		}
		
		return result;*/
		return FALSE;
	}

	inline BOOL OnProcessOrder(COrder& Order)
	{
		//Order.SetSourceId(m_senderSubID);
		//long SeqNum = ( m_pApp->Exchange()).GetNextOutgoingSeqNum();
		
		SenderSubIDMap::iterator theIterator;
		theIterator = m_SenderSubIDMap.find(Order.Trader());
		
		if(theIterator != m_SenderSubIDMap.end())													//if SenderSubID Map exists for trader in dat file
		{
			if( (((*theIterator).second).nOrdersPerSecond != -1)  )									//orders per second limit exists 
			{
				clock_t tmclock = clock();
				if( (tmclock - ((*theIterator).second).timestamp) > ((clock_t)1 * CLOCKS_PER_SEC ))	//if time diff is greater than 1 second then send primary SenderSubID
				{
					Order.SetSourceId( ((*theIterator).second).strSenderSubID );
					Order.SetOrderTag( ((*theIterator).second).strSenderSubID + Order.OrderTag());
					((*theIterator).second).timestamp = tmclock;
					((*theIterator).second).nPrimarySenderSubIDCnt = 1;
				}
				else if( ((*theIterator).second).nPrimarySenderSubIDCnt < ((*theIterator).second).nOrdersPerSecond )//if time diff is less than one second and orders sent within this secon do not execeed nOrdersPerSecond i.e. 3 Sec, then send primary SenderSubID
				{
					Order.SetSourceId( ((*theIterator).second).strSenderSubID );
					Order.SetOrderTag( ((*theIterator).second).strSenderSubID + Order.OrderTag());
					((*theIterator).second).nPrimarySenderSubIDCnt++;
				}
				else																				//then send secondary SenderSubID
				{
					Order.SetSourceId( ((*theIterator).second).strSecondarySenderSubID );
					Order.SetOrderTag( ((*theIterator).second).strSecondarySenderSubID + Order.OrderTag());
					((*theIterator).second).nSecondarySenderSubIDCnt++;
					
					//if more than three have been sent on the secondary sendersubid then toggle back to primary
					if ((tmclock - ((*theIterator).second).timestamp) < ((clock_t)1 * CLOCKS_PER_SEC ) 
					 && (((*theIterator).second).nSecondarySenderSubIDCnt >= ((*theIterator).second).nOrdersPerSecond) )
					{
						((*theIterator).second).timestamp = tmclock;
						((*theIterator).second).nPrimarySenderSubIDCnt = 0;
						((*theIterator).second).nSecondarySenderSubIDCnt = 0;
					}
				}
			}
			else
			{
				Order.SetSourceId( ((*theIterator).second).strSenderSubID );						//then send primary SenderSubID
				Order.SetOrderTag( ((*theIterator).second).strSenderSubID + Order.OrderTag());
			}
		}
		else 
		{
			bool useDefault = true;
			int clrId = 0;
			const char* clrIdStr = Order.lpszClearingId();
			if ( clrIdStr)
				clrId = atoi(clrIdStr);
			if ( clrId > 0 )
			{
				std::map<int, std::string>::iterator clrIdItor = m_clrFirmSenderIDMap.find(clrId);
				if (clrIdItor != m_clrFirmSenderIDMap.end() )
				{					//then use the map
					useDefault = false;
					const char* l_subID = (*clrIdItor).second.c_str();
					Order.SetSourceId(l_subID);
					Order.SetOrderTag(CString(l_subID) + Order.OrderTag());
				}
			}
			if ( useDefault )				//then send default SenderSubID i.e. TDX
			{
				Order.SetSourceId(m_senderSubID);
				Order.SetOrderTag(m_senderSubID + Order.OrderTag());
			}
		}
		
		Order.SetSecurityType(COrder::Option);
		
		BOOL result = ((COptFixSession*)&(m_pApp->Exchange()))->ProcessEntry(Order);
		
		Order.SetSourceId(m_pTags->Client());
		return result;
	}

	inline BOOL OnProcessCancel(CCancel& Cancel)
	{
		//Cancel.SetSourceId(m_senderSubID);
		if(-1 != (Cancel.OrderTag().Left(5)).Find("20040"))//to handle old ClOrdID beginning with "20040...."
		{
			Cancel.SetSourceId(m_senderSubID);//Original "TDX" will be sent for these orders but does not need to be sent
			Cancel.SetCancelTag(m_senderSubID + Cancel.CancelTag());
		}
		else	
		{
			Cancel.SetSourceId(Cancel.OrderTag().Left(3));
			Cancel.SetCancelTag(Cancel.OrderTag().Left(3) + Cancel.CancelTag());
		}

		Cancel.SetLeavesQty(m_pTags->LeavesShares());
		Cancel.SetShares(m_pTags->LeavesShares());//TEMP
		//Cancel.SetShares(m_pTags->OriginalShares());//TEMP
		
		BOOL result = ((COptFixSession*)&(m_pApp->Exchange()))->ProcessCancel(Cancel); 
		
		Cancel.SetSourceId(m_pTags->Client());
		return result;
	}

	inline BOOL OnProcessReplace(CReplace& Replace)
	{
		//Replace.SetSourceId(m_senderSubID);
		if(-1 != (Replace.OrderTag().Left(5)).Find("20040"))//to handle old ClOrdID beginning with "20040...."
		{
			Replace.SetSourceId(m_senderSubID);//Original "TDX" will be sent for these orders but does not need to be sent
			Replace.SetCancelTag(m_senderSubID + Replace.CancelTag());
		}
		else	
		{
			Replace.SetSourceId(Replace.OrderTag().Left(3));
			Replace.SetCancelTag(Replace.OrderTag().Left(3) + Replace.CancelTag());
		}

		Replace.SetSecurityType(COrder::Option);
		
		if(IsModify())
			//Replace.SetShares(m_pTags->OriginalShares() - m_pTags->LeavesShares() + Replace.Shares());
			Replace.SetShares(m_pTags->LastCumQty() + Replace.Shares()); 
		
		BOOL result = ((COptFixSession*)&(m_pApp->Exchange()))->ProcessReplace(Replace);
		
		Replace.SetSourceId(m_pTags->Client());
		return result;
	}

	inline BOOL OnProcessExecution(CExecution& Execution)
	{
		if(COrder::Stopped == Execution.Status() || COrder::ReplacePending == Execution.ExecType())
			return FALSE;
		
		if( (Execution.ExecType() == COrder::PartiallyFilled) && (Execution.Leaves() == m_pTags->LeavesShares()) && (Execution.Cumulative() == m_pTags->LastCumQty()) )
		{
			TOStrStream l_LogMsg;
			l_LogMsg << "Ignored a PARTIAL FILL execution report since it is a duplicate after it was busted by a move of account:"<<endl;
			l_LogMsg << "OrderID: " << Execution.OrderTag() << " OM-Tag: " << Execution.StaffordTag();
			m_pApp->ProcessLogEntry("Fix Session", l_LogMsg.str().c_str());
			return FALSE; //Ignore this message since this is a dupe following a busted message
		}

		m_pTags->SetLastCumQty(Execution.Cumulative());
		return TRUE;
	}

	inline BOOL OnProcessCancelReject(CCancelReject& Reject)
	{
		return TRUE;
	}
	inline BOOL OnProcessReject(CReject& Reject)
	{
		return TRUE;
	}

private:
	void InitializeMemoryMap();
	void InitializeOrderValidator();
	void ParseClrFirm2SenderSubIDMap();
	void ParseSenderSubID( const std::string & str, std::vector<std::string> & fields, char token, char end = '\n' );
	void ParseSenderSubIDMap( );
	std::string trim( const std::string & str );
private:
	SenderSubIDMap		m_SenderSubIDMap;
	CString				m_senderSubID;  
	std::map<int, std::string> m_clrFirmSenderIDMap;  
};


#endif