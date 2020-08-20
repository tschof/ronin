#ifndef _ROMORDER_H__
#define _ROMORDER_H__



#pragma once

#include "Utility.h"
#include "RomCsv.h"
#include "RomValueDefs.h"
#include <set>
#include <list>


class DShares
{
public:
	DShares() { m_active = false; }
	void Set(const CRomCsv& csv) 
	{ 
		m_active = true;
		setLast(csv.GetAt(ROMFIELDS::CSV_LAST_SHARES));
		setLeave(csv.GetAt(ROMFIELDS::CSV_LEAVE_SHARES));
		setCum(csv.GetAt(ROMFIELDS::CSV_CUM_SHARES));
	}

	bool active() { return m_active; }
	void setActive(bool value) { m_active = value;}
	void setLeave(const std::string& value) { m_leave = value; }
	void setCum(const std::string& value) { m_cum = value;}
	void setLast(const std::string& value) { m_last = value;}

	std::string& Leave() { return m_leave; }
	std::string& Last() { return m_last; }
	std::string& Cum() { return m_cum; }

private:
	bool	m_active;
	std::string	m_leave;
	std::string	m_cum;
	std::string	m_last;
};

class CRomOrder
{
public:
    //typedef std::list<CRomOrder*> tChildren;
    //typedef tChildren::iterator tChildrenIT;

	//static std::set<short> s_DeadStatus;
	static std::set<short> s_ActionStatus;
	static bool IsOptionType(char secType);
	static bool IsFutureType(char secType);
	static bool IsCurrencyType(char secType);
	static bool IsEquityType(char secType);
	static bool IsDead(const CRomOrder* pOrder);
	static bool IsActionStatus(short status);
	static bool IsLimitType(short orderType);
	static std::string GetFullOptionSymbol(CRomOrder* pOrder);
	static void Init();
		
	//CRomOrder(CRomCsv& msg, std::string& owner);
	CRomOrder(CRomCsv& msg);
	//CRomOrder(const CRomOrder& order);
	CRomOrder();

    //CRomCsv* csv()const { return m_pCsv; }
	std::string Message();
	std::string MessageSingle();
    //std::string message() { return m_pCsv->Message(); }

	~CRomOrder(void) {}

    const char* NewClientTag()const         { return m_NewClientTag; }
    const char* ActClientTag()const         { return m_ActClientTag; }
    //const char* RomExecTag()const           { return m_RomExecTag; }
    const char* RomTag() const              { return m_RomTag; }
    const char* ClientTag() const           { return m_ClientTag; }
    const char* Owner() const               { return m_Owner; }
    const char* Trader() const              { return m_Trader; }
    const char* TradeFor()	const           { return m_TradeFor; }
	void NextRomExecTag(std::string& tag);
    const char* ExpYearMonth() const        { return m_ExpYearMonth; }
	const char* ExpTime() const             { return m_ExpTime; }
    const char* Symbol() const              { return m_Symbol; }
	 const char* Underlying() const         { return m_Underlying; }
    //const char* NewTag()const               { return m_NewTag; }
    //const char* ActTag()const               { return m_ActTag; }
    const char* Firm()const                 { return m_Firm; }
    const char* LocalAccount() const        { return m_LocalAccount; }
    const char* ClearingAccount() const		{ return m_ClearingAccount; }
    const char* CMTAAccount() const		    { return m_CMTAAccount; }
    const char* GiveUpAccount() const		{ return m_GiveUpAccount; }
    const char* MMAccount() const			{ return m_MMAccount; }
    const char* ClearingId() const			{ return m_ClearingId; }
    const char* ExDest() const				{ return m_ExDest; }
    const char* ParentRomTag() const		{ return m_ParentRomTag; }
	const char* ParentClientTag() const		{ return m_ParentClientTag; }
    const char* SecID() const               { return m_SecID; }
	const char* SecDef() const              { return m_SecDef; }
	const char* DBSymbol() const			{ return m_DBSymbol; }
	const char* ShortLender() const			{ return m_ShortLender; }
	const char* ClientEcho() const			{ return m_ClientEcho; }
	const char* TimeStamp() const			{ return m_TimeStamp; }
	const char* RomExchangeTag() const		{ return m_RomExchangeTag; }
	const char* ExchangeTag() const			{ return m_ExchangeTag; }
	const char* MaxFloor()const             { return m_MaxFloor; }
	const char* StopPrice() const           { return m_StopPrice; }
	const char* PegOffset() const            { return m_PegOffset; }
	const char* ReplacingStopPrice()const	{ return m_ReplacingStopPrice; }
	const char* ReplacingPegOffset()const	{ return m_ReplacingPegOffset; }
	const char* MaturityDay() const			{ return m_MaturityDay; }

