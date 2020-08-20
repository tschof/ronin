// IActequenceSet.h: interface for the CIActequenceSet class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ACTSEQUENCESET_H__F92410DD_55CA_11D5_A61F_00500401F87A__INCLUDED_)
#define AFX_ACTSEQUENCESET_H__F92410DD_55CA_11D5_A61F_00500401F87A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

typedef struct SequneceSetStruct
{
	long	m_Sequence;
	char	m_Date[20];
} SequneceSetStruct;

class CActSequenceSet  
{
public:
	CActSequenceSet();
	virtual ~CActSequenceSet();

	void InitMep() 
	{ 
		if (m_SequenceSet == NULL)
		{
			AllocateMemoryMap();
		}
	}

	int	GetSequence();				// increments seq num and returns seq num - 1
	void	SetSequence(int Sequence);			// set the next sequence number to recover from errors
	//long	GetExpected();			// increments the incoming sequence number and returns the expected
	//void	SetExpected(long ExpectedSequence);		// sets number. used after receiving a sequence reset
	void	SetDate(const char * pDate);  // Set today's date

	SequneceSetStruct *m_SequenceSet;

private:
	HANDLE m_hFile;
	HANDLE m_hFileMap;
	LPVOID m_pFileView;

	void AllocateMemoryMap();
};


#endif // !defined(AFX_ACTSEQUENCESET_H__F92410DD_55CA_11D5_A61F_00500401F87A__INCLUDED_)
