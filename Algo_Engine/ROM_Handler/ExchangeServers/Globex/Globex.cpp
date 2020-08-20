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
#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers


#include <process.h>
#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")


#include "GlobexServer.h"

int main(int argc, char* argv[])
{
    std::string datFile = "Globex.dat";
    CGlobexServer server(datFile);
    server.Run();
    return 0;
}
