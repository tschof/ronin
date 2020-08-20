#ifndef __VOLUME_PROFILE_HPP__
#define __VOLUME_PROFILE_HPP__

#include <vector>

struct Volume_Profile {
  Volume_Profile()
    : start_hour(-1),
      start_min(-1),
      stop_hour(-1),
      stop_min(-1)
  {}

  int start_hour;
  int start_min;
  int stop_hour;
  int stop_min;

  struct Vol_Profile_Entry {
    size_t time_offset;
    double vol_pct;
  };

  typedef std::vector<Vol_Profile_Entry> Vol_Prof_Entries;
  Vol_Prof_Entries entries;
};

#endif
