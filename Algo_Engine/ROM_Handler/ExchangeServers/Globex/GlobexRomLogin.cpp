/*******************************************************************************
*
* Copyright (c) 2005 by RONIN CAPITAL, LLC
*
* All Rights Reserved. 
*******************************************************************************/
/*
*  : Defines the class behaviors for the application.
*
*
* History
*
* date        user	       comment
* -------     --------    -----------------------------------------------------
*     Parnell     
*
*******************************************************************************/


#include "GlobexRomLogin.h"

CGlobexRomLogin::CGlobexRomLogin()
: m_message("L"), m_appName("Globex")
{
}

void CGlobexRomLogin::Validate(RFIX::TFields& fields)
{
    fields[0] = m_message;
    fields[2] = m_appName;
}