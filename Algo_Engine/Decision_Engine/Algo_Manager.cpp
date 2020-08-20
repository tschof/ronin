// ACE
#include <ace/os_include/sys/os_types.h>
#include <ace/os.h>

// Decision Engine
#include "Algo_Manager.hpp"
#include "Order_Book.hpp"
#include "Decision_Engine_Factory.hpp"
#include "Decision_Engine.hpp"

// App
#include <Bullseye/Bullseye_Common.hpp>

// Market Data
#include <Market_Data/Market_Data_Source.hpp>
#include <Market_Data/Price_Block.hpp>

// Connectivity
#include "ROM_Handler/RomCsv.h"
#include "COMMON\ROMFIXENGINE\FixEngine\romvaluedefs.h"
#include <rom_handler/session_directory.hpp>
#include <rom_handler/session.hpp>

namespace dart
{
  Algo_Manager::Algo_Manager()
    : reactor_ ()
    , notifier_ (0, this, ACE_Event_Handler::WRITE_MASK)
    , md_src_ (0)
    , order_dest_ (0)
    , order_source_ (0)
    , tag_map_ () { }

  Algo_Manager::~Algo_Manager() { }

  int Algo_Manager::open(void*) {
    this->reactor(ACE_Reactor::instance());
    this->notifier_.reactor (this->reactor ());
    this->msg_queue ()->notification_strategy (&this->notifier_);
    return 0;
  }

  int Algo_Manager::put (ACE_Message_Block *mb, ACE_Time_Value * tv) {
    return this->putq (mb, tv);
  }

  int Algo_Manager::handle_output(ACE_HANDLE) {
    ACE_Message_Block *mb = 0;
    this->getq (mb);
    ACE_ASSERT (mb);

    std::string rawmsg (mb->rd_ptr (), mb->length ());
    CRomCsv msg (rawmsg);
    const std::string command (msg.Get (RFIX::CSV_COMMAND));
    if (command.empty ())
    {
      DART_WARNING ("No command specified: {|%.*C|}\n",
        mb->length (),
        mb->rd_ptr ());
      return 0;
    }

    switch (command.at (0)) {
    case ROMVALUE::COMMAND_ORDER:
      {
        // Validate the order while in this thread.  Under extreme
        // circumstances a large number of invalid orders would cause
        // thrashing if we constantly spawned a new thread, did the
        // validation on that thread, and then rejected the order.
        // The order validation happens in the constructor.

        try {
          // Register the order ID (by rom tag) with the source it came from
          ACE_Message_Block *mb_iterator=mb;
          while
            (mb_iterator
             && mb_iterator->msg_type()!=MB_SESSION_SOURCE_NAME
             && mb_iterator->cont())
             mb_iterator=mb_iterator->cont();

          if (mb_iterator) {
            const char *romtag=msg.Get(RFIX::CSV_STAFFORD_TAG);
            if (romtag && mb_iterator && mb_iterator->length()) {
              const char *srcname=mb_iterator->rd_ptr();
              if (srcname && srcname[0])
                this->tag2src_[romtag]=srcname;
            }
            else
              assert(false);
          }

          dart::Decision_Engine *decision_engine=
            dart::algo::create_decision_engine(msg);
          if (decision_engine==0) {
            this->reject_order (msg,"Unsupported algo type");
            return 0;
          }

          const std::string symbol (msg.GetAt (RFIX::CSV_SYMBOL));
          if (symbol.empty ()) {
            DART_WARNING ("No symbol in order");
            this->reject_order (msg, "no symbol in order");
            return 0;
          }

#if 0
          if (this->md_src_->subscribe_symbol (symbol.c_str ()) == -1) {
            DART_WARNING ("Unable to get market data for %s",symbol.c_str());
            this->reject_order (msg,"Market data subscription error");
            return 0;
          }
#endif

          if (decision_engine->open() == -1) {
            const char *errmsg =
              "Unable to activate decision engine for order";
            DART_WARNING ("%s\n",errmsg);
            this->reject_order (msg, errmsg);
            return 0;
          }

          if (ORDER_BOOK::instance ()->register_callback
                (msg.GetAt (RFIX::CSV_STAFFORD_TAG), decision_engine) == -1) {
            const char *errmsg = "Unable to register callback";
            DART_WARNING ("%s for order %s\n", errmsg,
              msg.GetAt(RFIX::CSV_STAFFORD_TAG).c_str());
            this->reject_order(msg, errmsg);
            return 0;
          }

          this->accept_order (msg);
        }
        catch (const std::exception &e) {
          this->reject_order (msg, e.what ());
        }

        break;
      }

    case ROMVALUE::COMMAND_STATUS:
      {
        // Find the ACE_Task_Base to handle this message.  It is assumed that
        // all status messages are coming from the exchange destinations and
        // will therefore be looked up by the Tag, not the ROM Tag.
        const std::string tag (msg.GetAt (RFIX::CSV_TAG));
        ACE_Task_Base *ptask = ORDER_BOOK::instance ()->find_callback (tag);
        if (ptask) {
          if (ptask->put (mb) == -1)
            DART_WARNING (
              "Unable to queue status message for processing: %.*C\n",
              mb->length() ? mb->length()-1 : 0,
              mb->rd_ptr());
        }
        else
          DART_WARNING ("Unable to find record of order %s\n", tag.c_str());
      }
      break;

    case ROMVALUE::COMMAND_CANCEL:
      try {
        const std::string rom_tag (msg.GetAt (RFIX::CSV_STAFFORD_TAG));
        if (!rom_tag.empty ()) {
          ACE_Task_Base *ptask = ORDER_BOOK::instance ()->find_callback (rom_tag);
          if (ptask) {
            if (ptask->put (mb) == -1)
              throw std::runtime_error ("unable to queue cancel for processing");
          }
          else {
            DART_WARNING ("Cannot find handler for cancel message, tag=%s\n",
              rom_tag.c_str());
          }
        }
        else
          throw std::runtime_error ("No ROM tag found in cancel");
      }
      catch (const std::exception &e) {
        ACE_DEBUG ((LM_DEBUG,
          DART_LOG ("Error '%s' while processing cancel for order %s\n"),
          e.what (),
          msg.GetAt (RFIX::CSV_STAFFORD_TAG).c_str ()));

        this->reject_order (msg, e.what ());
      }
      break;

    default:
      ACE_DEBUG ((LM_DEBUG,
        ACE_TEXT (DART_LOG ("Unexpected message type: %s\n")),
        ACE_TEXT (command.c_str ())));
      break;
    }
    return 0;
  }

