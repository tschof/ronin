#include "Http_Server.hpp"
#include <process.h>

// Macros.
#define INITIALIZE_HTTP_RESPONSE( resp, status, reason )  \
  do                                                      \
    {                                                     \
      RtlZeroMemory( (resp), sizeof(*(resp)) );           \
      (resp)->StatusCode = (status);                      \
      (resp)->pReason = (reason);                         \
      (resp)->ReasonLength = (USHORT) strlen(reason);     \
    } while (FALSE)


#define ADD_KNOWN_HEADER(Response, HeaderId, RawValue)                    \
  do                                                                      \
    {                                                                     \
      (Response).Headers.KnownHeaders[(HeaderId)].pRawValue = (RawValue); \
      (Response).Headers.KnownHeaders[(HeaderId)].RawValueLength =        \
                                               (USHORT) strlen(RawValue); \
    } while(FALSE)


/*******************************************************************++

Routine Description:
    The routine sends a HTTP response

Arguments:
    hReqQueue     - Handle to the request queue
    pRequest      - The parsed HTTP request
    StatusCode    - Response Status Code
    pReason       - Response reason phrase
    pEntityString - Response entity body

Return Value:
    Success/Failure.
--*******************************************************************/
DWORD SendHttpResponseFile(
  IN HANDLE        hFile,
  IN HANDLE        hReqQueue,
  IN PHTTP_REQUEST pRequest,
  IN USHORT        StatusCode,
  IN PSTR          pReason
  )
{
  HTTP_RESPONSE   response;
  HTTP_DATA_CHUNK dataChunk;
  DWORD           result;
  DWORD           bytesSent;

#pragma warning(push)
#pragma warning(disable:4127)
  // Initialize the HTTP response structure.
  INITIALIZE_HTTP_RESPONSE(&response, StatusCode, pReason);

  // Add a known header.
  ADD_KNOWN_HEADER(response, HttpHeaderContentType, "text/html");

#pragma warning(pop)

  if(hFile != INVALID_HANDLE_VALUE)
  {
    // Add an entity chunk.
    dataChunk.DataChunkType = HttpDataChunkFromFileHandle;

    ULARGE_INTEGER offset;
    offset.QuadPart = 0;
    dataChunk.FromFileHandle.ByteRange.StartingOffset = offset;

    ULARGE_INTEGER length;
    length.QuadPart = HTTP_BYTE_RANGE_TO_EOF;
    dataChunk.FromFileHandle.ByteRange.Length = length;
    dataChunk.FromFileHandle.FileHandle = hFile;

    response.EntityChunkCount         = 1;
    response.pEntityChunks            = &dataChunk;
  }

  // Because the entity body is sent in one call, it is not
  // required to specify the Content-Length.
  result = HttpSendHttpResponse(
    hReqQueue,           // ReqQueueHandle
    pRequest->RequestId, // Request ID
    0,                   // Flags
    &response,           // HTTP response
    NULL,                // pReserved1
    &bytesSent,          // bytes sent  (OPTIONAL)
    NULL,                // pReserved2  (must be NULL)
    0,                   // Reserved3   (must be 0)
    NULL,                // LPOVERLAPPED(OPTIONAL)
    NULL                 // pReserved4  (must be NULL)
    ); 

  return result;
}

