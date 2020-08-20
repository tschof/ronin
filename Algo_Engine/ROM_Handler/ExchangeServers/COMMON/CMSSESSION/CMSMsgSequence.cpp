#include "CMSMsgSequence.h"

const char*	CCMSMsgSequence::m_pMsgStartStr = "$#$#$#";
const char*	CCMSMsgSequence::m_pMsgEndStr = "#$#$#$\n";
const int		CCMSMsgSequence::m_pMsgStartStrLen = 6;
const int		CCMSMsgSequence::m_pMsgEndStrLen = 7;
const int		CCMSMsgSequence::m_pMaxCMSMsgSize = 200;


//---- Accessors ------------------------------------------------------------------------

bool CCMSMsgSequence::LoadFile(bool a_bReset)
{ 
	if(a_bReset)
	{
		//delete file
		FILE* pFile = fopen(m_FileName.c_str(),"w");
		if(pFile == NULL)
			return false;
		fclose(pFile);
	}
	if((m_pFile = fopen(m_FileName.c_str(), "a+")) == NULL)
		return false;
	LoadSequenceFromFile(); 
	return true;
}
//---------------------------------------------------------------------------------------

//---- Services -------------------------------------------------------------------------
void CCMSMsgSequence::ResetNextMsgSeqNum(const TMsgSeqNum& a_Value, bool a_bWrite)
{
	if(a_bWrite)
		fprintf(m_pFile,"\n%s\n",CMS_SEQUENCE_RESETSTRING);
	m_NextMsgSeqNum = a_Value;
	m_PosVector.clear();
	m_PosVector.resize(a_Value,0);

/*	int l_ArrayEntry = DOMOD(a_Value,19); //doing a mod 8192, 2^(32-13) = 8192
	int l_Overflow = l_ArrayEntry + 2048 - 8191;
	if(l_Overflow >= 0)
	{
		memset(m_PosInFile,0,sizeof(int)*l_Overflow);
		memset(&(m_PosInFile[l_ArrayEntry]),0,sizeof(int)*(8191 - l_ArrayEntry));
	}
	else
		memset(&(m_PosInFile[l_ArrayEntry]),0,sizeof(int)*2048); */
}

CCMSMsgSequence::EAddMsgReturnCode CCMSMsgSequence::AddMsg(const CCMSMsg& a_CMSMsg)
{
	unsigned int l_MsgSeqNumb = a_CMSMsg.MsgSeqNum();
	
	fseek(m_pFile,0,SEEK_END);
	int l_Pos = ftell(m_pFile);

	if(l_MsgSeqNumb == 0 || a_CMSMsg.MsgType() == CMS_Status_Order_ACK)
		return ADD_MSG_NO_ERROR;
	else if(l_MsgSeqNumb == m_NextMsgSeqNum) //one means restart
	{
		if(!WriteMsgToFile(a_CMSMsg))
			return ADD_MSG_CANT_WRITE_TO_FILE;
		else
		{
			++m_NextMsgSeqNum; 
			UpdateInUseArray(l_MsgSeqNumb, l_Pos);
			return ADD_MSG_NO_ERROR;
		}
	} 
	else if(l_MsgSeqNumb == 1) //doing reset
	{
		ResetNextMsgSeqNum(1);
		return AddMsg(a_CMSMsg);
	}
	else if(l_MsgSeqNumb < m_NextMsgSeqNum)
		return ADD_MSG_SEQ_NUM_SMALLER_THAN_EXPECTED;
	else 
		return ADD_MSG_SEQ_NUM_GAP;
}



//---- Laborers -------------------------------------------------------------------------

void CCMSMsgSequence::LoadSequenceFromFile()
{
	int l_MsgSeqNum = 0;
	int l_Pos;
	char* l_pNewMsgPos;
	char l_MsgBuf[256];

	if(!fgets(l_MsgBuf,255,m_pFile))//empty file
	{
		fprintf(m_pFile, "Begin\n"); //doing this to make the first message does not start from position 0
	}
	else 
	{
		l_Pos = ftell(m_pFile);
		while(fgets(l_MsgBuf,255,m_pFile))
		{
			//begining of a new message.  Get sequence numb
			if( (l_pNewMsgPos = strstr(l_MsgBuf, m_pMsgStartStr)) != NULL )
			{
				sscanf(l_pNewMsgPos+m_pMsgStartStrLen, "%d",&l_MsgSeqNum);
				UpdateInUseArray(l_MsgSeqNum, l_Pos);
			}
			else if(strstr(l_MsgBuf, CMS_SEQUENCE_RESETSTRING) != 0) //reset sequence
				ResetNextMsgSeqNum(1, false);

			l_Pos = ftell(m_pFile);
		}
	}
	m_NextMsgSeqNum = l_MsgSeqNum+1;
}

bool CCMSMsgSequence::FindRange(const TMsgSeqNum& a_Begin, const TMsgSeqNum& a_End, TCMSMsgPtrList& a_ListOut) const
{
	if(a_Begin >= m_NextMsgSeqNum) //nothing to get back
		return true;

	int l_Pos;
	for(TMsgSeqNum i = 	a_Begin; i<= a_End; i++)
	{
		if( (l_Pos = SeqNumPos(i)) > 0 ) //we get a message
		{
			if(fseek(m_pFile,l_Pos,SEEK_SET))
				return false;
			std::auto_ptr<CCMSMsg> l_pMsg = std::auto_ptr<CCMSMsg>(new CCMSMsg);
			if(ReadMsgFromFile(l_pMsg.get()))
			{
				if(l_pMsg->MsgSeqNum() != i)
					return false;

				a_ListOut.push_back(l_pMsg);
			}
		}
	}
	return true;
}

bool CCMSMsgSequence::ReadMsgFromFile(CCMSMsg* a_pMsgOut) const
{
	TMsgSeqNum	l_SeqNum;
	char		l_MsgType;
	char*		l_pEnd;

	//skip start string
	if(fseek(m_pFile,m_pMsgStartStrLen,SEEK_CUR))
		return false;

	char l_MsgStr[256];
	fgets(l_MsgStr, 255, m_pFile);
	sscanf(l_MsgStr, "%d/%c", &l_SeqNum,&l_MsgType);
	l_pEnd = strchr(l_MsgStr, '\t'); //find the message starting point
	a_pMsgOut->MsgStrForChange().append(l_pEnd+1);

	a_pMsgOut->SetMsgSeqNum(l_SeqNum);
	a_pMsgOut->SetMsgType((ECMSMsgType)l_MsgType);
	while(fgets(l_MsgStr, 255, m_pFile))
	{
		if( (l_pEnd = strstr(l_MsgStr, m_pMsgEndStr)) != NULL)
		{
			*l_pEnd = 0;
			a_pMsgOut->MsgStrForChange().append(l_MsgStr);
			return true;
		}
		else
			a_pMsgOut->MsgStrForChange().append(l_MsgStr);
	}
	return true;
}