	char ExecInst()const             {  return m_ExecInst; }
	char ReplacingExecInst()const	 {  return m_ReplacingExecInst; }
	char Capacity() const            {  return m_Capacity; }
	char Solicited() const			 {  return m_Solicited; }
	char SecType() const             {  return m_SecType; }
	char CallOrPut() const             {  return m_CallOrPut; }

	short Exchange() const           {  return m_Exchange; }
    short Dest() const               {  return m_Dest; }
	short Status() const             {  return m_Status; }
    short Side()const                {  return m_Side; }
	short TIF() const                {  return m_TIF; }
    short Type() const               {  return m_Type; }
	short MMExchange() const         {  return m_MMExchange; }
	short CplxType() const           {  return m_CplxType; }
	short ReplacingTIF()const		 {  return m_ReplacingTIF; }
	short ReplacingType()const		 {  return m_ReplacingType; }
	short AlgoType()const			 {  return m_AlgoType; }

	bool OpenClose() const           {  return m_OpenClose; }
	bool Used() const				 {  return m_Used; }
	bool ReplacePending() const      {  return m_ReplacePending; }
	
    double Price() const                    {  return m_Price; }
	
    //double StopPrice() const                {  return m_StopPrice; }
	double StrikePrice() const              {  return m_StrikePrice; }
    double AvgPrice() const                 {  return m_AvgPrice; }
    double CurrencyRate() const             {  return m_CurrencyRate; }
	double ReplacingPrice()const		    {  return m_ReplacingPrice; }
	//double ReplacingStopPrice()const	    {  return m_ReplacingStopPrice; }
	double PriceChange()const				{  return m_PriceChange; }
	int Multiplier() const                  {  return m_Multiplier; }
    long Shares()const                      {  return m_Shares; }
    long OriginalShares()const              {  return m_OriginalShares; }
    long Leave()const                       {  return m_Leave; } 
    //long MaxFloor()const                    {  return m_MaxFloor; }
    long Cum()const                         {  return m_Cum; }
	long ShareChange() const                {  return m_ShareChange; }
	//long ReplacingMaxFloor() const          {  return m_ReplacingMaxFloor; }
	unsigned OrderSeq() const				{  return m_OrderSeq; }
	unsigned CancelSeq() const				{  return m_CancelSeq; }
	unsigned ReplaceSeq() const				{  return m_ReplaceSeq; }

	void SetPrice(double value)                     { m_Price = value; }
	void SetAvgPrice(double value)                  { m_AvgPrice = value; }
	void SetReplacingPrice(double value)		    { m_ReplacingPrice = value; }
	//void SetReplacingStopPrice(double value)	    { m_ReplacingStopPrice = value; }
	void SetCurrencyRate(double value)				{ m_CurrencyRate = value; }
	//void SetStopPrice(double value)			        { m_StopPrice = value; }
	void SetStrikePrice(double value)			    { m_StrikePrice = value; }
	void SetPriceChange(double value)				{ m_PriceChange = value; }

