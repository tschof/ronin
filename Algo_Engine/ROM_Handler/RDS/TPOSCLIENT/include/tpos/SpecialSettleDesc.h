#ifndef SPECIAL_SETTLE_DESC_H
#define SPECIAL_SETTLE_DESC_H

#include <string>

/*
recordID                       varchar                                 10 NULL
symbolID                       varchar                                 10 NULL
productKind                    varchar                                 10 NULL
strikeCurrency                 varchar                                 10 NULL
onnProductCode                 varchar                                 10 NULL
numDeliveryComp                int                                      4 NULL
delivComponentNumber           int                                      4 NULL
undSymbolID                    varchar                                 10 NULL
cusipID                        varchar                                 10 NULL
delivUnits                     int                                      4 NULL
settleMethod                   varchar                                 10 NULL
strikePercent                  int                                      4 NULL
fixedAmount                    real                                     4 NULL
processDate                    varchar                                 20 NULL
settleDate                     varchar                                 20 NULL
*/

extern const char * s_recordID;
extern const char * s_symbolID;
extern const char * s_productKind;
extern const char * s_strikeCurrency;
extern const char * s_onnProductCode;
extern const char * s_numDeliveryComp;
extern const char * s_delivComponentNumber;
extern const char * s_undSymbolID;
extern const char * s_cusipID;
extern const char * s_delivUnits;
extern const char * s_settleMethod;
extern const char * s_strikePercent;
extern const char * s_fixedAmount;
extern const char * s_processDate;
extern const char * s_settleDate;

class SpecialSettleDesc
{
public:
   SpecialSettleDesc();
   SpecialSettleDesc(const SpecialSettleDesc & a_old );
   ~SpecialSettleDesc();

   bool setDesc( const char * a_desc );
   void getDesc( std::string & r_info ) const;

   int setRecordID( const char * a_info );
   int setSymbolID( const char * a_info );
   int setProductKind( const char * a_info );
   int setStrikeCurrency( const char * a_info );
   int setOnnProductCode( const char * a_info );
   int setNumDeliveryComp( int a_info );
   int setDelivComponentNumber( int a_info );
   int setUndSymbolID( const char * a_info );
   int setCusipID( const char * a_info );
   int setDelivUnits( int a_info );
   int setSettleMethod( const char * a_info );
   int setStrikePercent( int a_info );
   int setFixedAmount( double a_info );
   int setProcessDate( const char * a_info );
   int setSettleDate( const char * a_info );

   const std::string & getRecordID() const;
   const std::string & getSymbolID() const;
   const std::string & getProductKind() const;
   const std::string & getStrikeCurrency() const;
   const std::string & getOnnProductCode() const;
   int getNumDeliveryComp() const;
   int getDelivComponentNumber() const;
   const std::string & getUndSymbolID() const;
   const std::string & getCusipID() const;
   int getDelivUnits() const;
   const std::string & getSettleMethod() const;
   int getStrikePercent() const;
   double getFixedAmount() const;
   const std::string & getProcessDate() const;
   const std::string & getSettleDate() const;
  
private:
   std::string m_recordID;
   std::string m_symbolID;
   std::string m_productKind;
   std::string m_strikeCurrency;
   std::string m_onnProductCode;
   int m_numDeliveryComp;
   int m_delivComponentNumber;
   std::string m_undSymbolID;
   std::string m_cusipID;
   int m_delivUnits;
   std::string m_settleMethod;
   int m_strikePercent;
   double m_fixedAmount;
   std::string m_processDate;
   std::string m_settleDate;
};

#endif
