#ifndef _SEQMAP_H__
#define _SEQMAP_H__

#include "Windows.h"
#include <string>

typedef struct
{
	char date[9];
	unsigned long sequence;
} SeqStruct;

class SeqMap
{
public:
	static std::string SeqMap::DateString();
	SeqMap(const std::string& file, std::string memName);
	~SeqMap(void);
	void SetSequence(unsigned long value);
	unsigned long Sequence();
	unsigned long NextSequence();

private:
	SeqStruct* m_pSeqStruct;
	
	HANDLE	m_hFileHandle;
	HANDLE	m_hFileMap;
	LPVOID	m_pFileView;
};

#endif //_SEQMAP_H__