	void SetLeave(long value)						{ m_Leave = value; }	
	void SetCum(long value)							{ m_Cum = value; }
    void SetShares(long value)				        { m_Shares = value; }
    void SetOriginalShares(long value)		        { m_OriginalShares = value; }
	void SetShareChange(long value)		            { m_ShareChange = value; }
	void SetMultiplier(long value)			        { m_Multiplier = value; }
	//void SetReplacingMaxFloor(long value)			{ m_ReplacingMaxFloor = value; }
	//void SetMaxFloor(long value)				    { m_MaxFloor = value; }
	void SetMinQty(long value)				        { m_MinQty = value; }
	void SetNextExecId(long value)					{ m_NextExecId = value; }
	void SetOrderSeq(unsigned long value)			{ m_OrderSeq = value; }
	void SetCancelSeq(unsigned long value)			{ m_CancelSeq = value; }
	void SetReplaceSeq(unsigned long value)			{ m_ReplaceSeq = value; }

	void SetStatus(short value)				        { m_Status = value; }
	void SetDest(short value)				        { m_Dest = value; }
	void SetExchange(short value)			        { m_Exchange = value; }
	void SetMMExchange(short value)			        { m_MMExchange = value; }
	void SetType(short value)				        { m_Type = value; }
	void SetSecType(short value)				    { m_SecType = value; }
	void SetSide(short value)				        { m_Side = value; }
	void SetCplxType(short value)			        { m_CplxType = value; }
	void SetReplacingType(short value)				{ m_ReplacingType = value; }
	void SetReplacingTIF(short value)				{ m_ReplacingTIF = value; }
	void SetTIF(short value)					    { m_TIF = value; }
	void SetAlgoType(short value)					{ m_AlgoType = value; }

	void SetCallOrPut(char value)			        { m_CallOrPut = value; }
	void SetCapacity(char value)			        { m_Capacity = value; }
	void SetSolicited(char value)					{ m_Solicited = value; }
	void SetExecInst(char value)					{ m_ExecInst = value; }
	void SetReplacingExecInst(char value)			{ m_ReplacingExecInst = value; }

	void SetReplacePending(bool value)				{ m_ReplacePending = value; }
	void SetOpenClose(bool value)			        { m_OpenClose = value; }
	void SetUsed(bool value)						{ m_Used = value; }

