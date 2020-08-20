#ifndef TPOS_PUB_MESSAGE_H
#define TPOS_PUB_MESSAGE_H

#include <string>
#include <iostream>
class MelvinWrapMessage;
class RtpNvGeneric;

class TposPubMessage
{ 
public:
   TposPubMessage( );
   TposPubMessage( const char * a_blob );
   TposPubMessage( MelvinWrapMessage * );
   TposPubMessage( RtpNvGeneric * );
   ~TposPubMessage();

   int getInt( const char * a_name, int & r_value ) const;
   int getFloat( const char * a_name, float & r_value ) const;
   int getDouble( const char * a_name, double & r_value ) const;
   int getString( const char * a_name, char *& r_value ) const;

   void setInt( const char * a_name, int a_value );
   void setFloat( const char * a_name, float a_value );
   void setDouble( const char * a_name, double a_value );
   void setString( const char * a_name, const char * a_value );

   friend class TposClientConnection;

private:
   MelvinWrapMessage * m_msg;
   RtpNvGeneric * m_nvmsg;
};

#endif
