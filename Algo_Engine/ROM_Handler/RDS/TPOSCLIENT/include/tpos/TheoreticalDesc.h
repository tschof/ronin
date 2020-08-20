#ifndef __INTERFACE_TMLIGHT_DESC_H__
#define __INTERFACE_TMLIGHT_DESC_H__

#include <string>
#include <map>
#include <vector>

/*
InstrumentID                   numeric 
GroupingID                     numeric
TheoDateAndTime                smalldatetime
Price                          float
Delta                          float
BSDelta                        float
Gamma                          float
BSGamma                        float
Theta                          float
Vega                           float
Rho                            float
ImpliedVol                     float
LastModifiedBy                 varchar    
LastModifiedDateAndTime        smalldatetime
*/

extern const char * s_instrumentID;
extern const char * s_undInstrumentID;
extern const char * s_groupingID;
extern const char * s_theoDateTime;
extern const char * s_price;
extern const char * s_delta;
extern const char * s_bsdelta;
extern const char * s_gamma;
extern const char * s_bsgamma;
extern const char * s_theta;
extern const char * s_vega;
extern const char * s_rho;
extern const char * s_impliedVol;

class TheoreticalDesc
{
public:
   TheoreticalDesc();
   TheoreticalDesc(const TheoreticalDesc & a_old );
   virtual ~TheoreticalDesc();

   bool setDesc( const char * a_desc );
   void getDesc( std::string & r_info ) const;

   int setInstrumentID(const char* a_info);
   int setUndInstrumentID(const char* a_info);
   int setGroupingID(const char* a_info);
   int setEffectGroupingID(const char* a_info);
   int setTheoDateTime(const char* a_info);
   int setPrice( float a_info);
   int setDelta( float a_info);
   int setBSDelta( float a_info);
   int setGamma( float a_info);
   int setBSGamma( float a_info);
   int setTheta( float a_info);
   int setVega( float a_info);
   int setRho( float a_info);
   int setImpliedVol( float a_info);

   const std::string& getInstrumentID() const;
   const std::string& getUndInstrumentID() const;
   const std::string& getGroupingID() const;
   const std::string& getEffectGroupingID() const;
   const std::string& getTheoDateTime() const;
   float getPrice() const;
   float getDelta() const;
   float getBSDelta() const;
   float getGamma() const;
   float getBSGamma() const;
   float getTheta() const;
   float getVega() const;
   float getRho() const;
   float getImpliedVol() const;
  
private:
   std::string m_instrumentID;
   std::string m_undInstrumentID;
   std::string m_groupingID;
   std::string m_effectGroupingID;
   std::string m_theoDateTime;
   float       m_price;
   float       m_delta;
   float       m_bsdelta;
   float       m_gamma;
   float       m_bsgamma;
   float       m_theta;
   float       m_vega;
   float       m_rho;
   float       m_impliedVol;
};

typedef std::vector<TheoreticalDesc *> TheoreticalVector;
// typedef std::map<string, TheoreticalDesc *> TheoreticalMap;

class TheoMsgCallback
{
   public:
      virtual ~TheoMsgCallback() {}
      virtual void onChange(TheoreticalDesc*) = 0;
};

class TheoreticalMap : public std::map<std::string, TheoreticalDesc *>
{
   public:
      virtual ~TheoreticalMap() { free(); }
      void    free();
 
      // TheoreticalDesc*  operator [] (std::string& k) 
      // { return  std::map<std::string, TheoreticalDesc*>::operator [](k); }
};

typedef std::multimap<std::string, TheoreticalDesc *>::iterator TheoDescItor;

#endif
