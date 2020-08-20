#ifndef __PARAMETER_PARSING_HPP__
#define __PARAMETER_PARSING_HPP__

#include <string>
#include <map>
#include <sstream>


namespace dart
{
  namespace bullseye
  {
    typedef std::map <std::string, std::string> Parameters;
    bool extract_pairs (const std::string &, Parameters&);
  }

  namespace time
  {
    // Time helper functions

    /**
     * @brief Parses standard CSV ROM timestamp
     *
     * The standard format is YYYYMMDD-HH:MM:SS.  This is also the FIX standard
     * timestamp format.  The time is always represented in UTC to eliminate
     * ambiguity.
     */
    bool parse_time_str (const std::string&, time_t&);

    /**
     * @brief Retrieves a time_t representing the point in time corresponding
     * to US equity market open in New York relative to the current time of
     * day.
     *
     * @arg start_time Receives the time point indicating the start of New York
     * trading; the timepoint is relative to the current day and is set to the
     * start of trading on the day it is called.  Because of the quick and dirty
     * implementation the only hours this is guaranteed to work is during the
     * trading day, i.e. approximately 9:30am ET to 4:00pm ET.
     */
    bool get_ny_market_start_time (time_t* /*start_time*/);

    /**
     * @brief Retrieves a time_t representing the point in time corresponding
     * to US equity market close in New York relative to the current time of
     * day.
     *
     * @arg start_time Receives the time point indicating the end of New York
     * trading; the timepoint is relative to the current day and is set to the
     * start of trading on the day it is called.  Because of the quick and dirty
     * implementation the only hours this is guaranteed to work is during the
     * trading day, i.e. approximately 9:30am ET to 4:00pm ET.
     */
    bool get_ny_market_stop_time (time_t* /*stop_time*/);
  }
}

#endif
