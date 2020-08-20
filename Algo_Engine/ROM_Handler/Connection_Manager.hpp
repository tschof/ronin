#ifndef __CONNECTION_MANAGER_HPP__
#define __CONNECTION_MANAGER_HPP__

// ACE
#include <ace/Acceptor.h>
#include <ace/Connector.h>
#include <ace/Singleton.h>
#include <ace/SOCK_Acceptor.h>
#include <ace/SOCK_Connector.h>
#include <ace/Task.h>

// Std C++ Lib
#include <map>
#include <set>

// Application
#include "Connection.hpp"
#include "Bullseye/Bullseye_Common.hpp"

#ifdef HTTP_INTERFACE
// HTTP Server
#include <libdarthttp/Http_Server.hpp>
#endif

class Connector
  : public ACE_Connector <dart::Connection, ACE_SOCK_CONNECTOR>
{
public:
  typedef ACE_Connector <dart::Connection, ACE_SOCK_CONNECTOR> super;

  Connector (Connection_Manager*, const ACE_INET_Addr&, Session*);
  virtual int make_svc_handler (dart::Connection *&svc_handler);

  bool asynch_connect ();

private:
  Connection_Manager *conn_mgr_;
  ACE_INET_Addr addr_;
  Session *session_;
};

class Acceptor
  : public ACE_Acceptor <dart::Connection, ACE_SOCK_ACCEPTOR>
{
public:
  typedef ACE_Acceptor <dart::Connection, ACE_SOCK_ACCEPTOR> super;
  Acceptor (Connection_Manager*, const ACE_INET_Addr&);
  virtual int make_svc_handler (dart::Connection *&svc_handler);

private:
  Connection_Manager *conn_mgr_;
  ACE_INET_Addr addr_;
};

class Session;

class Connection_Manager
  : public ACE_Task <ACE_MT_SYNCH>
#ifdef HTTP_INTERFACE
  , public Url_Handler
#endif
{
public:
  Connection_Manager ();
  virtual ~Connection_Manager ();
  void reg (dart::Connection*);
  void unreg (dart::Connection*);

  bool connected (const ACE_INET_Addr&, dart::Connection*);
  bool  accepted (dart::Connection*);

  virtual int open (void * = 0);
  virtual int handle_signal (int, siginfo_t * = 0, ucontext_t * = 0);
  virtual int handle_timeout(const ACE_Time_Value&, const void* = 0);
  virtual int handle_exit (ACE_Process *);

  bool destination(const ACE_INET_Addr&, Session*);
  Session *destination() const;

#ifdef HTTP_INTERFACE
  // Url_Handler Interface
  virtual bool on_url_request(const std::wstring &url,
                              Http_Server *http_server,
                              void *token);
#endif

private:
  typedef std::set <dart::Connection*> Connections;
  Connections accepted_;
  Connections connected_;

  Acceptor acceptor_;

  typedef std::pair <Connector*, dart::Connection*> Connection_Info;
  typedef std::map <ACE_INET_Addr, Connection_Info> Connection_Info_Collection;

  // This map serves two purposes:  (1)  the keys form a collection of all
  // destination connections required and (2) the value of each key should
  // is a connector object that, if present, indicates whether a connection
  // attempt is currently in progress.
  Connection_Info_Collection connectors_;

  bool shutting_down_;
  Session *destination_;
  mutable bullseye::Mutex mutex_;
};

typedef ACE_Singleton <Connection_Manager, ACE_SYNCH_MUTEX> CONNECTION_MANAGER;

#endif
