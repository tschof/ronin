// Act.h: interface for the CAct class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACT_H__8100EF32_4CD2_11D2_A83E_006008CD4894__INCLUDED_)
#define AFX_ACT_H__8100EF32_4CD2_11D2_A83E_006008CD4894__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "Csv.h"
#include "ActFields.h"
#include "time.h"
#include "math.h"

#define ACT_FUNCTION						0
#define ACT_FUNCTION_POS					1	// CSV_COMMAND
#define ACT_FUNCTION_LENGTH					1

#define ACT_ASOF							1
#define ACT_ASOF_POS						2	//blank
#define ACT_ASOF_LENGTH						1

#define ACT_SECURITYCLASS					2
#define ACT_SECURITYCLASS_POS				3	//"N"
#define ACT_SECURITYCLASS_LENGTH			1

#define ACT_BSX								3
#define ACT_BSX_POS							4	//CSV_TYPE
#define ACT_BSX_LENGTH						1

#define ACT_REFERENCENUMBER					4
#define ACT_REFERENCENUMBER_POS				5	//unique reference number
#define ACT_REFERENCENUMBER_LENGTH			6

#define ACT_VOLUME							5
#define ACT_VOLUME_POS						11  //CSV_SHARES
#define ACT_VOLUME_LENGTH					8

#define ACT_SYMBOL							6
#define ACT_SYMBOL_POS						19  //CSV_SYMBOL
#define ACT_SYMBOL_LENGTH					5

#define ACT_PRICEDOLLAR						7
#define ACT_PRICEDOLLAR_POS					24  //CSV_PRICE - dollar portion
#define ACT_PRICEDOLLAR_LENGTH				4

#define ACT_PRICENUMERATOR					8
#define ACT_PRICENUMERATOR_POS				28	//CSV_PRICE - fraction numerator
#define ACT_PRICENUMERATOR_LENGTH			3

#define ACT_PRICETRADEDIGIT					9
#define ACT_PRICETRADEDIGIT_POS				31	//CSV_PRICE - fraction denominator
#define ACT_PRICETRADEDIGIT_LENGTH			1

#define ACT_TRADEMODIFIER					10
#define ACT_TRADEMODIFIER_POS				32	//if late "SLD"
#define ACT_TRADEMODIFIER_LENGTH			3

#define ACT_PRICEOVERIDE					11
#define ACT_PRICEOVERIDE_POS				35	//"O"
#define ACT_PRICEOVERIDE_LENGTH				1

#define ACT_OEID							12
#define ACT_OEID_POS						36	//"STAF"
#define ACT_OEID_LENGTH						4

#define ACT_OEGU							13
#define ACT_OEGU_POS						40  //not used
#define ACT_OEGU_LENGTH						4

#define ACT_OECLEARNUMBER					14
#define ACT_OECLEARNUMBER_POS				44  //CSV_CLEARINGID
#define ACT_OECLEARNUMBER_LENGTH			4

#define ACT_MMID							15
#define ACT_MMID_POS						48	//"STAF"
#define ACT_MMID_LENGTH						4

#define ACT_MMGU							16
#define ACT_MMGU_POS						52  //not used
#define ACT_MMGU_LENGTH						4

#define ACT_MMCLEARNUMBER					17
#define ACT_MMCLEARNUMBER_POS				56  //CSV_CLEARINGID
#define ACT_MMCLEARNUMBER_LENGTH			4

#define ACT_MMPAINDICATOR					18
#define ACT_MMPAINDICATOR_POS				60	//CSV_CAPACITY
#define ACT_MMPAINDICATOR_LENGTH			1

#define ACT_TRADEREPORTFLAG					19
#define ACT_TRADEREPORTFLAG_POS				61	//CSV_REPORT
#define ACT_TRADEREPORTFLAG_LENGTH			1

#define ACT_CLEARINGFLAG					20
#define ACT_CLEARINGFLAG_POS				62	//???
#define ACT_CLEARINGFLAG_LENGTH				1

#define ACT_SPECIALTRADEINDICATOR			21
#define ACT_SPECIALTRADEINDICATOR_POS		63	//blank
#define ACT_SPECIALTRADEINDICATOR_LENGTH	1

#define ACT_EXECUTIONTIME					22
#define ACT_EXECUTIONTIME_POS				64	//CSV_TIME
#define ACT_EXECUTIONTIME_LENGTH			6

