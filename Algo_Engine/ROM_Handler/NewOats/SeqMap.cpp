#include "SeqMap.h"
//#include "Config.h"
#include "ZLog.h"


unsigned long SeqMap::NextSequence()
{
	m_pSeqStruct->sequence++;
	return m_pSeqStruct->sequence;
}

//SeqMap::SeqMap(void)
SeqMap::SeqMap(const std::string& file, std::string memName)
{
	//std::string file = (CConfig::getInstance()).SeqMapFile();

	//std::string memName = file + std::string("_MEM");
	m_hFileHandle = CreateFile(	file.c_str(),
								GENERIC_READ | GENERIC_WRITE,
								FILE_SHARE_READ | FILE_SHARE_WRITE,
								NULL,
								CREATE_NEW,
								FILE_ATTRIBUTE_NORMAL,	
								NULL
							   );

	bool NewFile = m_hFileHandle == INVALID_HANDLE_VALUE ? false : true;
	if(NewFile != true)
	{
		m_hFileHandle = CreateFile(	file.c_str(),
									GENERIC_READ | GENERIC_WRITE,
									FILE_SHARE_READ | FILE_SHARE_WRITE,
									NULL,
									OPEN_ALWAYS,
									FILE_ATTRIBUTE_NORMAL,
									NULL
								  );
	}

	m_hFileMap = CreateFileMapping(	(HANDLE)m_hFileHandle,
									NULL,
									PAGE_READWRITE,
									0,
									sizeof(SeqStruct),
									memName.c_str());

	if (m_hFileMap)
	{
		if (m_pFileView = MapViewOfFile(m_hFileMap, FILE_MAP_WRITE, 0, 0, 0))
		{
			m_pSeqStruct = (SeqStruct *)m_pFileView;
			if (NewFile ||
				strcmp(m_pSeqStruct->date, SeqMap::DateString().c_str()) != 0)
			{
				strcpy(m_pSeqStruct->date, ZLog::DateString().c_str());
				m_pSeqStruct->sequence = 0;
			}
		}
	}
}


void SeqMap::SetSequence(unsigned long value)
{
	m_pSeqStruct->sequence = value;
}

unsigned long SeqMap::Sequence()
{
	return m_pSeqStruct->sequence;
}


SeqMap::~SeqMap(void)
{
	if(m_pFileView)
	{
		UnmapViewOfFile(m_pFileView);
	}

	if(m_hFileMap)
	{
		CloseHandle(m_hFileMap);
	}

	if(m_hFileHandle)
	{
		CloseHandle(m_hFileHandle);
	}
}


std::string SeqMap::DateString()
{
	string r_value;

	char	CurrentDate[128];
	SYSTEMTIME systime;
	GetSystemTime(&systime);
	sprintf(CurrentDate, "%04u-%02u-%02u", systime.wYear, systime.wMonth, systime.wDay);
	r_value = CurrentDate;

	return r_value;
}