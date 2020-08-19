#ifndef __RASHMSG_H__
#define __RASHMSG_H__

struct rash_order_related_header {
    char timestamp[8];
    char type;
    char order_token[14];
};

struct rash_system_event_message {
    char timestamp[8];
    char type;
    char event_code;
};

/* The RASH protocol provides two different enter order messages. One
 * is a superset of the other and adds ISO-eligibility and a crossing
 * session indicator. Since this provides a superset of the other
 * message, adding only two additional fields that can be defaulted,
 * we are only going to support the more expanded of the two. */
struct rash_EnterOrder {
    char type;
    char order_token[14];
    char side;
    char shares[6];
    char symbol[8];
    char price[10];
    char time_in_force[5];
    char firm[4];
    char display;
    char min_qty[6];
    char max_floor[6];
    char peg_type;
    char peg_difference_sign;
    char peg_difference[10];
    char discretion_price[10];
    char discretion_peg_type;
    char discretion_peg_difference_sign;
    char discretion_peg_difference[10];
    char capacity;
    char random_reserve[6];
    char route_dest[4];
    char cust[32];
    char inter_market_sweep_eligibility;
    char cross_type;
};

struct rash_CancelOrder {
    char type;
    char orderToken[14];
    char shares[6];
};

struct rash_ExtOrdAccept {
    char timestamp[8];
    char type;
    char orderToken[14];
    char side;
    char shares[6];
    char symbol[8];
    char price[10];
    char timeInForce[5];
    char firm[4];
    char display;
    char orderReferenceNumber[9];
    char minQty[6];
    char maxFloor[6];
    char pegType;
    char pegDifferenceSign;
    char pegDifference[10];
    char discretionPrice[10];
    char discretionPegType;
    char discretionPegDifferenceSign;
    char discretionPegDifference[10];
    char capacity;
    char randomReserve[6];
    char routeDest[4];
    char cust[32];
    char interMarketSweepEligibility;
    char crossType;
};

struct rash_OrdAccept {
    char timestamp[8];
    char type;
    char order_token[14];
    char side;
    char shares[6];
    char symbol[8];
    char price[10];
    char time_in_force[5];
    char firm[4];
    char display;
    char order_reference_number[9];
    char min_qty[6];
    char max_floor[6];
    char peg_type;
    char peg_difference_sign;
    char peg_difference[10];
    char discretion_price[10];
    char discretion_peg_type;
    char discretion_peg_difference_sign;
    char discretion_peg_difference[10];
    char capacity;
    char random_reserve[6];
    char route_dest[4];
    char pass_thru[32];
};

struct rash_OrderIsCanceled {
    char timeStamp[8];
    char type;
    char orderToken[14];
    char shares[6];
    char reason;
};

struct rash_OrderExecution {
    char timeStamp[8];
    char type;
    char orderToken[14];
    char execShares[6];
    char execPrice[10];
    char liquidityCode;
    char matchNumber[9];
};

struct rash_OrderIsRejected {
    char timeStamp[8];
    char type;
    char orderToken[14];
    char reason;
};

struct rash_BrokenTrade {
    char timeStamp[8];
    char messageType;
    char orderToken[14];
    char matchNumber[9];
    char reason;
};

#endif