#define ACT_MEMO							23
#define ACT_MEMO_POS						70  //any memo note
#define ACT_MEMO_LENGTH						10

#define ACT_TRADEDECIMALPRICE				24
#define ACT_TRADEDECIMALPRICE_POS			80  //CSV_PRICE 
#define ACT_TRADEDECIMALPRICE_LENGTH		12

#define ACT_CONTRABRANCHSEQ					25
#define ACT_CONTRABRANCHSEQ_POS				92
#define ACT_CONTRABRANCHSEQ_LENGTH			8

#define ACT_TRADEDATE						26
#define ACT_TRADEDATE_POS					100
#define ACT_TRADEDATE_LENGTH				8

#define ACT_REVERSALIND						27
#define ACT_REVERSALIND_POS					108
#define ACT_REVERSALIND_LENGTH				1

#define ACT_FILLER							28
#define ACT_FILLER_POS						109
#define ACT_FILLER_LENGTH					115

#define ACT_NUMPARAMS 29

#define ACT_MSG_LENGTH 115
#define ACT_TRAILER_LENGTH 4

// definitions for array
#define ACT_POSITION 0
#define ACT_LENGTH   1

enum ActType
{
	ACT_T_NONE = 0,
	ACT_T_TCEN,
	ACT_T_TCAL,
	ACT_T_TCLK,
	ACT_T_REJ,
	ACT_T_TTEN,
	ACT_T_CTEN,
	ACT_T_TTAL,
	ACT_T_CTAL,
};

class CActHeader
{
public:
	CActHeader();
	const CString Header();
private:
	CString m_Header;
};

class CActTrailer
{
public:
	CActTrailer();
	const CString Trailer();
	int Increment();
	int Decrement();

	static int LastSent() { return m_seqNumber; }
	static void LastSent(int seq) { m_seqNumber = seq; }
private:
	static int m_seqNumber;
	CString m_strSeqNumber;
	CString m_Trailer;
};

class CAct  
{
public:
	CAct();
	CAct(CCsv *);
	CAct(CString&);
	virtual ~CAct();

	void Convert(CCsv *);
	void Convert(CString&);
	CCsv* Convert();
	void Reference(CCsv*);

	void SetAt(int Field, CString Value);
	const CString GetAt(int Field);
	static CString Validate(int Field, CString &Value);
	const CString Body();
	const CString Message();
	const CString Send();
	int Increment() {return m_Trailer.Increment();}
	int Decrement() {return m_Trailer.Decrement();}
	const CString Debug() { return GetAt(ACT_FUNCTION) + ' '
								 + GetAt(ACT_BSX) + ' '
								 + GetAt(ACT_SYMBOL) + ' ' 
								 + GetAt(ACT_VOLUME) 
								 + " Shares At "
                                 + GetAt(ACT_TRADEDECIMALPRICE); }
	void SetType(ActType Type) { m_Type = Type; }
	ActType GetType() { return m_Type; }
	CString GetControlNum() { return m_ControlNumber; }
	
	static ActType ValidAct(CString &ActMsg) { if(ActMsg.Find("TCEN") != -1)
													return ACT_T_TCEN;
												else if(ActMsg.Find("TCAL") != -1)
													return ACT_T_TCAL;
												else if(ActMsg.Find("TCLK") != -1)
													return ACT_T_TCLK;
												else if(ActMsg.Find("REJ - ") != -1)
													return ACT_T_REJ;
												else if(ActMsg.Find("TTEN") != -1)
													return ACT_T_TTEN;
												else if(ActMsg.Find("CTEN") != -1)
													return ACT_T_CTEN;
												else if(ActMsg.Find("TTAL") != -1)
													return ACT_T_TTAL;
												else if(ActMsg.Find("CTAL") != -1)
													return ACT_T_CTAL;
												else
													return ACT_T_NONE;
	}
	static int LastSent(){ return CActTrailer::LastSent(); }
private:
	CString m_Message;
	CFile m_Output;
	CCsv *m_Source;
	static int m_ActInfo[][2];
	BOOL m_ActValid[ACT_NUMPARAMS];

	CActHeader m_Header;
	CActTrailer m_Trailer;

	ActType m_Type;
	CString m_RejectReason;
	CString m_ControlNumber;
};

#endif // !defined(AFX_ACT_H__8100EF32_4CD2_11D2_A83E_006008CD4894__INCLUDED_)
