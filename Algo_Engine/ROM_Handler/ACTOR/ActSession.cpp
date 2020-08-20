// ActSession.cpp: implementation of the CActSession class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MessageApp.h"
#include "Actor.h"
#include "Act.h"
#include "ActFields.h"
#include "ActSession.h"
//#include "TerraHandler.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#pragma warning(disable:4786)

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CActSession::~CActSession()
{
}

CActSession::CActSession()
{
	COleDateTime time = COleDateTime::GetCurrentTime();
	CTerraNovaApp *pApp = (CTerraNovaApp *)AfxGetApp();
	
	char *buffer = new char[10];
	
	_itoa( time.GetMonth(), buffer, 10 );
	m_Date += buffer;
	_itoa( time.GetDay(), buffer, 10 );
	m_Date += buffer;
	_itoa( time.GetYear(), buffer, 10 );
	m_Date += buffer;

	CString Date = pApp->GetProfileString("Session", "Date", m_Date);
	if(m_Date != Date)
	{
		CActTrailer::LastSent(0);
	}
	else
	{
		CActTrailer::LastSent(pApp->GetProfileInt("Session", "Seq", 0));
	}
	pApp->WriteProfileString("Session", "Date", m_Date );

	delete buffer;
}

void CActSession::ProcessTimer()
{
	COleDateTime time = COleDateTime::GetCurrentTime();
	CString Date;

	CTimeSpan TimeDiff;
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	

	CTime CurrentTime = CTime::GetCurrentTime();
	int StartTime, EndTime;

	StartTime = GetPrivateProfileInt("Runtime Information",	"Start Time",6, pApp->Path() + pApp->Name() + ".DAT");
	EndTime = GetPrivateProfileInt("Runtime Information",	"End Time", 6, pApp->Path() + pApp->Name() + ".DAT");

	if(StartTime == EndTime)
	{
		if((CurrentTime.GetHour() == StartTime))
		{
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}
	else if(StartTime > EndTime)
	{
		if ((CurrentTime.GetHour() >= EndTime) && (CurrentTime.GetHour() < StartTime))
		{
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}
	else
	{
		if ((CurrentTime.GetHour() < StartTime) || (CurrentTime.GetHour() >= EndTime))
		{
			AfxGetMainWnd()->PostMessage(WM_CLOSE);
			return;
		}
	}

	char *buffer = new char[10];
	
	_itoa( time.GetMonth(), buffer, 10 );
	Date += buffer;
	_itoa( time.GetDay(), buffer, 10 );
	Date += buffer;
	_itoa( time.GetYear(), buffer, 10 );
	Date += buffer;

	delete buffer;

	if(Date != m_Date)
	{
		CTerraNovaApp *pApp = (CTerraNovaApp *)AfxGetApp();
		pApp->WriteProfileString("Session", "Date", Date );
		pApp->OpenLogFile();
		m_Date = Date;
		CActTrailer::LastSent(0);
	}
}
/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// incoming event processing

CString CActSession::ProcessMessage(CString Message)
{
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
	CCsv csv;
	//if(CAct::ValidAct(Message))
	{
		CAct Act(Message);
		CString tag = Act.GetAt(ACT_REFERENCENUMBER);
		CString name = pApp->GetName(tag);
		Act.Reference(&csv);
		csv.SetAt(CSV_ACT_ORDER_TAG, pApp->GetTag(tag));
		csv.SetAt(CSV_ACT_COMMAND, CSV_ACT_COMMAND_STATUS);
		csv.SetAt(CSV_ACT_TIME, Act.GetAt(ACT_EXECUTIONTIME));
		csv.SetAt(CSV_ACT_REFERENCE_NUM, Act.GetControlNum());
		
		if(Act.GetAt(ACT_BSX) == "B")
			csv.SetAt(CSV_ACT_SIDE, "1");
		else
			csv.SetAt(CSV_ACT_SIDE, "2");

		switch(Act.GetType())
		{
		case ACT_T_TCEN:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TCEN_TYPE);
			pApp->ProcessTraffic("MSG RECV TCEN: ", Act.Debug());
			break;
		case ACT_T_TTEN:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TTEN_TYPE);
			pApp->ProcessTraffic("MSG RECV TTEN: ", Act.Debug());
			break;
		case ACT_T_CTEN:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TCEN_TYPE);
			pApp->ProcessTraffic("MSG RECV CTEN: ", Act.Debug());
			break;
		case ACT_T_TCAL:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TCAL_TYPE);
			pApp->ProcessTraffic("MSG RECV TCAL: ", Act.Debug());
			break;
		case ACT_T_TTAL:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TTAL_TYPE);
			pApp->ProcessTraffic("MSG RECV TTAL: ", Act.Debug());
			break;
		case ACT_T_CTAL:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_CTAL_TYPE);
			pApp->ProcessTraffic("MSG RECV CTAL: ", Act.Debug());
			break;
		case ACT_T_TCLK:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_TCLK_TYPE);
			pApp->ProcessTraffic("MSG RECV TCLK", "See Log");
			pApp->RemoveTag(tag);
			break;
		case ACT_T_REJ:
			csv.SetAt(CSV_ACT_TYPE, CSV_ACT_REJ_TYPE);
			pApp->ProcessError("MSG RECV REJ: ", Act.Debug());
			pApp->RemoveTag(tag);
			break;
		default:
			//ProcessEntry(&csv);
			break;
		};

		pApp->Clients().Send(name, csv.Message());
	}
	return(Message);
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
// order related message processing

long CActSession::ProcessEntry(CCsv *csv)
{
	long rValue = 0;
	CMessageApp	*pApp = (CMessageApp *)AfxGetApp();	
	
	CAct Act(csv);
	Act.SetAt(ACT_OEID, "INCA");
	if(rValue = (pApp->Host()->SendMessage(Act.Send())))
	{
		pApp->ProcessTraffic("ORDER SENT", Act.Debug());
	}
	else
	{
		Act.Decrement();
		CString tag = Act.GetAt(ACT_REFERENCENUMBER);
		CString name = pApp->GetName(tag);

		csv->SetAt(CSV_ACT_TYPE, CSV_ACT_REJ_TYPE);
		csv->SetAt(CSV_ACT_REJECT_REASON, "SEND FAILED");
		pApp->RemoveTag(tag);
		pApp->Clients().Send(name, csv->Message());
	}
	AfxGetApp()->WriteProfileInt( "Session", "Seq", CActTrailer::LastSent());
	return rValue;
}
