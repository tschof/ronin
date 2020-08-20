#ifndef __TIME_SLICE_MANAGER_HPP__
#define __TIME_SLICE_MANAGER_HPP__
class CRomCsv;
struct Interval;
namespace dart { class TWAP_Decision_Engine; }
class Time_slice_manager {
public:
  virtual void time_slice(const Interval&)=0;
  virtual void volume(size_t)=0;
  virtual void customer_order(CRomCsv&)=0;
  virtual void decision_engine(dart::TWAP_Decision_Engine*)=0;
  virtual bool init()=0;
  virtual bool start()=0;
  virtual bool abort()=0;
  virtual bool isdone() const=0;
  virtual const char *lasterror() const=0;
};
#endif