DWORD SendHttpResponse(
    IN HANDLE        hReqQueue,
    IN PHTTP_REQUEST pRequest,
    IN USHORT        StatusCode,
    IN PSTR          pReason,
    IN PSTR          pEntityString
    )
{
    HTTP_RESPONSE   response;
    HTTP_DATA_CHUNK dataChunk;
    DWORD           result;
    DWORD           bytesSent;

#pragma warning(push)
#pragma warning(disable:4127)
    //
    // Initialize the HTTP response structure.
    //
    INITIALIZE_HTTP_RESPONSE(&response, StatusCode, pReason);

    //
    // Add a known header.
    //
    ADD_KNOWN_HEADER(response, HttpHeaderContentType, "text/html");
#pragma warning(pop)
   
    if(pEntityString)
    {
        // 
        // Add an entity chunk.
        //
        dataChunk.DataChunkType           = HttpDataChunkFromMemory;
        dataChunk.FromMemory.pBuffer      = pEntityString;
        dataChunk.FromMemory.BufferLength = 
                                       (ULONG) strlen(pEntityString);

        response.EntityChunkCount         = 1;
        response.pEntityChunks            = &dataChunk;
    }

    // 
    // Because the entity body is sent in one call, it is not
    // required to specify the Content-Length.
    //
    
    result = HttpSendHttpResponse(
                    hReqQueue,           // ReqQueueHandle
                    pRequest->RequestId, // Request ID
                    0,                   // Flags
                    &response,           // HTTP response
                    NULL,                // pReserved1
                    &bytesSent,          // bytes sent  (OPTIONAL)
                    NULL,                // pReserved2  (must be NULL)
                    0,                   // Reserved3   (must be 0)
                    NULL,                // LPOVERLAPPED(OPTIONAL)
                    NULL                 // pReserved4  (must be NULL)
                    ); 

    if(result != NO_ERROR)
    {
        wprintf(L"HttpSendHttpResponse failed with %lu \n", result);
    }

    return result;
}

file_url_handler::file_url_handler (const std::string &file)
: file_(file)
{
  hfile_ = CreateFileA(file.c_str(),
    GENERIC_READ, // Only need to read it
    FILE_SHARE_READ, // Only read share, no locking
    NULL, // Use process security tokens
    OPEN_EXISTING, // Never create
    0, // No special flags
    NULL); // No template file
  if (hfile_ == INVALID_HANDLE_VALUE)
    throw http_error(win32_strerror(GetLastError()));
}

bool file_url_handler::on_url_request(const std::wstring&,
                                      Http_Server *http_server,
                                      void *token)
{
  return http_server->sendfile(token, this->hfile_);
}

file_url_handler::~file_url_handler()
{
  if (this->hfile_ != INVALID_HANDLE_VALUE)
    CloseHandle(this->hfile_);
}

Http_Server::Http_Server()
: request_queue_handle_(INVALID_HANDLE_VALUE)
, thread_handle_(0)
, shutdown_event_(NULL)
{
  // Not doing any initialization in the constructor plays nicer with the ACE
  // framework.  I'm not exactly sure how the ACE framewok handles exceptions
  // being thrown in the constructor of an ACE_Singleton, but I'm pretty sure
  // it doesn't handle it well.
}

void Http_Server::init()
{
  // Step 1: Initialize Microsoft HTTP library
  HTTPAPI_VERSION ver = HTTPAPI_VERSION_1;
  ULONG result = HttpInitialize(ver,HTTP_INITIALIZE_SERVER,NULL);
  if (result != NO_ERROR)
  {
    std::string errmsg("HttpInitialize failed: ");
    if (result == ERROR_INVALID_PARAMETER)
      errmsg += "Invalid Parameter";
    else
      errmsg += win32_strerror(result);
    throw http_error(errmsg);
  }

  // Step 2: Create a handle to an HTTP request queue, one is sufficient
  result = HttpCreateHttpHandle(&this->request_queue_handle_,0);
  if (result != NO_ERROR)
  {
    std::string errmsg("HttpCreateHttpHandle failed: ");
    if (result == ERROR_DLL_INIT_FAILED)
      errmsg += "DLL Init Failed";
    else
      errmsg += win32_strerror(result);
    throw http_error(errmsg);
  }

  // Create a manual reset event to signal a shutdown when using a thread.
  this->shutdown_event_ = CreateEvent(NULL, TRUE, FALSE, NULL);
  if (this->shutdown_event_ == NULL)
    throw http_error(win32_strerror(GetLastError()));

  // Postconditions met. Once this object has been created the HTTP library
  // has been successfully initialized and an HTTP request queue has been
  // successfully created.
}

