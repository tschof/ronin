#ifndef __ALGO_MANAGER_HPP__
#define __ALGO_MANAGER_HPP__

// ACE
#include <ace/Task.h>
#include <ace/Reactor.h>
#include <ace/Reactor_Notification_Strategy.h>
#include <ace/Asynch_Acceptor.h>
#include <ace/Asynch_Connector.h>

// Std C++
#include <map>
#include <string>
#include <vector>

class CRomCsv;
class Session;

namespace dart
{
  class Decision_Engine;
  class Market_Data_Source;
  class TWAP_Order;
  class ROM_Connection;

  class Algo_Manager
    : public ACE_Task <ACE_MT_SYNCH>
  {
  public:
    Algo_Manager();
    virtual ~Algo_Manager();

    virtual int open(void* = 0);
    virtual int handle_output (ACE_HANDLE = ACE_INVALID_HANDLE);
    virtual int put (ACE_Message_Block *, ACE_Time_Value * = 0);

    void set_market_data_source (Market_Data_Source*);
    Market_Data_Source *get_market_data_source ();

    void outbound_connection (Session*);
    Session *outbound_connection ();

    void inbound_connection (ROM_Connection*);
    ROM_Connection *inbound_connection ();

    void send_csv_to_client (CRomCsv&);
    void reject_order (const CRomCsv&, const char*);
    void accept_order (const CRomCsv&);

  private:
    ACE_Reactor reactor_;
    ACE_Reactor_Notification_Strategy notifier_;

    Market_Data_Source *md_src_;
    Session *order_dest_;
    ROM_Connection *order_source_;

    typedef std::map <std::string, TWAP_Order*> Tag_Map_T;
    Tag_Map_T tag_map_;

    typedef std::map <std::string,std::string> Tag_To_Src_T;
    Tag_To_Src_T tag2src_;
  };

  typedef ACE_Singleton <dart::Algo_Manager,ACE_SYNCH_MUTEX> ALGO_MANAGER;
}

#endif
