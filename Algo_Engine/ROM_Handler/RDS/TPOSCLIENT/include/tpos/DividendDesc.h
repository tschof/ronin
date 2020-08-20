#ifndef __DIVIDEND_DESC_H__
#define __DIVIDEND_DESC_H__

#include <string>
#include <set>

/*
InstrumentID                   numeric 
DivDate                        smalldatetime
Dividend                       float
*/

class DividendDesc
{
 public:
   DividendDesc();
   DividendDesc(const DividendDesc & a_old );
   virtual ~DividendDesc();

   bool setDesc( const char * a_desc );
   void getDesc(std::string & r_info ) const;

   int setInstrumentID(const char* a_info);
   int setDivDate(const char* a_info);
   int setAmount( float a_info);

   const std::string& getInstrumentID() const;
   const std::string& getDivDate() const;
   float getAmount() const;
  
private:
   std::string       m_instrumentID;
   std::string       m_divDate;
   float          m_amount;
};

struct ltdiv { 
   bool operator() (const DividendDesc* d1, const DividendDesc* d2) const
   {   
       if ( d2 == 0 )
          return false;
       else if ( d1 == 0 )
          return true;
       int ret = d1->getInstrumentID().compare(d2->getInstrumentID());
       if ( ret == 0 )
            return (d1->getDivDate().compare(d2->getDivDate()) < 0);
       else return (ret < 0);
   }
};

// typedef set<DividendDesc*, ltdiv> DividendSet;
class DividendSet : public std::set<DividendDesc*, ltdiv> 
{
   public:
     virtual ~DividendSet() { free(); }
     void    free();
};

#endif
