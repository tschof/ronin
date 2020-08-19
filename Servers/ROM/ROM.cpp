// ROM.cpp : Defines the entry point for the console application.
//

#include "Thread.h"
#include "RomMapN.h"
#include "ClientProfileManagerNew.h"
#include "ClientsManager.h"
#include "DestsManager.h"
#include "ThreadRomOrdersNew.h"
//#include "ThreadOrderHandler.h"
#include "client_manager.h"
#include "TimerServer.h"
#include "mailer.h"
#include "HeavyDuty.h"
#include "BuffStore.h"
//#include "CsvStore.h"
//#include "GStore.h"
#include "version.h"
#include "romtimer.h"
#include "Oats.h"
#include "Store.h"
#include "PositionManager.h"

static pthread_t g_maintID;
static bool g_closing;

/*
static bool CheckEnd(unsigned short now, void* obj)
{
    bool rc = CConfig::getInstance().Closing(now);

    if (rc == true)
    {
        g_closing = true;
        pthread_kill(g_maintID, SIGTERM);
    }

    return rc;
}
*/
static void theEnd(std::set<void*>&)
{
    g_closing = true;
    pthread_kill(g_maintID, SIGTERM);
    
    
}

static void StopThreads()
{
    CTimerServer::getInstance().Stop();
    CDestsManager::getInstance().Stop();
    COats::getInstance().Stop();
    CHeavyDuty::getInstance().Stop();
}

static void InitObjects()
{
    int csvStoreStep = CConfig::getInstance().CsvStoreStep();
    int orderAllocStep = CConfig::getInstance().OrdersAllocStep();

    CBuffStore::getInstance();
    CStore<CCsv>::getInstance().init(csvStoreStep, csvStoreStep);
    CStore<CRomOrderNew>::getInstance().init(orderAllocStep, orderAllocStep);

    CClientProfileManagerNew::getInstance();
    CClientsManager::getInstance();
    CTimerServer::getInstance();
    CDestsManager::getInstance();
    CThreadRomOrdersNew::getInstance();
    CPositionManager::getInstance();
    COats::getInstance();
}

static void StartThreads()
{
    CHeavyDuty::getInstance().Start();
    CDestsManager::getInstance().Start();

    while (!(CHeavyDuty::getInstance().Running() == true &&
             CTimerServer::getInstance().Running() == true &&
             CDestsManager::getInstance().Running() == true &&
             COats::getInstance().Running() == true))
    {
        CUtility::Sleep(1);
    }
        
    CConfig::getInstance().SetReady(true);
}

/*void issdouble_test()
{
    std::string sd = "82473.1212";
    
    bool rc = CUtility::IsValidDartSDouble(sd);
    
    sd = "asdfadf";
    rc = CUtility::IsValidDartSDouble(sd);
    
    sd = "-E1234";
    rc = CUtility::IsValidDartSDouble(sd);
    
    sd = "3.E0120";
    rc = CUtility::IsValidDartSDouble(sd);
    
    sd = "-3333-23";
    rc = CUtility::IsValidDartSDouble(sd);
    
    sd = "34.523.52351212";
    rc = CUtility::IsValidDartSDouble(sd);
    
    sd = "-.52352351212";
    rc = CUtility::IsValidDartSDouble(sd);
    
    sd = "-0.52351212";
    rc = CUtility::IsValidDartSDouble(sd);
}*/

