#ifndef __VWAP_DECISION_ENGINE_HPP__
#define __VWAP_DECISION_ENGINE_HPP__

#include "Decision_Engine.hpp"
#include <string>

namespace dart
{
  class VWAP_Decision_Engine : public Decision_Engine
  {
  public:
    VWAP_Decision_Engine(const CRomCsv&,Algo_Manager*);
  protected:
    virtual int setup_initial_timer();
  private:
    std::string symbol;
    CRomCsv customer_order;
  };
}

#endif
