#include <ace/log_msg.h>

#include "Wombat_Market_Data_Source.hpp"

using namespace dart;
using namespace Wombat;

#include "Transport_Callback.hpp"
#include "Dictionary_Callback.hpp"
#include <Bullseye/Bullseye_Common.hpp>
#include <cassert>

Wombat_Market_Data_Conduit::Wombat_Market_Data_Conduit ()
: bridge_ (0),
  transport_ (0),
  queue_ (0)
{
  try {
    // TODO: Will have to make this configurable for
    // production to allow LBM connections as well.
    bridge_ = Mama::loadBridge ("wmw");

    // VERY important this is done AFTER Mama::loadBridge()
    // but before any other initialization steps!
    Mama::open ();

    this->transport_ = new MamaTransport ();

    // TODO: Will have to make this configurable for
    // production to allow different configuration names
    this->transport_->create ("ronin", bridge_);

    transport_->setTransportCallback (new Transport_Callback ());
    this->queue_ = Mama::getDefaultEventQueue (bridge_);
    if (queue_ == 0) {
      throw std::runtime_error ("cannot get default MAMA queue");
    }

    Mama::startBackground (this->bridge_, this);
  }
  catch (const MamaStatus &e) {
    ACE_ERROR ((LM_ERROR,
                ACE_TEXT (e.toString ())));
  }
}

int
Wombat_Market_Data_Conduit::subscribe_symbol (const char *symbol)
{
  Symbols::const_iterator iter (this->symbol_subscriptions_.find (symbol));

  // Only subscribe if we are not already getting data
  if (iter == this->symbol_subscriptions_.end ()) {
    try {
      MamaSubscription *psubscription = new MamaSubscription ();
      psubscription->setSubscriptionType (MAMA_SUBSC_TYPE_NORMAL);
      psubscription->setServiceLevel (MAMA_SERVICE_LEVEL_REAL_TIME, 0);

      // TODO: This subscription is either for CTA or NASDAQ; which one it is will
      // have to be determined by another lookup to the TPOS database to determine
      // the feed it's on.  For now, we'll artificially pick one or the other.
      psubscription->create (this->transport_,
                             this->queue_,
                             this,
                             "NASDAQ",
                             symbol);

      this->subscriptions_.insert (psubscription);
      this->symbol_subscriptions_.insert (std::string (symbol));
    }
    catch (const MamaStatus &mama_error) {
      ACE_ERROR ((LM_ERROR,
        ACE_TEXT (mama_error.toString ())));
      return -1;
    }
  }

  return 0;
}

int
Wombat_Market_Data_Conduit::unsubscribe_symbol (const char*)
{
  return -1;
}

Price_Block*
Wombat_Market_Data_Conduit::get_price_block (const char *symbol) const
{
  return this->get_price_block (std::string (symbol));
}

Price_Block*
Wombat_Market_Data_Conduit::get_price_block (const std::string &symbol) const
{
  assert (!symbol.empty ());
  ACE_Guard <ACE_Thread_Mutex> g (this->mutex_);
  Watch_Map::const_iterator i (sym_watches_.find (symbol));
  if (i == sym_watches_.end ()) {
    return 0;
  }

  Price_Block *pnewblock = new Price_Block (i->second);
  return pnewblock;
}

void
Wombat_Market_Data_Conduit::onStartComplete (MamaStatus status)
{
  ACE_DEBUG ((LM_DEBUG,
              ACE_TEXT (DART_LOG ("%s received status %s")),
              __FUNCTION__,
              status.toString ()));
}

void
Wombat_Market_Data_Conduit::onCreate (Wombat::MamaSubscription *psubscription)
{
  ACE_DEBUG ((LM_DEBUG,
              DART_LOG ("Successfully created subscription for %s\n"),
              psubscription ? psubscription->getSymbol () : "nullptr"));
}

