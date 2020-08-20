#ifndef CMSMSGSEQUENCE
#define CMSMSGSEQUENCE

#pragma warning(disable:4786)


#include "CMSMsg.h"
#include <list>
#include <vector>

typedef std::auto_ptr<CCMSMsg> TMsgAutoPtr;
typedef std::list<TMsgAutoPtr> TCMSMsgPtrList;
typedef std::vector<int>    TCMSPosVector;
#define CMS_SEQUENCE_RESETSTRING	"Sequence Number Reset"

class CCMSMsgSequence
{
	// Enums
	public:
		enum EAddMsgReturnCode 
		{
			ADD_MSG_NO_ERROR,
			ADD_MSG_SEQ_NUM_GAP,
			ADD_MSG_NUM_ALREADY_IN_SEQUENCE,
			ADD_MSG_SEQ_NUM_SMALLER_THAN_EXPECTED,
			ADD_MSG_CANT_WRITE_TO_FILE
		};

	// Constructors & destructors
	public:
		CCMSMsgSequence()
			: m_NextMsgSeqNum(1) 
		{
			m_PosVector.reserve(10000);
		}
		CCMSMsgSequence(const char* a_FileName)
		{
			m_PosVector.reserve(10000);
			SetFile(a_FileName);
		}
		virtual ~CCMSMsgSequence() {;}

	// Accessors
	public:
		inline const TMsgSeqNum& NextMsgSeqNum() const
			{ return m_NextMsgSeqNum; }

		void ResetNextMsgSeqNum(const TMsgSeqNum& a_Value, bool a_bWrite = true);

		const char* FileName() const
			{ return m_FileName.c_str(); }

		inline void SetFile(const char* a_Str) 
			{ m_FileName = a_Str;}

		inline int SeqNumPos(int a_SeqNum) const
		{ return m_PosVector[a_SeqNum];}
	
	// Services
	public:
		EAddMsgReturnCode AddMsg(const CCMSMsg& a_Msg);
		bool LoadFile(bool a_bReset); 
		bool FindRange(const TMsgSeqNum& a_Begin, const TMsgSeqNum& a_End, TCMSMsgPtrList& a_ListOut) const;
		
	// Laborers
	private:
		void LoadSequenceFromFile();

		inline bool WriteMsgToFile(const CCMSMsg& a_Msg) 
		{
			fseek(m_pFile,0,SEEK_END);
			if(fprintf(m_pFile, "%s%d/%c\t%s%s", 
				m_pMsgStartStr, 
				a_Msg.MsgSeqNum(), 
				a_Msg.MsgType(),
				a_Msg.MsgStr().c_str(),
				m_pMsgEndStr
				) <0)
				return false;
			fflush(m_pFile);
			return true;
		}

		bool ReadMsgFromFile(CCMSMsg* a_Msg) const;

		inline void UpdateInUseArray(int a_SeqNum, int a_Pos)
		{
			while(a_SeqNum > m_PosVector.size())
			{
				m_PosVector.push_back(0);
			}
			m_PosVector.push_back(a_Pos);
		}
	// Members
	private:
		TMsgSeqNum	m_NextMsgSeqNum;
		TString		m_FileName;
		FILE*		m_pFile;
		TCMSPosVector	m_PosVector;

		const static char*	m_pMsgStartStr;
		const static char*	m_pMsgEndStr;

		const static int	m_pMsgStartStrLen;
		const static int	m_pMsgEndStrLen;
		const static int	m_pMaxCMSMsgSize;
};

#endif
