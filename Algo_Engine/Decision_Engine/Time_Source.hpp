#ifndef __TIME_SOURCE_HPP__
#define __TIME_SOURCE_HPP__

// ACE
#include <ace/singleton.h>
#include <ace/Time_Value.h>
#include <ace/OS.h>

// Std C++
#include <stdexcept>

namespace dart {
  namespace time {
    class Time_Source {
    public:
      Time_Source () : frozen_time_point_ (ACE_Time_Value::zero) {}

      ACE_Time_Value current() const {
        if (frozen_time_point_ == ACE_Time_Value::zero)
          return ACE_OS::gettimeofday();
        else
          return frozen_time_point_;
      }

      void current (time_t t) {
        if (this->frozen_time_point_ != ACE_Time_Value::zero)
          this->frozen_time_point_ = t;
        else
          throw std::runtime_error ("cannot set time unless time is frozen");
      }

      void current (ACE_Time_Value &t) {
        if (this->frozen_time_point_ != ACE_Time_Value::zero)
          this->frozen_time_point_ = t;
        else
          throw std::runtime_error ("cannot set time unless time is frozen");
      }

      void freeze_time (time_t t) {
        this->frozen_time_point_ = t;
      }

      void freeze_time (ACE_Time_Value &t) {
        this->frozen_time_point_ = t;
      }

      void unfreeze_time () {
        this->frozen_time_point_ = ACE_Time_Value::zero;
      }

      void increment (time_t t) {
        if (this->frozen_time_point_ != ACE_Time_Value::zero)
          this->frozen_time_point_ += t;
        else
          throw std::runtime_error ("cannot increment time unless time is frozen");
      }

      void increment (ACE_Time_Value &t) {
        if (this->frozen_time_point_ != ACE_Time_Value::zero)
          this->frozen_time_point_ += t;
        else
          throw std::runtime_error ("cannot increment time unless time is frozen");
      }

    private:
      ACE_Time_Value frozen_time_point_;
    };

    typedef ACE_Singleton <Time_Source, ACE_SYNCH_MUTEX> TIME_SOURCE;
  }
}

#endif
