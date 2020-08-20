/*******************************************************************************
 *
 * Copyright (c) 2005 by RONIN CAPITAL, LLC
 *
 * All Rights Reserved. 
 *******************************************************************************/
/*
 *   
 *
 *
 * History
 *
 * date        user	       comment
 * -------     --------    -----------------------------------------------------
 * 11-11-2005   Parnell     Added the process InstructionField and SetBrutTags 
 *                          functions.
 *******************************************************************************/
#ifndef BRUTFIXSESSION
#define BRUTFIXSESSION

#include "FixSession.h"
#include "FixAppMsgs.h"

/* Parnell - 100 characters should be sufficient.*/
static const int BRUT_UD_VALUE_SIZE = 101;
/*Fix tags only include the values 0 - 9999 so 6 covers it for now.*/
static const int BRUT_UD_TAG_SIZE = 6;

class CBrutFixSession : public CFixSession
{
	// Constructors & destructors
	public:
		CBrutFixSession();
		~CBrutFixSession() {}

	// Services
	public:
		BOOL ProcessCancel(CCancel& a_Cancel);
		long ProcessEntry(COrder& a_Order);
        BOOL ProcessReplace(CReplace& Replace);
	protected:
		void Process(const CFixExecution& a_Exec);

	private:
        /* 
           This method is used when the Instruction field in the OM manager message
           is not blank.  This method assumes that the string in the instructions field
           has been created like so "fixTag"="Value"#"fixtag"="Value" ...  If the string
           has not been created like that this method will exit and no further parsing of
           the instruction string will occur.
        */
        void ProcessInstructionField(const CString& instructions, CFixOrderBase& order);
        void SetBrutTags(CFixOrderBase& l_Order, EFixOrdType& l_OrdType,
                                   CString& exDestination, CString& instructions);
        /* Single allocation of the tag field.*/
        TCHAR m_tag[BRUT_UD_TAG_SIZE];
        /* Single allocation of the value field.*/
        TCHAR m_value[BRUT_UD_VALUE_SIZE];
		BOOL m_bSuperMontage;     
};

#endif