bool Http_Server::add_url(const wchar_t *url, Url_Handler *handler)
{
  bool return_value(false);
  if (url)
  {
    // Use the request queue opened in the constructor
    // to listen for requests on this URL.
    std::wstring urlstr(L"http://*:10000");
    urlstr += url;
    ULONG result = HttpAddUrl(this->request_queue_handle_,
                              urlstr.c_str(),
                              NULL);
    if (result != NO_ERROR)
    {
      std::string errmsg("Error calling HttpAddurl: ");
      switch (result)
      {
      case ERROR_ACCESS_DENIED:
        errmsg += "Access Denied\n";
        break;
      case ERROR_DLL_INIT_FAILED:
        errmsg += "DLL Init Failed\n";
        break;
      case ERROR_INVALID_PARAMETER:
        errmsg += "Invalid Parameter\n";
        break;
      case ERROR_ALREADY_EXISTS:
        errmsg += "Already exists\n";
        break;
      case ERROR_NOT_ENOUGH_MEMORY:
        errmsg += "Not Enough Memory\n";
        break;
      default:
        errmsg += win32_strerror(result);
        break;
      }
      throw http_error(errmsg);
    }
    else
    {
      Handlers::iterator handler_iterator = this->handlers_.find(url);
      if (handler_iterator == this->handlers_.end())
        return_value = this->handlers_.insert
          (std::make_pair(url,handler)).second;
    }
  }
  return return_value;
}

void Http_Server::start()
{
  this->thread_handle_ = reinterpret_cast <HANDLE>
    (_beginthreadex(0, 0, Http_Server::thread_launch_function, this, 0, 0));
  if (this->thread_handle_ == 0)
    throw http_error(strerror(errno));
}

bool Http_Server::send(void* token,
                       IN USHORT StatusCode,
                       IN PSTR pReason,
                       IN PSTR pEntityString)
{
  return (SendHttpResponse(this->request_queue_handle_,
                           (PHTTP_REQUEST) token,
                           StatusCode,
                           pReason,
                           pEntityString) == NO_ERROR);
}

bool Http_Server::sendfile(void *token,
                           HANDLE file_handle)
{
  return (SendHttpResponseFile(file_handle,
                               this->request_queue_handle_,
                               (PHTTP_REQUEST) token,
                               200,
                               "OK") == NO_ERROR);
}

void Http_Server::stop()
{
  if (this->thread_handle_ == 0)
    return; // Nothing to stop

  assert(this->shutdown_event_);
  if (this->shutdown_event_ != NULL)
  {
    if (!SetEvent(this->shutdown_event_))
      throw http_error(win32_strerror(GetLastError()));
    DWORD result = WaitForSingleObject(this->thread_handle_, INFINITE);
    if (result == WAIT_OBJECT_0)
      return;
    else if (result == WAIT_FAILED)
      throw http_error(win32_strerror(GetLastError()));
    else if (result == WAIT_ABANDONED)
      throw http_error
        ("Unexpected WAIT_ABANDONED return code from WaitForSingleObject");
    else if (result == WAIT_TIMEOUT)
      throw http_error
        ("Unexpected WAIT_TIMEOUT return code from WaitForSingleObject");
  }
}

Http_Server::~Http_Server()
{
  // Call stop() to shutdown thread if necessary
  CloseHandle(this->shutdown_event_);
  CloseHandle(this->request_queue_handle_);
  HttpTerminate(HTTP_INITIALIZE_SERVER,NULL);
}

unsigned int Http_Server::thread_launch_function(void *arg)
{
  Http_Server *http_server = static_cast <Http_Server*> (arg);
  http_server->svc();
  return 0;
}

