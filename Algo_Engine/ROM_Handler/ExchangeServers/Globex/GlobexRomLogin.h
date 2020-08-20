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
#ifndef __GLOBEX_ROM_LOGIN_H__
#define __GLOBEX_ROM_LOGIN_H__

#include <string>
#include "ROMMessage.h"
#include "RomValidators.h"

class CGlobexRomLogin {
public:
    CGlobexRomLogin();
    ~CGlobexRomLogin() {}
    void Validate(RFIX::TFields& fields);
    inline bool ShouldSend() {
        return true;
    }
private:
    std::string m_message;
    std::string m_appName;
};



#endif //__GLOBEX_ROM_LOGIN_H__