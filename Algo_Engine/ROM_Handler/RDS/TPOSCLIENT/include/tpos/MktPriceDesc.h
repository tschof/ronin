#ifndef __INTERFACE_MKTPRICE_DESC_H__
#define __INTERFACE_MKTPRICE_DESC_H__

#include <string>
#include <map>

// class MktPriceDesc;
// typedef  std::map<std::string, double >  MktPriceMap;

class MktPriceDesc
{
 public:
   MktPriceDesc();
   MktPriceDesc(const MktPriceDesc & a_old );
   virtual ~MktPriceDesc();

   bool setDesc( const char * a_desc );
   void getDesc( std::string & r_info ) const;

   int setRic(const char* a_info);
   int setType(const char* a_type);
   int setRicGroup(const char* a_info);
   int setMktDate(const char* a_info);
   int setTrdDate(const char* a_info);
   int setPrice( float a_info);
   int setSource( const char* a_info);

   const std::string& getRic() const;
   const std::string& getType() const;
   const std::string& getRicGroup() const;
   const std::string& getMktDate() const;
   const std::string& getTrdDate() const;
   float getPrice() const;
   const std::string& getSource() const;
  
 private:
   std::string m_ric;
   std::string m_ricgrp;
   std::string m_type;
   std::string m_mktDate;
   std::string m_trdDate;
   float       m_price;
   std::string m_priceSource;
};

class MktPriceMap : public std::map<std::string, MktPriceDesc *>
{
   public:
      virtual ~MktPriceMap() { free(); }
      void    free();
};

typedef std::map<std::string, MktPriceDesc *>::iterator MktPriceMapItor;

#endif