void Http_Server::process_request(PHTTP_REQUEST request)
{
  const std::wstring url(request->CookedUrl.pAbsPath);
  Handlers::const_iterator handler_iterator = this->handlers_.find(url);
  bool handled(false);
  if (handler_iterator != this->handlers_.end())
  {
    if (handler_iterator->second)
    {
      handled = handler_iterator->second->on_url_request
        (url, this, (void*) request);
    }
  }

  if (!handled)
    SendHttpResponse(this->request_queue_handle_,
                     request,
                     404,
                     "Not found",
                     "URL not found on this server");
}

void Http_Server::svc()
{
  HTTP_REQUEST_ID request_id(HTTP_NULL_ID);
  ULONG request_buffer_size = sizeof(HTTP_REQUEST);
  // Initial size, may grow as needed but will never be shrunk
  void *request_buffer = malloc(request_buffer_size);
  if (request_buffer == 0)
    return;

  OVERLAPPED overlapped;
  memset(&overlapped, 0, sizeof(overlapped));
  overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);

  HANDLE wait_handle_array[] = {overlapped.hEvent, this->shutdown_event_};
  const size_t wait_handle_array_size(2);

  bool done(false);

  while (!done)
  {
    ULONG recv_request_result = HttpReceiveHttpRequest
      (this->request_queue_handle_,
       request_id,
       HTTP_RECEIVE_REQUEST_FLAG_COPY_BODY,
       static_cast <PHTTP_REQUEST> (request_buffer),
       request_buffer_size,
       NULL,
       &overlapped);

    switch (recv_request_result)
    {
    case NO_ERROR:
      // Success! Process the request.
      this->process_request(static_cast <PHTTP_REQUEST> (request_buffer));
      request_id = HTTP_NULL_ID; // Reset, it could've been changed
      // below on the last iteration
      break;

    case ERROR_IO_PENDING:
      // Asynchronous operation not complete
      {
        // Wait for results or shutdown command
        DWORD wait_result = WaitForMultipleObjects
          (wait_handle_array_size,
           wait_handle_array,
           FALSE,
           INFINITE);

        if (wait_result == WAIT_OBJECT_0)
        {
          DWORD bytes_xferred(0);
          if (GetOverlappedResult(this->request_queue_handle_,
                                  &overlapped,
                                  &bytes_xferred,
                                  TRUE))
          {
            // The operation succeeded, handle it
            this->process_request
              (static_cast <PHTTP_REQUEST> (request_buffer));
          }
          else
          {
            // The operation failed, find out why
            if (GetLastError() == ERROR_MORE_DATA)
            {
              PHTTP_REQUEST phttpreq = (PHTTP_REQUEST) request_buffer;
              request_id = phttpreq->RequestId;
              request_buffer = realloc(request_buffer, bytes_xferred);
              request_buffer_size = bytes_xferred;
              if (request_buffer == 0 || !ResetEvent(overlapped.hEvent))
                done = true;
              // If done is still false the next read will happen from where
              // we left off on the next iteration of the loop, we've setup
              // the arguments to the next call here. It should complete
              // immediately and have all the data.
            }
          }
        }
      }
      break;

    case ERROR_MORE_DATA:
      // Not sure if this will ever trigger with overlapped I/O, but better
      // account for it anyway. Possibly if a request is pending as the call
      // is made and the buffer is too small this might trigger.
      assert(false);
      break;

    default:
      // Any other return code indicates an error
      // It would be nice to have a way to report an error here
      done = true;
      assert(false);
      break;
    }

    // At the end of each loop, check to see if the shutdown flag has been set
    if (!done)
    {
      DWORD wait_result = WaitForSingleObject(this->shutdown_event_, 0);
      if (wait_result == WAIT_OBJECT_0)
        done = true;
      // TODO: other paranoid wait checks here
    }
  }

  // Before exiting this method and subsequently the thread, signal our going
  // away by setting the thread handle to 0.
  this->thread_handle_ = 0;
}
