#ifndef TPOS_UTILS_H
#define TPOS_UTILS_H

#include <string>
#include <time.h>
#include <iostream>

class MelvinStatus;

void TposGenerateError( const char * a_error );

class TposTime 
{
public:
   enum e_timeStamp
   {
      INVALID_FORMAT,
      NO_CONVERSION_NEEDED,
      CONVERTED,
      LAST_ENUM
   };

   static e_timeStamp   convertTimeStamp( std::string & a_date, std::string & a_time );
   static void          today( std::string & r_date );
   static void          stamp( std::ostream & a_os );
   static void          todayAdjust( int a_adjust, std::string & r_date );
};


class TposStatus
{
 public:
   enum TPSTATUS 
   {
      ELVIN_UNKNOWN = 0,
      ELVIN_CONNECTION_OK,
      ELVIN_CONNECTION_FAIL,
      TPOS_RTTF_MAP_CHANGE,
      TPOS_RTTF_ACCOUNT_CHANGE,
      TPOS_PROX_MAP_CHANGE,
      TPOS_PROX_ACCOUNT_CHANGE,
      TPOS_CONNECTION_CHANGE,
      TPOS_BLOCK_TRADES,
      TPOS_UNBLOCK_TRADES,
      TPOS_TURN_ON_LOGGING,
      TPOS_TURN_OFF_LOGGING,
      LAST_ENUM
   };

   TposStatus();
   TposStatus(TposStatus::TPSTATUS s) : m_type(s), m_data1(0) {}
   TposStatus(TposStatus::TPSTATUS s, int d1, const char * d2) : m_type(s), m_data1(d1), m_data2(d2) {}
   TposStatus(MelvinStatus&);
   TposStatus(TposStatus& t) : m_type(t.m_type), m_data1(t.m_data1), m_data2(t.m_data2), m_description(t.m_description) {}
   ~TposStatus() { }

   TPSTATUS     type() const;
   int          data1() const{return m_data1;};
   std::string  data2() const{return m_data2;};
   std::string  what() const;

   TposStatus& operator = (TposStatus& t)
          { m_type = t.m_type;  
            m_data1 = t.m_data1;  
            m_data2 = t.m_data2;  
            m_description = t.m_description; 
            return *this;}

 private:
   TPSTATUS    m_type;
   int         m_data1;
   std::string m_data2;
   std::string  m_description;
};


//  RDateTime,  added by WXU
//
const unsigned long  LAST2000 =  978328800UL;
const unsigned long  EPOCH    = 2177452800UL;

class RDateTime
{
  public:
    enum {
      HUNDREDTHS_PER_SECOND  =  100,
      HUNDREDTHS_PER_MINUTE  =  60 * HUNDREDTHS_PER_SECOND,
      HUNDREDTHS_PER_HOUR    =  60 * HUNDREDTHS_PER_MINUTE,
      HUNDREDTHS_PER_DAY     =  24 * HUNDREDTHS_PER_HOUR,

      SECONDS                 =  1,
      SECONDS_PER_MINUTE      =  60,
      SECONDS_PER_HOUR        =  60 * SECONDS_PER_MINUTE,
      SECONDS_PER_DAY         =  24 * SECONDS_PER_HOUR
    };

    // Construction  and Destriction
    //
    RDateTime(bool set_to_now=true);
    RDateTime(time_t utcTime);
    RDateTime(tm &localtime);
    RDateTime(const RDateTime &ts);
    RDateTime(int year, int month=0, int day=0, int hour=0, int min=0, int sec=0);
    virtual ~RDateTime();

    static RDateTime      now();
    time_t     utc() const;

    std::string     toString() const;
    std::string     format(const std::string &formatStr) const;
    void       GetLocalTime(tm &tm) const;
    bool       ParseYYYYMMDD(const char *crf);

    void       SetToNow();
    void       SetUTC(time_t utcTime);
    void       SetLocalTime(tm &localTime);
    void       Set(int year, int month, int day, int hour=0, int min=0, int sec=0);

    RDateTime&         AddDay(int days);
    RDateTime&         PrevBusinessDay();
    RDateTime&         operator=  (const RDateTime &ts);

  protected:
    time_t        m_utctime;
};

#endif
