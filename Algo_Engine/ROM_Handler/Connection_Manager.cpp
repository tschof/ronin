#include <ace/Reactor.h>
#include "Connection_Manager.hpp"
#include "Session.hpp"
#include "Null_Session.hpp"
#include <cassert>
#include "Configurator/configuration.h"
#include "Bullseye/DART_Log.hpp"

#ifdef HTTP_INTERFACE
#include <libdarthttp/http_server_singleton.hpp>
#endif

Connection_Manager::Connection_Manager ()
  : shutting_down_ (false)
  , acceptor_(this,ACE_INET_Addr(9001))
  , destination_(0)
{
}

Connection_Manager::~Connection_Manager ()
{
}

int Connection_Manager::open (void*)
{
  bullseye::Guard guard(this->mutex_);

  if (this->reactor () == 0)
    this->reactor (ACE_Reactor::instance ());

  u_short accept_port(static_cast<u_short>
    (atoi(CONFIGURATION::instance()->get("Bullseye","ServerPort").c_str())));

  ACE_INET_Addr accept_addr (accept_port);
  if (this->acceptor_.open (accept_addr) == -1)
    ACE_ERROR_RETURN ((LM_ERROR, ACE_TEXT ("%p\n"),
                       ACE_TEXT ("acceptor open")),
                      -1);

#ifdef HTTP_INTERFACE
//  HTTP_SERVER::instance()->add_url(L"/connect", this);
  Http_Server *http = HTTP_SERVER::instance();
  try
  {
    if (http)
    {
      http->add_url(L"/connect", new file_url_handler
        ("C:\\dev\\OrderExecution\\SERVERS\\Algo_Engine\\Bullseye\\etc\\zengarden-sample.html"));
      http->add_url(L"/zengarden-sample.css", new file_url_handler
        ("C:\\dev\\OrderExecution\\SERVERS\\Algo_Engine\\Bullseye\\etc\\zengarden-sample.css"));
      http->add_url(L"/dart.jpg", new file_url_handler
        ("C:\\dev\\OrderExecution\\SERVERS\\Algo_Engine\\Bullseye\\etc\\dart.jpg"));
      http->add_url(L"/connection_manager", this);
    }
  }
  catch (const std::exception &e)
  {
    ACE_DEBUG((LM_DEBUG, "Cannot add file url handler: %s\n", e.what()));
  }
#endif

  return this->reactor ()->register_handler (SIGINT, this);
}

void Connection_Manager::reg (dart::Connection *)
{
//  managed_connections_.insert (pmanaged);
}

int Connection_Manager::handle_timeout(const ACE_Time_Value&, const void *connector)
{
  bullseye::Guard guard(this->mutex_);
  if (!shutting_down_ &&
      !static_cast<Connector*>(const_cast<void*>(connector))->asynch_connect())
    ACE_ERROR ((LM_ERROR, ACE_TEXT ("Could not reconnect\n")));
  return 0;
}

void Connection_Manager::unreg (dart::Connection *connection)
{
  bullseye::Guard guard(this->mutex_);

  // Check accepted connections first
  Connections::iterator connection_iterator (this->accepted_.find (connection));
  if (connection_iterator != this->accepted_.end ())
    this->accepted_.erase (connection_iterator);
  else
  {
    // Must be an outbound connection
    for (Connection_Info_Collection::iterator map_iter (this->connectors_.begin ());
         map_iter != this->connectors_.end (); ++map_iter)
    {
      if (map_iter->second.second == connection)
      {
        map_iter->second.second = 0;
        if (this->reactor()->schedule_timer
              (this, (void*) map_iter->second.first, ACE_Time_Value(30)) == -1)
          ACE_ERROR((LM_ERROR, ACE_TEXT("%p\n"),
                    ACE_TEXT("setting reconnect timer")));
      }
    }
  }

  if (shutting_down_ && accepted_.empty ())
    this->reactor ()->end_event_loop ();
}

int Connection_Manager::handle_signal (int, siginfo_t*, ucontext_t*)
{
  bullseye::Guard guard(this->mutex_);

  for (Connections::const_iterator iter = this->accepted_.begin ();
       iter != this->accepted_.end (); ++iter)
  {
    ACE_Message_Block *mb = 0;
    ACE_NEW_RETURN (mb, ACE_Message_Block (1, ACE_Message_Block::MB_STOP), -1);
    (*iter)->put (mb);
  }

  for (Connections::const_iterator iter = this->connected_.begin ();
       iter != this->connected_.end (); ++iter)
  {
    ACE_Message_Block *mb = 0;
    ACE_NEW_RETURN (mb, ACE_Message_Block (1, ACE_Message_Block::MB_STOP), -1);
    (*iter)->put (mb);
  }

  this->shutting_down_ = true;

  if (this->accepted_.empty () && this->connected_.empty ())
    this->reactor ()->end_event_loop ();

  return 0;
}