   void Algo_Manager::set_market_data_source(Market_Data_Source *psource) {
     this->md_src_ = psource;
   }

   Market_Data_Source *Algo_Manager::get_market_data_source() {
     return this->md_src_;
   }

   void Algo_Manager::outbound_connection(Session *pconn) {
     this->order_dest_ = pconn;
   }

   Session *Algo_Manager::outbound_connection() {
     return this->order_dest_;
   }

   void Algo_Manager::inbound_connection(ROM_Connection *pconn) {
     this->order_source_ = pconn;
   }

   ROM_Connection *Algo_Manager::inbound_connection() {
     return this->order_source_;
   }

   void Algo_Manager::send_csv_to_client(CRomCsv &order) {
     // Needed to find which session to send order on
     const std::string romtag(order.GetAt(RFIX::CSV_STAFFORD_TAG));
     if (!romtag.empty())
     {
       const std::string src(this->tag2src_[romtag]);
       if (!src.empty())
       {
         const std::string &msg_contents (order.Message ());
         ACE_Message_Block *pmessage = new ACE_Message_Block
           (msg_contents.size () + 1);
         pmessage->copy (msg_contents.c_str (), msg_contents.size ());
         Session *session =
           SESSION_DIRECTORY::instance()->lookup_server_session(src);

         if (session)
         {
           if (!session->send(pmessage))
             ACE_DEBUG((LM_DEBUG,
                        "Sending on %s session failed!\n",
                        src.c_str()));
         }
         else
           ACE_DEBUG((LM_DEBUG,
                      "Could not find session for order %s!\n",
                      romtag.c_str()));
       }
     }
     else
       ACE_DEBUG((LM_DEBUG,
                  "Cannot find source for order %s\n",
                  romtag.c_str()));
   }

   void Algo_Manager::reject_order(
     const CRomCsv &order,
     const char *reject_message
     ) {
     CRomCsv reject (order);
     reject.SetAt (RFIX::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
     reject.SetAt (RFIX::CSV_STATUS, ROMVALUE::STATUS_ORDERREJECTED);
     reject.SetAt (RFIX::CSV_STAFFORD_TAG, order.GetAt (RFIX::CSV_STAFFORD_TAG));
     if (reject_message == 0 || reject_message [0] == '\0') {
       reject_message = "Unknown reason";
     }
     reject.SetAt (RFIX::CSV_TEXT, reject_message);

     this->send_csv_to_client (reject);
   }

   void Algo_Manager::accept_order(const CRomCsv &order) {
     CRomCsv accept (order);
     accept.SetAt (RFIX::CSV_COMMAND, ROMVALUE::COMMAND_STATUS);
     accept.SetAt (RFIX::CSV_STATUS, ROMVALUE::STATUS_OPEN);
     this->send_csv_to_client (accept);
   }
}