	void SetRomTag(const char* value)				{ CUtility::StrCpy(m_RomTag, sizeof(m_RomTag), value); }
	void SetClearingAccount(const char* value)		{ CUtility::StrCpy(m_ClearingAccount, sizeof(m_ClearingAccount), value); }
    void SetClearingId(const char* value)			{ CUtility::StrCpy(m_ClearingId, sizeof(m_ClearingId), value); }
    void SetSingleTimeStamp(const char* value)		{ CUtility::StrCpy(m_TimeStamp, sizeof(m_TimeStamp), value); }
    void SetCMTAAccount(const char* value)			{ CUtility::StrCpy(m_CMTAAccount, sizeof(m_CMTAAccount), value); }
	void SetExpYearMonth(const char* value)			{ CUtility::StrCpy(m_ExpYearMonth, sizeof(m_ExpYearMonth), value); }
    void SetExDest(const char* value)				{ CUtility::StrCpy(m_ExDest, sizeof(m_ExDest), value); }
	void SetFirm(const char* value)				    { CUtility::StrCpy(m_Firm, sizeof(m_Firm), value); }
    void SetGiveUpAccount(const char* value)		{ CUtility::StrCpy(m_GiveUpAccount, sizeof(m_GiveUpAccount), value); }
    void SetInstructions(const char* value)		    { CUtility::StrCpy(m_Instructions, sizeof(m_Instructions), value); }
    void SetLocalAccount(const char* value)		    { CUtility::StrCpy(m_LocalAccount, sizeof(m_LocalAccount), value); }
  	void SetMMAccount(const char* value)			{ CUtility::StrCpy(m_MMAccount, sizeof(m_MMAccount), value); }
    void SetExpTime(const char* value)				{ CUtility::StrCpy(m_ExpTime, sizeof(m_ExpTime), value); }
    void SetRomExchangeTag(const char* value)       { CUtility::StrCpy(m_RomExchangeTag, sizeof(m_RomExchangeTag), value);}
	void SetExchangeTag(const char* value)			{ CUtility::StrCpy(m_ExchangeTag, sizeof(m_ExchangeTag), value);}
	void SetOwner(const char* value)				{ CUtility::StrCpy(m_Owner, sizeof(m_Owner), value); }
	void SetSymbol(const char* value)				{ CUtility::StrCpy(m_Symbol, sizeof(m_Symbol), value); }
	void SetClientTag(const char* value)			{ CUtility::StrCpy(m_ClientTag, sizeof(m_ClientTag), value); }
    void SetTradeFor(const char* value)			    { CUtility::StrCpy(m_TradeFor, sizeof(m_TradeFor), value); }
	void SetTrader(const char* value)				{ CUtility::StrCpy(m_Trader, sizeof(m_Trader), value); }
	void SetUnderlying(const char* value)			{ CUtility::StrCpy(m_Underlying, sizeof(m_Underlying), value); }
    //void SetRomExecTag(const char* value)			{ CUtility::StrCpy(m_RomExecTag, sizeof(m_RomExecTag), value); }
    void SetParentRomTag(const char* value)		    { CUtility::StrCpy(m_ParentRomTag, sizeof(m_ParentRomTag), value); }
    void SetSecID(const char* value)		        { CUtility::StrCpy(m_SecID, sizeof(m_SecID), value); }
	void SetSecDef(const char* value)		        { CUtility::StrCpy(m_SecDef, sizeof(m_SecDef), value); }
    void SetNewClientTag(const char* value)			{ CUtility::StrCpy(m_NewClientTag, sizeof(m_NewClientTag), value); }
    void SetActClientTag(const char* value)			{ CUtility::StrCpy(m_ActClientTag, sizeof(m_ActClientTag), value); }
	void SetNextExecIdFromExecTag(const char* value);
	void SetCurrency(const char* value)			    { CUtility::StrCpy(m_Currency, sizeof(m_Currency), value); }
	void SetDBSymbol(const char* value)				{ CUtility::StrCpy(m_DBSymbol, sizeof(m_DBSymbol), value); }
	void SetParentClientTag(const char* value)		{ CUtility::StrCpy(m_ParentClientTag, sizeof(m_ParentClientTag), value); }
	void SetShortLender(const char* value)			{ CUtility::StrCpy(m_ShortLender, sizeof(m_ShortLender), value); }
	void SetClientEcho(const char* value)			{ CUtility::StrCpy(m_ClientEcho, sizeof(m_ClientEcho), value); }
	void SetTimeStamp(const char* value);
	void SetMaxFloor(const char* value)				{ CUtility::StrCpy(m_MaxFloor, sizeof(m_MaxFloor), value); }
	void SetReplacingMaxFloor(const char* value)	{ CUtility::StrCpy(m_ReplacingMaxFloor, sizeof(m_ReplacingMaxFloor), value); }
	void SetStopPrice(const char* value)			{ CUtility::StrCpy(m_StopPrice, sizeof(m_StopPrice), value); }
	void SetPegOffset(const char* value)			{ CUtility::StrCpy(m_PegOffset, sizeof(m_PegOffset), value); }
	void SetReplacingStopPrice(const char* value)	{ CUtility::StrCpy(m_ReplacingStopPrice, sizeof(m_ReplacingStopPrice), value); }
	void SetReplacingPegOffset(const char* value)	{ CUtility::StrCpy(m_ReplacingPegOffset, sizeof(m_ReplacingPegOffset), value); }
	void SetMaturityDay(const char* value)			{ CUtility::StrCpy(m_MaturityDay, sizeof(m_MaturityDay), value); }


    void UpdateReplacingFields(CRomCsv& msg, short clientType); // before sending replace
	void UpdateReplacedFields();						// upon replaced
	void Update(CRomCsv& hoststatus);  // update independent of status