int Connection_Manager::handle_exit (ACE_Process *)
{
  bullseye::Guard guard(this->mutex_);

  this->reactor ()->run_reactor_event_loop ();
  return 0;
}

bool Connection_Manager::connected (const ACE_INET_Addr &addr,
                                    dart::Connection *pconn)
{
  bullseye::Guard guard(this->mutex_);

  this->connectors_ [addr].second = pconn;
  return true;
}

bool Connection_Manager::accepted (dart::Connection *pconn)
{
  bullseye::Guard guard(this->mutex_);

  if (this->accepted_.find (pconn) == this->accepted_.end ())
  {
    this->accepted_.insert (pconn);
    return true;
  }
  return false;
}

Session *Connection_Manager::destination() const
{
  bullseye::Guard guard(this->mutex_);

  return this->destination_;
}

bool
Connection_Manager::destination(const ACE_INET_Addr &addr, Session *session)
{
  bullseye::Guard guard(this->mutex_);

  bool result (false);

  if (this->destination_ == 0)
  {
    char addr_format[64];
    if (addr.addr_to_string(addr_format,sizeof(addr_format))==-1)
      memcpy(addr_format,"Error creating address string",sizeof(addr_format));

    DART_DEBUG ("%s: Adding destination %s\n",__FUNCTION__,addr_format);

    // Only add a connector if the address is new
    if (this->connectors_.find (addr) == this->connectors_.end ())
    {
      Connector *connector = new Connector (this, addr, session);
      dart::Connection *connection = 0;
      if (this->connectors_.insert
            (std::make_pair (addr,
             std::make_pair (connector,
             connection))).second == true)
             result = connector->asynch_connect ();
    }
  }

  return result;
}

Connector::Connector (Connection_Manager *conn_mgr,
                      const ACE_INET_Addr &addr,
                      Session *session)
  : conn_mgr_ (conn_mgr)
  , addr_ (addr)
  , session_(session)
{
}

int Connector::make_svc_handler (dart::Connection *&svc_handler)
{
  int result (-1);
  if (super::make_svc_handler (svc_handler) == 0)
  {
    this->conn_mgr_->connected (this->addr_, svc_handler);
    svc_handler->session(this->session_);
    assert(svc_handler);
    this->session_->io_handler(svc_handler);
    result = 0;
  }
  return result;
}

bool Connector::asynch_connect ()
{
  bool result (false);

  // Signal to connector that a new Connection is to be created.
  dart::Connection *p = 0;

  ACE_Synch_Options options;
  options.set (ACE_Synch_Options::USE_REACTOR, ACE_Time_Value (ACE_DEFAULT_TIMEOUT));

  // Set the connector in motion
  if (this->connect (p, this->addr_, options) == 0 ||
      ACE_OS::last_error () == EWOULDBLOCK)
    result = true;

  return result;
}

Acceptor::Acceptor(Connection_Manager *conn_mgr,
                   const ACE_INET_Addr &addr)
  : conn_mgr_(conn_mgr)
  , addr_(addr)
{
}

int Acceptor::make_svc_handler (dart::Connection *&svc_handler)
{
  int result (-1);
  if (super::make_svc_handler (svc_handler) == 0)
  {
    assert(svc_handler);
    this->conn_mgr_->accepted(svc_handler);
    Session *session = new Null_Session;
    session->io_handler(svc_handler);
    svc_handler->session(session);
    result = 0;
  }
  return result;
}

#ifdef HTTP_INTERFACE
#include <sstream>
bool Connection_Manager::on_url_request(const std::wstring&,
                                        Http_Server *http_server,
                                        void *token)
{
  std::stringstream page;
  page << "<html>"
            "<head>"
              "<title>Connection Manager</title>"
              "<link href=\"zengarden-sample.css\" rel=\"stylesheet\" type=\"text/css\">"
            "</head>"
            "<body id=\"css-zen-garden\">\r\n";

  page << "<div id=\"container\">";
  page << "<div id=\"preamble\">";
  page << "<p>" << this->accepted_.size() << " Connections Accepted</p>";
  if (this->accepted_.size())
  {
    // Dump them to a table
    page << "<table><tr><td>State</td></tr>";
    for (Connections::const_iterator conn_iter(this->accepted_.begin());
         conn_iter != this->accepted_.end(); ++conn_iter)
    {
      Session *psession = (*conn_iter)->session();
      page << "<tr><td>" << psession->current_state() << "</td></tr>";
    }
    page << "</table>";
  }
  page << "</div>";
  page << "<div id=\"preamble\">" << this->connected_.size() << " Outbound Connections</div>";
  page << "</div>";

  page << "</body>"
          "</html>\r\n";

  http_server->send(token, 200, "OK", const_cast<char*>(page.str().c_str()));
  return true;
}
#endif
