#include "stdafx.h"
#include <iostream>
#include "getopt-repl.h"
//#include "CPPTest/TestStandardOutputDisplay.h"
#include "FIXServer.h"

BOOL CtrlHandler(DWORD fdwCtrlType) 
{ 
    switch (fdwCtrlType) 
    { 
        // Handle the CTRL+C signal. 
 
        case CTRL_C_EVENT: 
 
            Beep(1000, 1000); 
            return TRUE; 
 
        // CTRL+CLOSE: confirm that the user wants to exit. 
 
        case CTRL_CLOSE_EVENT: 
 
            return FALSE; 
 
        // Pass other signals to the next handler. 
 
        case CTRL_BREAK_EVENT: 
 
        case CTRL_LOGOFF_EVENT: 
 
        case CTRL_SHUTDOWN_EVENT: 
 
        default: 
 
            return FALSE; 
    } 
} 
 
int main( int argc, char** argv )
{
    BOOL fSuccess; 
 
    fSuccess = SetConsoleCtrlHandler( 
        (PHANDLER_ROUTINE) CtrlHandler,  // handler function 
        TRUE);                           // add to list 
    if (! fSuccess) 
		std::cout << "Could not set control handler"; 
	switch(getopt(argc, argv, "+d"))
	{
		case 'd':
		{
			/*
			CPPTest::TestStandardOutputDisplay display;

			TestSuite suite(display);
			suite.run();

			return suite.getExceptions().size() > 0;
			*/
		}
		default:
		{
			try
			{
				FIXServer::m_omFile = "om.cfg";
				FIXServer::m_fixFile = "fix.cfg";
				FIXServer::getInst()->Start();
				char szName[1024];
				GetModuleFileName( NULL, szName, sizeof(szName) );
				SetConsoleTitle(szName);
				char c;
				while( ( c = getchar() ) != 'q' )
				{
					if( c == 'r' )
					{
						FIXServer::getInst()->LoadAccount();
						FIXServer::getInst()->LoadSymbols();
					}
				}
			}
			catch(...)
			{
				return -1;
			}
			return 0;
		}
	}
}
