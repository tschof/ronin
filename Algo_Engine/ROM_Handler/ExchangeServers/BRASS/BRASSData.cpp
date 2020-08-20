// BRASSData.cpp: implementation of the CBRASSData class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "BRASS.h"
#include "BRASSData.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CBRASSData::CBRASSData()
{

}

CBRASSData::~CBRASSData()
{

}

CIData* CBRASSData::Insert(int place)
{
	CBRASSData *NewBRASS = new CBRASSData();
	return(CIData::Insert((CIData*)NewBRASS, place));
}

CIData* CBRASSData::Remove()
{
	return(CIData::Remove(GetKey()));
}

void CBRASSData::SetDefault()
{
}