void
Wombat_Market_Data_Conduit::onError (MamaSubscription *psubscription,
                             const MamaStatus &status,
                             const char *msg)
{
  ACE_DEBUG ((LM_DEBUG,
              DART_LOG ("Error on subscription for %s with status %s: %s"),
              psubscription ? psubscription->getSymbol () : "nullptr",
              status.toString (),
              msg ? msg : "nullptr"));
}

void
Wombat_Market_Data_Conduit::onMsg (MamaSubscription *psubscription, MamaMsg &msg)
{
  if (msg.getType () != MAMA_MSG_TYPE_INITIAL &&
      msg.getType () != MAMA_MSG_TYPE_QUOTE)
    return;

  ACE_Guard <ACE_Thread_Mutex> guard (this->mutex_);

  const char *psym = 0;
  bool foundsym = msg.tryString (0, 305 /*wIssueSymbol*/, psym);
  if (!foundsym) {
    psym = psubscription->getSymbol ();
    if (psym == 0 || !isalpha (psym [0])) {
      const char *pstr_try_again = 0;
      msg.tryString (0, 305, pstr_try_again);
    }
  }

  Price_Block *pblock = 0;
  if (foundsym) {
    Watch_Map::iterator price_block_iter = this->sym_watches_.find (psym);
    if (price_block_iter != this->sym_watches_.end ()) {
      pblock = & (price_block_iter->second);
    }
    else {
      // Add priceb lock for the first time
      Watch_Map_Insert_Result result =
        this->sym_watches_.insert (std::make_pair (std::string (psym),
        Price_Block ()));
      if (result.second) {
        Watch_Map::iterator iter (result.first);
        pblock = & (iter->second);
      }
    }

    if (pblock) {
      switch (msg.getType ()) {
        case MAMA_MSG_TYPE_INITIAL:
          processInitialUpdate (msg, pblock);
        case MAMA_MSG_TYPE_QUOTE:
          updatePriceBlock (msg, pblock);
          break;
        case MAMA_MSG_TYPE_TRADE:
          break;
        default:
          break;
      }
    }
  }
}

void
Wombat_Market_Data_Conduit::processInitialUpdate (const MamaMsg & /*msg*/,
                                                  Price_Block * /*pblock*/)
{
#if 0
  pblock->initialized_ = true;
  msg.tryI64 (0, 324 /*wLotSize*/, pblock->lot_size);
#endif
}

void
Wombat_Market_Data_Conduit::updatePriceBlock (const MamaMsg & /*msg*/,
                                              Price_Block * /*pblock*/)
{
#if 0
  msg.tryF64 (0, 109 /*ask price*/, pblock->ask_price);
  msg.tryF64 (0, 237 /*bid price*/, pblock->bid_price);
  msg.tryI64 (0, 110 /*ask size */, pblock->ask_size);
  msg.tryI64 (0, 238 /*bid size */, pblock->bid_size );
  const char *pbuffer = 0;
  if (msg.tryString (0, 108 /*wAskPartId*/, pbuffer)) {
    pblock->ask_part_id = pbuffer;
  }
  if (msg.tryString (0, 236 /*wBidPartId*/, pbuffer)) {
    pblock->bid_part_id = pbuffer;
  }

  // Sanity check: did we get this quote before?
  mama_i32_t current_seq_num (-1);
  if (msg.tryI32 (0, 441 /*wQuoteSeqNum*/, current_seq_num)) {
    if (current_seq_num == pblock->quote_seq_no_) {
      cerr << "Got this quote already! pblock->Quote ID = " << current_seq_num << endl;
      cerr << *pblock << endl;
    }
    pblock->quote_seq_no_ = current_seq_num;
  }
#endif
}

void
Wombat_Market_Data_Conduit::onQuality (MamaSubscription*,
                                       mamaQuality quality,
                                       const char *symbol,
                                       short,
                                       const void*)
{
  ACE_DEBUG ((LM_DEBUG,
              DART_LOG ("Quality update on %s: %s"),
              symbol ? symbol : "nullptr",
              mamaQuality_convertToString (quality)));
}
