#include "VWAP_Decision_Engine.hpp"
#include "Volume_Profile_Data_Source.hpp"
#include <cassert>
#include "bullseye/bullseye_common.hpp"
#include "Parameter_Parsing.hpp"

// TODO: Make the use of the volume profiles more robust so if things like the
//       interval of data used changes, the code will handle that change
//       gracefully.

/**
 * Since our data is in x minute intervals we need to be able to calculate the
 * relative volume at any given point along the curve.  This function uses a
 * linear interpolation to calculate a relative volume for any given point on
 * the curve. 
 */
double relative_volume_from_offset
  (const Volume_Profile::Vol_Prof_Entries &vol_entries,
   size_t offset) {
  // Search the data set for either:
  // 1) The actual data point, or
  // 2) The nearest two data points and do a linear
  //    interpolation to find the required data point.
  Volume_Profile::Vol_Prof_Entries::const_iterator p1=vol_entries.end();
  Volume_Profile::Vol_Prof_Entries::const_iterator p2=vol_entries.end();
  for (Volume_Profile::Vol_Prof_Entries::const_iterator vol_prof_iter=vol_entries.begin();
    vol_prof_iter!=vol_entries.end();++vol_prof_iter) {
      if (vol_prof_iter->time_offset==offset)
        return vol_prof_iter->vol_pct;
      else if (vol_prof_iter->time_offset<offset)
        p1=vol_prof_iter;
      else if (vol_prof_iter->time_offset>offset) {
        p2=vol_prof_iter;
        break;
      }
  }

  // Once the previous loop is done and we reach here we know that the exact
  // offset has not been found and that p1 is the nearest point that is less
  // than the offset we need and p2 is the nearest point greater than the
  // offset we need.  If p1 or p2 is equal to the initial value (the end of
  // the sequence) we have an error.
  if (p1==vol_entries.end() || p2==vol_entries.end())
    throw std::runtime_error("Error linearly interpolating offset data");

  double interval_rel_volume=p2->vol_pct-p1->vol_pct;
  double rel_vol_per_sec=interval_rel_volume/(p2->time_offset-p1->time_offset);
  double relative_volume=p1->vol_pct+(rel_vol_per_sec*(offset-p1->time_offset));
  return relative_volume;
}

size_t offset_from_relative_volume
  (const Volume_Profile::Vol_Prof_Entries &/*vol_entries*/,
   double /*relative_volume*/) {
     return 0;
}

time_t time_today(int hour,int min) {
  time_t now=0;
  now=time(&now);
  struct tm today;
  struct tm *ptm=localtime(&now);
  if (ptm==0)
    throw std::runtime_error("localtime failed");
  today=*ptm;
  today.tm_hour=hour;
  today.tm_min=min;
  time_t result=mktime(&today);
  if (result==-1)
    throw std::runtime_error("mktime failed");
  return result;
}

/**
 * Given a time_t, verify it falls on the volume profile curve and calculate
 * its offset.
 */
size_t calc_time_offset(const Volume_Profile &vol_profile, time_t t) {
  // Find the time_t (time today) corresponding to the first and last data
  // point in the volume curve, verify the above time_t falls within it and
  // calculate the offset
  try {
    time_t start=time_today(vol_profile.start_hour,vol_profile.start_min);
    time_t stop=time_today(vol_profile.stop_hour,vol_profile.stop_min);
    if (t<start || t>stop)
      throw std::runtime_error("Time not covered by volume profile");
    return static_cast<size_t>(t-start);
  }
  catch (const std::exception &e) {
    DART_WARNING
      ("Error while calculating start/stop times for volume profiles: %s",
       e.what());
    throw std::runtime_error("Cannot calculate time offset");
  }
}

namespace dart
{
  VWAP_Decision_Engine::VWAP_Decision_Engine
    (const CRomCsv &csv,Algo_Manager *algomgr)
    : Decision_Engine(csv,algomgr),
      symbol(csv.GetAt(RFIX::CSV_SYMBOL)),
      customer_order(csv)
  {}

  int VWAP_Decision_Engine::setup_initial_timer() {
    int result=-1;
    Volume_Profile *volume_profile=
      VOLUME_PROFILE_DATA_SOURCE::instance()->get_vol_profile
        (this->symbol.c_str());
    try {
      if (volume_profile) {
        // Start/stop time
        std::string start_time_str=this->customer_order.GetAt(RFIX::CSV_START_TIME);
        std::string end_time_str=this->customer_order.GetAt(RFIX::CSV_END_TIME);
        time_t start_time=::time(0);
        if (!start_time_str.empty()
            && !dart::time::parse_time_str(start_time_str,start_time))
          throw std::runtime_error("bad start time");
        time_t end_time=0;
        if (end_time_str.empty()) {
          if (!dart::time::get_ny_market_stop_time(&end_time))
            throw std::runtime_error("could not get NY market stop time");
        }
        else if (!dart::time::parse_time_str(end_time_str,end_time))
          throw std::runtime_error("bad stop time");

        // Now we have start and stop time_t
        const size_t start_offset_for_today=
          calc_time_offset(*volume_profile,start_time);
        const size_t stop_offset_for_today=
          calc_time_offset(*volume_profile,end_time);

        if (stop_offset_for_today<start_offset_for_today)
          throw std::runtime_error("stop time before start time");

        double rel_vol_at_start=
          relative_volume_from_offset(volume_profile->entries,start_offset_for_today);
        double rel_vol_at_end=
          relative_volume_from_offset(volume_profile->entries,stop_offset_for_today);


        double relative_volume_for_life_of_order=rel_vol_at_end-rel_vol_at_start;
        relative_volume_for_life_of_order;
      }
    }
    catch (const std::exception &e) {
      DART_WARNING("Cannot setup initial timer: %s\n",e.what());
    }
    return result;
  }
}
