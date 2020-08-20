// ACE
#include <ace/Get_Opt.h>

// Application
#include "Application_Reactor_Thread_Pool.hpp"
#include "../ROM_Handler/Connection_Manager.hpp"
#include <ROM_Handler/Session_Control_Strategy.hpp>
#include <ROM_Handler/Session.hpp>
#include "Configurator/configuration.h"
#include <decision_engine/algo_manager.hpp>
#include <market_data/wombat_market_data_source.hpp>
#include <libdarthttp/http_server_singleton.hpp>
#include <Market_Data/Generic_DLL_Market_Data_Source.hpp>
#include "DART_Log.hpp"
#include "Decision_Engine/Volume_Profile_Data_Source.hpp"

// TODO: Excise as many platform specific dependencies
//       as possible, use ACE as the platform.

// Platform SDK
#include <windows.h> // GetPrivateProfileIntA

// CRT
#include <ctime> // time_t, time (), gmtime (), strftime ()
#include <io.h>  // _access

// Standard C++
#include <fstream> // std::ofstream
#include <string>  // std::string
#include <iostream>

void app_invalid_param_handler(const wchar_t *expression,
                               const wchar_t *function,
                               const wchar_t *file,
                               unsigned int line,
                               uintptr_t /*reserved*/)
{
  if (expression&&function&&file)
  {
    wchar_t widebuffer[128];
    memset(widebuffer,
      0,
      sizeof(widebuffer));
    swprintf(widebuffer,
      sizeof(widebuffer)/sizeof(wchar_t),
      L"%s:%d %s:%s",
      file,
      line,
      function,
      expression);
    char narrowbuffer[128];
    memset(narrowbuffer,
      0,
      sizeof(narrowbuffer));
    wcstombs(narrowbuffer,
      widebuffer,
      sizeof(narrowbuffer));
    ACE_DEBUG((LM_DEBUG,
      "Invalid parameter handler invocation: %s\n",
      narrowbuffer));
  }
  else
    ACE_DEBUG((LM_DEBUG,"Invalid parameter handler invocation\n"));
}

int init_volume_profile_data() {
  // Configure the volume profile data source for the VWAP algorithm
  const std::string vwap_profile_dir
    (CONFIGURATION::instance()->get("VWAP","VolumeProfileDirectory"));
  if (vwap_profile_dir.empty()) {
    DART_CRITICAL("No [VWAP]VolumeProfileDirectory INI section found\n");
    return -1;
  }

  if (VOLUME_PROFILE_DATA_SOURCE::instance()->init
        (vwap_profile_dir.c_str())==-1) {
    DART_CRITICAL
      ("Volume profile directory (%s) doesn't exist or is not accessible\n",
       vwap_profile_dir.c_str());
    return -1;
  }

  return 0;
}

