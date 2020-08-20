#ifndef __DECISION_ENGINE_FACTORY_HPP__
#define __DECISION_ENGINE_FACTORY_HPP__

class CRomCsv;
namespace dart {
  class Decision_Engine;
  namespace algo {
    dart::Decision_Engine *create_decision_engine(const CRomCsv &order);
  }
}

#endif
