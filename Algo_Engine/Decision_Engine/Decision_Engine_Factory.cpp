#include "Decision_Engine_Factory.hpp"
#include "Algo_Manager.hpp"
#include "TWAP_Decision_Engine.hpp"
#include "VWAP_Decision_Engine.hpp"
#include "ROM_Handler/RomCsv.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"

namespace dart {
  namespace algo {
    dart::Decision_Engine *create_decision_engine(const CRomCsv &order) {
      const long algotype=order.lGet(RFIX::CSV_ALGOTYPE);
      dart::Decision_Engine *decision_engine=0;
      switch (algotype) {
      case ROMVALUE::ALGO_TWAP:
        decision_engine=new dart::TWAP_Decision_Engine
          (order,dart::ALGO_MANAGER::instance());
        break;
      case ROMVALUE::ALGO_VWAP:
        decision_engine=new dart::VWAP_Decision_Engine
          (order,dart::ALGO_MANAGER::instance());
        break;
      }
      return decision_engine;
    }
  }
}
