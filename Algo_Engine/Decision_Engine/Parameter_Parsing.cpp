#include "Parameter_Parsing.hpp"
#include "Parameter_Parsing_Imp.hpp"
#include "Time_Source.hpp"
#include "Configurator/Configuration.h"
#include "bullseye/bullseye_common.hpp"

#include <string>
#include <map>

#include <ctime>


namespace dart
{
  namespace bullseye
  {
    typedef std::map <std::string, std::string> Parameters;
    bool extract_pairs (const std::string &src_str,
                        Parameters &parameters)
    {
      bool result (false);
      std::istringstream reader (src_str);
      while (reader) {
        std::string key, value;
        if (std::getline (reader, key, '=') && !key.empty ()) {
          if (std::getline (reader, value, ' ') && !value.empty ()) {
            parameters [key] = value;
            result = true;
          }
          else {
            result = false;
            break;
          }
        }
      }

      return result;
    }
  }

  namespace time
  {
    bool
    parse_time_str (const std::string &time_str, time_t &time_point)
    {
      tm time_fields;
      memset (&time_fields, 0, sizeof (time_fields));
      const int fields_read = sscanf (time_str.c_str (),
                                      "%4d%2d%2d-%2d:%2d:%2d",
                                      &time_fields.tm_year,
                                      &time_fields.tm_mon,
                                      &time_fields.tm_mday,
                                      &time_fields.tm_hour,
                                      &time_fields.tm_min,
                                      &time_fields.tm_sec);

      if (fields_read != 6 && fields_read != 5) {
        return false;
      }

      time_fields.tm_year -= 1900;
      --time_fields.tm_mon;
      time_point = _mkgmtime (&time_fields);

      return true;
    }

    bool isdst (const time_t &t, bool &dst_flag)
    {
      // Even though localtime might give us Central time if the server is in
      // Chicago, the DST flag is still correct for New York since they follow
      // the same DST rules.  However, if this server is placed in a time zone
      // outside the United States there will be problems.  In either case, if
      // DST checking is turned off in the OS (the "Adjust for daylight savings"
      // checkbox in the time control applet on Windows) this will not be correct
      // for the time of the year we're in DST.
      tm *ptm = localtime (&t);
      if (ptm) {
        dst_flag = (ptm->tm_isdst != 0);
        return true;
      }
      else {
        return false;
      }
    }

    bool get_ny_utc_offset (time_t t, int *poffset)
    {
      bool result (false);
      if (poffset) {
        bool dst_flag (false);
        if (isdst (t, dst_flag)) {
          result = true;
          if (dst_flag) {
            *poffset = -4;
          }
          else {
            *poffset = -5;
          }
        }
      }
      return result;
    }

    bool get_ny_time (time_t *nytime, int hour, int min)
    {
      bool result = false;

      if (nytime) {
        tm *time_struct = gmtime (nytime);
        if (time_struct) {
          int ny_utc_offset (-5);
          if (get_ny_utc_offset ((*nytime), &ny_utc_offset)) {
            time_struct->tm_hour = hour - ny_utc_offset;
            time_struct->tm_min = min;
            time_struct->tm_sec = 0;
            (*nytime) = _mkgmtime (time_struct);
            if ((*nytime) != -1) {
              result = true;
            }
          }
        }
      }

      return result;
    }

    bool get_ny_market_start_time_imp(time_t *start_time) {
      std::string startstr
        (CONFIGURATION::instance()->get("Bullseye","NYMarketStart"));
      if (startstr.empty())
        startstr="09:30";
      int hour=-1,min=-1;
      if (sscanf(startstr.c_str(),"%d:%d",&hour,&min)!=2) {
        DART_WARNING
          ("Error parsing NYMarketStart string: %s, defaulting to 09:30\n",
           startstr.c_str());
        hour=9;
        min=30;
      }
      DART_DEBUG("Using NYMarketStart=%02d:%02d\n",hour,min);
      return get_ny_time(start_time,hour,min);
    }

    bool get_ny_market_stop_time_imp(time_t *stop_time) {
      std::string startstr
        (CONFIGURATION::instance()->get("Bullseye","NYMarketEnd"));
      if (startstr.empty())
        startstr="16:00";
      int hour=-1,min=-1;
      if (sscanf(startstr.c_str(),"%d:%d",&hour,&min)!=2) {
        DART_WARNING
          ("Error parsing NYMarketEnd string: %s, defaulting to 16:00\n",
           startstr.c_str());
        hour=16;
        min=0;
      }
      DART_DEBUG("Using NYMarketEnd=%02d:%02d\n",hour,min);
      return get_ny_time(stop_time,hour,min);
    }

    bool get_ny_market_start_time (time_t *start_time)
    {
      (*start_time) = (dart::time::TIME_SOURCE::instance()->current()).sec();
      return get_ny_market_start_time_imp (start_time);
    }

    bool get_ny_market_stop_time (time_t *stop_time)
    {
      (*stop_time) = (dart::time::TIME_SOURCE::instance()->current()).sec();
      return get_ny_market_stop_time_imp (stop_time);
    }
  }
}