int main(int argc, char* argv[])
{
    //issdouble_test();
    
    std::string errStr;

    tRunKey* pKey = CUtility::GetRunKey(argv[0], errStr);
    if (NULL == pKey)
    {
        printf(errStr.c_str());
        return -1;
    }

    g_maintID = pthread_self();
    g_closing = false;

    //gendb_init();

    sigset_t g_signal_mask;
    sigfillset(&g_signal_mask);
    //SIG_BLOCK means the thread will not be interrupted to call the corresponding signal handler
    //and the blocked signal will still be put into the pending queue when it is received.
    //sigwait will retrieve the pending signal and then return!!!
    //take out SGFPE, SIGLL, SIGSEGV and SIGBUS since the behavior of blocking them is undefined
    sigdelset(&g_signal_mask, SIGFPE);
    sigdelset(&g_signal_mask, SIGILL);
    sigdelset(&g_signal_mask, SIGSEGV);
    sigdelset(&g_signal_mask, SIGBUS);
    pthread_sigmask(SIG_BLOCK, &g_signal_mask, NULL);

    CUtility::SetConfigFile(argv[0]);
    //if (CConfig::getInstance().Closing())
    //{
    //    printf("ROM started at invalid time\n");
    //    return 0;
    //}

    //gendb_init();
    start_dart_mail_service(CConfig::getInstance().MailServer().c_str(),
                            CConfig::getInstance().MailServer().size(),
                            CConfig::getInstance().MailBizGroup().c_str(),
                            CConfig::getInstance().MailBizGroup().size(),
                            CConfig::getInstance().MailTechGroup().c_str(),
                            CConfig::getInstance().MailTechGroup().size(),
                            CConfig::getInstance().EndHour(),
                            CConfig::getInstance().EndMin(),
                            7);

    CUtility::Sleep(5);

    if (CConfig::getInstance().DbUp() == false)
    {
        char buffer[128];
        sprintf(buffer,
                "%s: DB connection failed!\n",
                CConfig::getInstance().InstanceName().c_str());
        //send email warning
        std::string sbj = buffer;
        char buf[256];
        sprintf(buf, "Check configure or contact DBA\n");
        CUtility::SendTechEMail(sbj, buf, strlen(buf));
        CUtility::Sleep(10);
        return 0;
    }

    InitObjects();

    CTimerServer::getInstance().Start();
    COats::getInstance().Start();

    while (!(COats::getInstance().Running() &&
             COats::getInstance().CreateNewEffective() == true && //make sure that first connect attempt has been acknowledged
             CTimerServer::getInstance().Running()))
    {
        CUtility::Sleep(1);
    }

    if (CConfig::getInstance().QueryLiveOrderFromOats())
    {
        while (COats::getInstance().OrderQueryFinished() == false) //make sure live orders have been loaded
        {
            CUtility::Sleep(1);
        }
    }

    if (CConfig::getInstance().PosMgrEnabled())
    {
        if (CConfig::getInstance().IsFirstRun())
        {
            CPositionManager::getInstance().Start();
        }
    }
    
    //start MDS
    CClientsManager::getInstance().StartMDS();
    //wait until recap is finished
    CUtility::Sleep(15);
    //load live orders from DB
    if (CConfig::getInstance().QueryLiveOrderFromOats() == 0)
    {
        CThreadRomOrdersNew::getInstance().LoadOrdersFromDB();
    }

    CClientsManager::getInstance().StartService();
    StartThreads();

    CConfig::getInstance().UpdateRunDate();

    time_t secondsToClose = CConfig::getInstance().SecondsToClose();
    std::set<void*> dummy;
    CRomTimer endTimer(0, secondsToClose, &theEnd, dummy);

    sigemptyset(&g_signal_mask);
    sigaddset(&g_signal_mask, SIGINT);
    pthread_sigmask(SIG_UNBLOCK, &g_signal_mask, NULL);
    sigdelset(&g_signal_mask, SIGINT);
    sigaddset(&g_signal_mask, SIGTERM);
    sigaddset(&g_signal_mask, SIGUSR1);

    int sig = -1;
    while (!(sigwait(&g_signal_mask, &sig) == 0 &&
             sig == SIGTERM))
    {
        if (sig == SIGUSR1)
        {
            CConfig::getInstance().Reload();
            COats::getInstance().ReloadIpPort();
        }
    }

    if (g_closing)
    {
        tDuty duty;
        duty.theFunc = CDestsManager::CancelAllDayOrders;
        duty.pArg = NULL;
        duty.tmp = false;
        CHeavyDuty::getInstance().AddMessage(duty);
        CUtility::Sleep(CConfig::getInstance().adCancel() * 60);
    }

    StopThreads();
    std::string shutdownStr = "Shut Down:";
    shutdownStr.append(CUtility::GetLocalDayStr());
    shutdownStr.append("\n");
    printf(shutdownStr.c_str());

    CUtility::DestroyRunKey(pKey);

    return 0;
}