int
ACE_TMAIN (int argc, ACE_TCHAR *argv[])
{
  HANDLE named_mutex=NULL;
  named_mutex=CreateMutex(NULL,TRUE,"Global\\Bullseye");
  if (named_mutex==NULL || GetLastError()==ERROR_ALREADY_EXISTS)
  {
    DART_CRITICAL ("Another instance of Bullseye.exe is running\n");
    return 1;
  }

  std::string log_file_name ("bullseye");
  // Set the ACE logger to use our own logger
  // Create a filename based on the date:
  const time_t now (time (0));
  const tm *ptm = gmtime (&now);
  if (ptm)
  {
    char time_str [64];
    memset (time_str, 0, sizeof (time_str));
    if (strftime (time_str, sizeof (time_str), "%Y%m%d", ptm) > 0)
    {
      log_file_name += '-';
      log_file_name += time_str;
      log_file_name += ".log";
    }
  }

  ACE_OSTREAM_TYPE *output = new std::ofstream
    (log_file_name.c_str (), std::ios::out | std::ios::app);
  ACE_LOG_MSG->msg_ostream (output, true);
  ACE_LOG_MSG->set_flags (ACE_Log_Msg::OSTREAM);


  DART_INFO ("Bullseye v0.0.0.2 starting up\n");

  // Log command line
  std::string commandline(argv[0]);
  for (int i=1; i<argc; ++i)
  {
    commandline += ' ';
    commandline += argv [i];
  }
  DART_INFO("Command line: %s\n", commandline.c_str());

  // Setup our invalid parameter handler.
  _set_invalid_parameter_handler(app_invalid_param_handler);

  ACE_Get_Opt opt (argc, argv, "f:t:");

  const char *configfile = 0;
  size_t thread_count=4;

  for (int c; (c = opt ()) != -1;)
  {
    if (c == 'f')
      configfile = opt.opt_arg ();
    else if (c == 't')
      thread_count=ACE_OS::atoi(opt.opt_arg());
  }

  if (thread_count==0)
    ACE_ERROR_RETURN((LM_ERROR, "Illegal thread count\n"), 1);

  if (!configfile) {
    ACE_DEBUG ((LM_DEBUG, "usage: Bullseye config-file\n"));
    return 1;
  }

  // Get port from configuration file
  if (_access (configfile, 0) == -1) {
    ACE_DEBUG ((LM_DEBUG, "Cannot access configfile %s\n", configfile));
    return 1;
  }

  CONFIGURATION::instance()->filename(configfile);
  if (init_volume_profile_data()!=0)
    return 1;

  UINT port = atoi(CONFIGURATION::instance()->get("Bullseye", "ServerPort").c_str());

  if (port == 0) {
    ACE_DEBUG ((LM_DEBUG,
                "Cannot find ServerPort key in the Bullseye section of %s\n",
                configfile));
    return 1;
  }

#ifdef HTTP_INTERFACE
  // Startup the HTTP interface
  try
  {
    HTTP_SERVER::instance()->init();
    HTTP_SERVER::instance()->start();
  }
  catch (const std::exception &e)
  {
    std::cerr
      << "HTTP server initialization/start failed: "
      << e.what() << std::endl;
  }
#endif // HTTP_INTERFACE

  const std::string configsection
    (CONFIGURATION::instance()->get("MarketData","Section"));
  const std::string dll
    (CONFIGURATION::instance()->get("MarketData","Dll"));
  if (!dll.empty()
      && dart::GENERIC_MD_SOURCE::instance()->open
        (dll.c_str(), configfile, configsection.c_str())==-1)
  {
    ACE_ERROR_RETURN((LM_ERROR,"Error opening market data source\n"),1);
  }

  /*
   * Setup the reactor model for our application.  The application architecture
   * is designed for a pool of threads servicing a single reactor.  The single
   * reactor is set as the default singleton reactor instance for convenience,
   * there is no need to pass around reactor references when setting up tasks.
   * The multiple threads give concurrency to the application.  The exact number
   * of threads used for the reactor can be configured at application startup
   * or dynamically determined based on the hardware the application is being
   * run on.
   */

  Application_Reactor_Thread_Pool thread_pool;

  Connection_Manager *conn_mgr = CONNECTION_MANAGER::instance ();

  if (conn_mgr->open () == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("mgr.open")), 1);


  // TODO: Eventually we might want to get this from the Session_Registrar, we
  // could allow the Session_Registrar to make decisions about how to find
  // files, etc.
  Session *market_dest_session = new Session(new Client_Session_Control_Strategy);

  Logon_Credentials credentials;
  credentials.username = CONFIGURATION::instance()->get("Bullseye", "ClientUsername");
  credentials.password = CONFIGURATION::instance()->get("Bullseye", "ClientPassword");
  market_dest_session->logon_credentials(credentials);

  const std::string dest
    (CONFIGURATION::instance()->get ("Bullseye","OutboundHost"));
  if (dest.empty())
  {
    DART_CRITICAL ("No outbound ROM host specified in configuration\n");
    return 1;
  }

  const std::string destport
    (CONFIGURATION::instance()->get("Bullseye","OutboundPort"));
  if (destport.empty())
  {
    DART_CRITICAL ("No outbound ROM port specified in configuration\n");
    return 1;
  }

  int destportnum (ACE_OS::atoi (destport.c_str()));
  if (destportnum==0 || destportnum > std::numeric_limits<u_short>::max())
  {
    DART_CRITICAL ("Invalid outbound ROM port '%s'\n",destport.c_str());
    return 1;
  }

  if (!conn_mgr->destination
        (ACE_INET_Addr(static_cast <u_short> (destportnum), dest.c_str()),
         market_dest_session))
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("Unable to add destination\n")), 1);

  // Make sure the ALGO_MANAGER Singleton is initialized
  dart::Algo_Manager *algoman = dart::ALGO_MANAGER::instance();
  if (algoman->open() != 0)
    ACE_ERROR_RETURN((LM_ERROR,
                      ACE_TEXT("(%t) %s: algo_manager open failed"),
                      ACE_TEXT(__FUNCTION__)), -1);


  algoman->set_market_data_source(dart::GENERIC_MD_SOURCE::instance());
  algoman->outbound_connection(market_dest_session);

  DART_DEBUG ("Spawning %d threads\n",thread_count);

  if (thread_pool.activate (THR_NEW_LWP | THR_JOINABLE, thread_count) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"), ACE_TEXT ("activate")), 1);

  ACE_Thread_Manager::instance ()->wait ();

  return 0;
}

void Session::logon_credentials(const Logon_Credentials &logon_credentials)
{
  this->logon_credentials_ = logon_credentials;
}
