#include "Volume_Profile_Data_Source.hpp"
#include "Vol_Curve_Parser.hpp"
#include "bullseye/bullseye_common.hpp"

int Volume_Profile_Data_Source::init(const char *dir) {
  int result=-1;
  if (dir) {
    this->dir_=dir;
    if (!this->dir_.empty() && _access(this->dir_.c_str(),06)==0)
      result=0;
  }
  return result;
}

Volume_Profile*
Volume_Profile_Data_Source::get_vol_profile(const char *symbol) {
  Volume_Profile *profile=new Volume_Profile;
  std::string filename(this->dir_);
  try{
    const char lastchar=filename.at(filename.size()-1);
    if (lastchar!='/' && lastchar!='\\')
      filename+='\\';
    filename+=symbol;
    filename+=".txt";
    parse_vol_curve_file(filename.c_str(),*profile);
  }catch(const std::exception&e){
    DART_WARNING
      ("VWAP: Error getting volume data for %s: %s\n",symbol,e.what());
    delete profile;
    profile=0;
  }
  return profile;
}
