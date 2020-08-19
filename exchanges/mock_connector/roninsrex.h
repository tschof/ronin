#ifndef RONIN_SREX_INC
#define RONIN_SREX_INC

typedef struct SREX_QUOTE {
    char quoteEntryID[16];      // FIX tag 299
    char symbol[32];            // FIX tag 55
    char securityType[4];       // FIX tag 167 (FUT, OPT, GOVT, etc)
    char maturityMonthYear[6];  // FIX tag 200 (YYYYMM, e.g. 201012)
    char maturityDay[2];        // FIX tag 205
    double strike;              // FIX tag 202
    int putOrCall;              // FIX tag 201
    int securityID;             // FIX tag 48

    int revisionType;

    double bidPrice;            // FIX tag 132
    double askPrice;            // FIX tag 133
    size_t bidSize;             // FIX tag 134
    size_t askSize;             // FIX tag 135

    char securityDesc[63];      // FIX tag 107
    char sessionID[4];          // FIX tag 336  e.g. 'CME'

} srex_quote_t;

typedef struct SREX_CME_DATA {
    char cust_or_firm;          // FIX tag 204 : 0=cust, 1=firm
    char mm_protect_reset;      // FIX tag 9773 : 'N' or 'Y'
    char cti_code;              // FIX tag 9702 : always '2'... for now
    char mm_account[32];        // FIX tag 9771
    char quoteID[32];           // FIX tag 117
} srex_cme_data_t;


#endif