    //For Cplx Order
   // void SetParent(CRomOrder* pOrder) { m_pParent = pOrder; }
   // CRomOrder* Parent() { return m_pParent; }
   // void AddChild(CRomOrder* pOrder);
    //tChildren& Children() { return *m_Children; }
	//tChildren* Children() { return m_Children; }
	//bool CRomOrder::HasChildren();
    void InheritParent(CRomOrder* pOrder);
    void IncrementLegCount() { m_LegCount++; }
    void DecrementLegCount() { m_LegCount--; }
    unsigned short LegCount() { return m_LegCount; }

   // CRomOrder* GetLegFromSecID(const std::string& secid);
	void GetCsv(CRomCsv& csv);
	//void DeleteChildren();
	void Set(CRomCsv& csv);
	void SingleSet(CRomCsv& csv);
	//void ResetChildParent() { m_Children = NULL; m_pParent = NULL; }
	int SideSign() const;

private:
	//CRomCsv* m_pCsv;
		
    double m_Price;
	double m_AvgPrice;
    double m_StrikePrice;
    //double m_StopPrice;
	double m_ReplacingPrice;
	
	//double m_ReplacingStopPrice;
	double m_CurrencyRate;
	double m_PriceChange;

	long m_NextExecId;
    long m_Shares;
    long m_OriginalShares;
    long m_Cum;
    long m_Leave;
    //long m_MaxFloor;
	long m_ShareChange;
	//long m_ReplacingMaxFloor;
	long m_Multiplier;
	long m_MinQty;
	unsigned m_OrderSeq;  //the sequence number of sending this order
	unsigned m_CancelSeq; //the sequence number of sending the last cancel
	unsigned m_ReplaceSeq; //the sequence number of sending the last replace

    short m_TIF;
    short m_Status;
    short m_Side;
    short m_Exchange;
    short m_Dest;
    short m_MMExchange;
    short m_Type;
    short m_ReplacingType;
	short m_LegCount;
	short m_CplxType;
	short m_ReplacingTIF;
	short m_AlgoType;

    char m_ExecInst;
    char m_SecType;
    char m_CallOrPut;
    char m_Capacity;
	char m_ReplacingExecInst;
	char m_Solicited;

    char m_RomTag[128];
    //char m_RomExecTag[128];
    char m_ClientTag[128];
    char m_RomExchangeTag[64];
	char m_ExchangeTag[64];
    char m_ParentClientTag[128];
    char m_ParentRomTag[128];
    char m_Owner[32];
    char m_Trader[32];
    char m_TradeFor[32];
    char m_Currency[4];
    char m_ExpYearMonth[11];
	char m_ExpTime[20];
	char m_ReplacingExpTime[20];
    char m_Symbol[36];
	char m_DBSymbol[36];
	char m_Underlying[36];
    char m_NewClientTag[128];
    char m_ActClientTag[128];;
    char m_Firm[21];
    char m_LocalAccount[32];
    char m_ClearingAccount[32];
    char m_CMTAAccount[32];
    char m_GiveUpAccount[32];
    char m_ClearingId[32];
    char m_ExDest[10];
    char m_TimeStamp[9];
    char m_Instructions[128];
	char m_ReplacingInstructions[128];
	char m_SecID[32];
	char m_SecDef[128];
	char m_MMAccount[32];
	char m_ShortLender[31];
	
	char m_ClientEcho[21];
	char m_MaxFloor[10];
	char m_ReplacingMaxFloor[10];
	char m_StopPrice[10];
	char m_PegOffset[10];
	char m_ReplacingStopPrice[10];
	char m_ReplacingPegOffset[10];
	char m_MaturityDay[3];
	
	bool m_OpenClose;
	bool m_ReplacePending;
	bool m_Used; //for memory map purpose
    //tChildren* m_Children;
    //CRomOrder* m_pParent;

    //void RemoveChild(CRomOrder* pOrder);
		
	static void LoadActionStatus();
	//static void LoadDeadStatus();
};

#endif //_ROMORDER_H__