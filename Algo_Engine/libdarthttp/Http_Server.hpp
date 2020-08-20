#ifndef __HTTP_SERVER_HPP__
#define __HTTP_SERVER_HPP__

#include <map>
#include <string>
#include <http.h>
#include <windows.h>
#include <cassert>
#include "win32_error.hpp"

class Http_Server;
class Url_Handler
{
public:
  virtual bool on_url_request(const std::wstring &url,
                              Http_Server *http_server,
                              void *token) = 0;
};

class http_error : public std::runtime_error
{
public:
  explicit http_error(const std::string &errmsg)
    : std::runtime_error(errmsg)
  {
  }
};

class file_url_handler : public Url_Handler
{
public:
  file_url_handler (const std::string &file);
  bool on_url_request(const std::wstring &url,
                      Http_Server *http_server,
                      void *token);
  virtual ~file_url_handler();
private:
  std::string file_;
  HANDLE hfile_;
};

class Http_Server
{
public:
  Http_Server();
  void init();
  bool add_url(const wchar_t *url, Url_Handler *handler);
  void start();
  bool send(void* token,
            IN USHORT StatusCode,
            IN PSTR pReason,
            IN PSTR pEntityString);
  bool sendfile(void *token, HANDLE file_handle);
  void stop();
  virtual ~Http_Server();
private:
  static unsigned int __stdcall thread_launch_function(void *arg);
  void process_request(PHTTP_REQUEST request);
  void svc();
  HANDLE request_queue_handle_;
  HANDLE thread_handle_;
  HANDLE shutdown_event_;
  typedef std::map <std::wstring,Url_Handler*> Handlers;
  Handlers handlers_;
};

#endif
