#ifndef __VOLUME_PROFILE_DATA_HPP__
#define __VOLUME_PROFILE_DATA_HPP__

#include "ace/Singleton.h"
#include "Volume_Profile.hpp"
#include <string>

/**
 * @class Volume_Profile_Data_Source
 * @brief Manages the creation and caching
 * of volume profiles for various symbols.
 */
class Volume_Profile_Data_Source {
public:
  int init(const char *dir);
  Volume_Profile *get_vol_profile(const char *symbol);
private:
  std::string dir_;
};

typedef ACE_Singleton<Volume_Profile_Data_Source,ACE_SYNCH_MUTEX>
  VOLUME_PROFILE_DATA_SOURCE;

#endif
