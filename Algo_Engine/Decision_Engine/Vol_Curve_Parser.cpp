#define _CRT_SECURE_NO_WARNINGS
#include "Vol_Curve_Parser.hpp"
#include <string>
#include <stdexcept>
#include <cstdio>
#include <sstream>
void parse_vol_curve_file(const char *filename, Volume_Profile &profile) {
  FILE *f=fopen(filename,"r");
  if (f==0)
    throw std::runtime_error
      (std::string(strerror(errno))+std::string(": opening file"));
  int hour=-1,min=-1,sec=-1;
  char ampm[3]={0,0,0};
  float pct=0.0;
  size_t line=1;
  const size_t market_open_offset=(8*60*60)+(30*60);
  try {
    while (!feof(f)) {
      if (fscanf(f,"%d:%d:%d %c%c\t%f%%\n",
            &hour,&min,&sec,&ampm[0],&ampm[1],&pct)!=6)
        throw std::runtime_error("could not scan line");
      if (hour<0||hour>12)
        throw std::runtime_error("invalid hour");
      if (min<0||min>59)
        throw std::runtime_error("invalid min");
      if (sec<0||sec>59)
        throw std::runtime_error("invalid sec");
      if (_stricmp(ampm,"am")!=0 && _stricmp(ampm,"pm")!=0)
        throw std::runtime_error(std::string("am/pm expected: ")+"ampm");
      if (pct<0.0||pct>100.0)
        throw std::runtime_error("invalid pct");
      // Validates okay, create profile entry
      if (hour!=12&&_stricmp(ampm,"pm")==0)
        hour+=12;//Normalize to 24-hour clock
      if (hour<8||hour>15)
        throw std::runtime_error("Market hours only");
      Volume_Profile::Vol_Profile_Entry entry;
      entry.time_offset=(hour*60*60)+(min*60)+sec;
      entry.time_offset-=market_open_offset;
      entry.vol_pct=pct;
      profile.entries.push_back(entry);
      // start_* are initialized with -1, we detect the first iteration by that
      if (profile.start_hour==-1)
        profile.start_hour=hour;
      if (profile.start_min==-1)
        profile.start_min=min;
      // Always set the stop times, when we return this should be accurate then
      profile.stop_hour=hour;
      profile.stop_min=min;
      ++line;
    }
  }
  catch (const std::exception &e) {
    std::ostringstream msgwriter;
    msgwriter<<filename<<" ("<<line<<"): "<<e.what();
    throw std::runtime_error(msgwriter.str());
  }
}
