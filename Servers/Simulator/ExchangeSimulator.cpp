#include "FileStore.h"
#include "SocketAcceptor.h"
#include "SessionSettings.h"
#include "Executor.h"
#include <string>
#include <iostream>
#include <fstream>


static bool g_End = false;



void wait(unsigned int endtime)
{
    std::cout << "Type Ctrl-C to quit" << std::endl;
    while(false == g_End)
    {
        FIX::process_sleep(10);
        time_t t = time(NULL);
        tm * pDT = localtime( &t );
        unsigned int now = (pDT->tm_hour + 1) * 100 + pDT->tm_min;
        if (now >= endtime)
        {
            g_End = true;
        }
    }
}

int main(int argc, char* argv[])
{
    //CExceptionLogger::Init();

    std::string cfg = "Simulator.cfg";
    std::string log;
    
    try
    {
        freopen(log.c_str(), "a+", stdout);
        FIX::SessionSettings settings( cfg );
        CExecutor executor(&g_End);
        FIX::FileStoreFactory storeFactory( settings );
        FIX::ScreenLogFactory logFactory( settings );
        FIX::SocketAcceptor acceptor( executor, storeFactory, settings, logFactory );

        acceptor.start();
        wait(2350);
        acceptor.stop();
        return 0;
    }
    catch ( std::exception & e )
    {
        std::cout << e.what() << std::endl;
        return 1;
    }
}


