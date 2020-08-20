#include <ace/Log_msg.h>
#include "Generic_DLL_Market_Data_Source.hpp"
#include <libdarthttp/win32_error.hpp>

namespace dart {
  Generic_DLL_Market_Data_Source::Generic_DLL_Market_Data_Source()
    : dll_handle_(NULL)
    , init_func_(0)
    , subscribe_func_(0)
    , unsubscribe_func_(0)
    , init_price_block_func_(0)
    , get_price_block_func_(0)
    , shutdown_func_(0)
  {}

  Generic_DLL_Market_Data_Source::~Generic_DLL_Market_Data_Source() {
    if (this->shutdown_func_)
      this->shutdown_func_();
    if (dll_handle_)
      FreeLibrary(dll_handle_);
  }

  int Generic_DLL_Market_Data_Source::open(
    const char *dll,
    const char *configfile,
    const char *configsection)
  {
    int result = -1;
    dll_handle_ = LoadLibraryA(dll);
    if (dll_handle_) {
      this->init_func_ =
        (PDART_MD_INIT_FUNC) GetProcAddress
          (this->dll_handle_, "DART_MD_init");

      this->subscribe_func_ =
        (PDART_MD_SUBSCRIBE_FUNC) GetProcAddress
          (this->dll_handle_, "DART_MD_subscribe");

      this->unsubscribe_func_ =
        (PDART_MD_UNSUBSCRIBE_FUNC) GetProcAddress
          (this->dll_handle_, "DART_MD_unsubscribe");

      this->init_price_block_func_ =
        (PDART_MD_INIT_PRICE_BLOCK_FUNC) GetProcAddress
          (this->dll_handle_, "DART_MD_init_price_block");

      this->get_price_block_func_ =
        (PDART_MD_GET_PRICE_BLOCK_FUNC) GetProcAddress
          (this->dll_handle_, "DART_MD_get_price_block");

      this->shutdown_func_ =
        (PDART_MD_SHUTDOWN) GetProcAddress
          (this->dll_handle_,"DART_MD_shutdown");

      if (this->init_func_
          && this->subscribe_func_
          && this->unsubscribe_func_
          && this->init_price_block_func_
          && this->get_price_block_func_
          && this->shutdown_func_
          && this->init_func_(configfile,configsection)==0)
      {
        result = 0;
      }
    }
    else {
      DWORD lasterror=GetLastError();
      ACE_DEBUG ((LM_ERROR,
                  "%s: Unable to load DLL with error %d: %s",
                  __FUNCTION__,
                  lasterror,
                  win32_strerror(lasterror).c_str()));
    }
    return result;
  }

  int Generic_DLL_Market_Data_Source::subscribe_symbol (const char *symbol) {
    if (dll_handle_)
      return this->subscribe_func_(symbol);
    else
      return -1;
  }

  int Generic_DLL_Market_Data_Source::unsubscribe_symbol (const char*) {
    return -1;
  }

  Price_Block *Generic_DLL_Market_Data_Source::get_price_block
    (const char *symbol) const
  {
    if (dll_handle_) {
      Price_Block *pblock=0;
      try {
        pblock = new Price_Block;
        if (this->init_price_block_func_(pblock)==-1
          || this->get_price_block_func_(symbol,pblock)==-1) {
          delete pblock;
          pblock=0;
        }
      }
      catch (...)
      {
      }
      return pblock;
    }
    else
      return 0;
  }

  Price_Block *Generic_DLL_Market_Data_Source::get_price_block
    (const std::string &symbol) const
  {
    if (dll_handle_)
      return this->get_price_block (symbol.c_str());
    else
      return 0;
  }
}
