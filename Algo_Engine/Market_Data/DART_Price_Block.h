#ifndef __DART_PRICE_BLOCK_H__
#define __DART_PRICE_BLOCK_H__

#ifdef __cplusplus
extern "C" {
#endif

/**
 * The Price_Block structure is a generic internal
 * representation of the current state of a symbol.
 */
typedef struct tagPrice_Block
{
  double bid_price;
  double ask_price;
  __int64 bid_size;
  __int64 ask_size;
  __int64 lot_size;
  int quote_seq_no;
  int initialized;
  char bid_participant_id [8];
  char ask_participant_id [8];
} Price_Block;

#ifdef __cplusplus
}
#endif

#endif
