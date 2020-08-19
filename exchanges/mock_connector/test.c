#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "client_manager.h"
#include "fix_handler.h"
#include "dart_orderbook.h"
#include "dart_constants.h"
#include "roninsrex.h"

char *target_comp;

static const int ONLY_BID_QUOTE_SIDE_CHANGED = 1;
static const int ONLY_ASK_QUOTE_SIDE_CHANGED = 2;
static srex_quote_t quotes[100];
static srex_cme_data_t cme = { 0 };

static char fix_buffer[16384];


#define MAXLATENCY	1000000
static const size_t freq = MAXLATENCY;
static size_t fix_tran[MAXLATENCY] = { 0 };
static size_t fix_send[MAXLATENCY] = { 0 };
static size_t fix_totl[MAXLATENCY] = { 0 };


static void sig_handler(int);
static void submit_mass_quotes(const srex_quote_t *, size_t,
                               const srex_cme_data_t *);
static size_t timedif(const struct timeval *, const struct timeval *);
static void record_latency(size_t *, size_t);
static void calc_percentiles(const char *const, const size_t *);
static void add_dubl_field(unsigned short, char **, double);
static void add_sint_field(unsigned short, char **, int);
static void add_uint_field(unsigned short, char **, size_t u);
static void add_char_field(unsigned short, char **, const char *);

/*
	NOTE: we use the attribute of external visibility so that we can use -fwhole-program
	optimizations in our Makefile
*/

__attribute__ ((externally_visible))
extern void init_latency_test(void)
{
    signal(SIGINT, sig_handler);

    memset(quotes, 0, sizeof(quotes));
    int i;

    for (i = 0; i < 100; ++i) {
        srex_quote_t *q = &quotes[i];

        sprintf(q->quoteEntryID, "QID_%04i", i);
        strcpy(q->symbol, "ES");
        strncpy(q->securityType, "OPT", 3);
        strncpy(q->maturityMonthYear, "201005", 6);
        strncpy(q->maturityDay, "21", 2);
        q->strike = 830.;
        q->putOrCall = 0;
        q->securityID = ((long) q % 16) ? 1 : 0;
        q->revisionType = 3;
        q->bidPrice = 31;
        q->askPrice = 30;
        q->askSize = 1;
        q->bidSize = 1;
        strcpy(q->securityDesc, "ESK0 P0830");
        strncpy(q->sessionID, "CME", 3);
    }

    cme.cust_or_firm = 1;       // FIX tag 204 : 0=cust, 1=firm
    cme.mm_protect_reset = 1;   // FIX tag 9773 : 'N' or 'Y'
    cme.cti_code = 2;           // FIX tag 9702 : always '2'... for now
    strcpy(cme.mm_account, "MM_ACCOUNT");       // FIX tag 9771
    sprintf(cme.quoteID, "QU_%d", rand());      // FIX tag 117
}

__attribute__ ((externally_visible))
extern useconds_t run_latency_test(void)
{
    struct timeval start, stop;
    sprintf(cme.quoteID, "QU_%d", rand());      // FIX tag 117

    gettimeofday(&start, 0);
    submit_mass_quotes(quotes, 10, &cme);
    gettimeofday(&stop, 0);

    record_latency(fix_totl, timedif(&start, &stop));
    return (useconds_t) 100000; // 0.1 second pause to keep TCP window from closing
}

static void calc_percentiles(const char *const tag, const size_t * latency)
{
    // index at which a given percentile is reached
    size_t min = 0;
    size_t max = 0;
    size_t i99 = 0;
    size_t i90 = 0;
    size_t i75 = 0;
    size_t i66 = 0;
    size_t i50 = 0;             // aka the 'average'
    size_t i25 = 0;
    size_t num_events = 0;      // num of latency events
    size_t num_buckets = 0;     // num non-zero buckets w/ events
    size_t i;
    for (i = 0; i < MAXLATENCY; ++i) {
        if (!latency[i])
            continue;
        ++num_buckets;
        num_events += latency[i];
        max = i;
        if (!min) {
            min = i25 = i50 = i66 = i75 = i90 = i99 = i;
        }
    }

    if (!num_buckets) {
        fprintf(stdout, "%s : no metrices to report\n", tag);
        return;
    }
    // percentiles
    const size_t p99 = 99 * num_events;
    const size_t p90 = 90 * num_events;
    const size_t p75 = 75 * num_events;
    const size_t p66 = 66 * num_events;
    const size_t p50 = 50 * num_events;
    const size_t p25 = 25 * num_events;

    size_t temp = 0;
    for (i = 0; i < MAXLATENCY; ++i) {
        if (!latency[i])
            continue;

        temp += latency[i];
        const size_t _sum = 100 * temp;
        if (_sum <= p99) {
            i99 = i;
            if (_sum <= p90) {
                i90 = i;
                if (_sum <= p75) {
                    i75 = i;
                    if (_sum <= p66) {
                        i66 = i;
                        if (_sum <= p50) {
                            i50 = i;
                            if (_sum <= p25) {
                                i25 = i;
                            }
                        }
                    }
                }
            }
        }
    }
    // weighted mean
    double wmean = 0.;
    {
        size_t weight = 0;
        size_t sum = 0;
        for (i = 1; i < MAXLATENCY; ++i) {
            if (!latency[i])
                continue;
            sum += (i * latency[i]);
            weight += latency[i];
        }
        if (weight) {
            wmean = (double) sum / (double) weight;
        }
    }

    // standard deviation
    double sdev = 0.;
    {
        double sum = 0.;
        for (i = 0; i < MAXLATENCY; ++i) {
            if (!latency[i])
                continue;
            double temp = (double) i - wmean;
            sum += (temp * temp);
        }
        sum /= num_events;
        sdev = sqrt(sum);
    }

    static const char *fmt =
        "%s : [%u](min/25%%/50%%/66%%/75%%/90%%/99%%/max/sdev) : (% 3u||% 3u||% 3u||% 3u||% 3u||% 3u||% 3u||% 3u||% 4.2f) uSec\n";

    fprintf(stdout, fmt, tag, num_events, min, i25, i50, i66, i75, i90,
            i99, max, sdev);
}

static size_t timedif(const struct timeval *start,
                      const struct timeval *stop)
{
    const size_t end = (1000000 * stop->tv_sec) + stop->tv_usec;
    const size_t beg = (1000000 * start->tv_sec) + start->tv_usec;
    return (end - beg);
}

static void record_latency(size_t * latency, size_t value)
{
    if (value < MAXLATENCY) {
        latency[value]++;
    } else {
        latency[MAXLATENCY - 1]++;
    }
}

static void add_dubl_field(unsigned short id, char **buf, double d)
{
    int rc = sprintf(*buf, "%u=%f\001", id, d);
    if (rc > 0) {
        *buf += rc;
    }
}

static void add_sint_field(unsigned short id, char **buf, int i)
{
    int rc = sprintf(*buf, "%u=%d\001", id, i);
    if (rc > 0) {
        *buf += rc;
    }
}

static void add_uint_field(unsigned short id, char **buf, size_t u)
{
    int rc = sprintf(*buf, "%u=%lu\001", id, u);
    if (rc > 0) {
        *buf += rc;
    }
}

static void add_char_field(unsigned short id, char **buf, const char *text)
{
    int rc = sprintf(*buf, "%u=%s\001", id, text);
    if (rc > 0) {
        *buf += rc;
    }
}

void
submit_mass_quotes(const srex_quote_t * q, size_t count,
                   const srex_cme_data_t * cme)
{
    struct timeval start, stop;

    const srex_quote_t *end = &q[count];
    char *pbuf = fix_buffer;
    struct message m;

    if (!q || !count || !cme) {
        return;
    }
    fix_buffer[0] = 0;

    gettimeofday(&start, 0);

    add_sint_field(296, &pbuf, 1);      /* NoQuoteSets */
    add_sint_field(302, &pbuf, 1);      /* QuoteSetID  */
    add_char_field(311, &pbuf, q[0].symbol);    /* UnderlyingSymbol */
    add_uint_field(304, &pbuf, count);  /* TotQuoteEntries */
    add_uint_field(295, &pbuf, count);  /* NoQuoteEntries */
    for (; q != end; ++q) {
        add_char_field(299, &pbuf, q->quoteEntryID);    /* QuoteEntryID */
        add_char_field(55, &pbuf, q->symbol);   /* Symbol */
        add_char_field(167, &pbuf, q->securityType);    /* SecurityType */

        if (q->securityID) {
            add_sint_field(48, &pbuf, q->securityID);   /* SecurityID */
        } else {
            if (q->maturityMonthYear[0]) {
                add_char_field(200, &pbuf, q->maturityMonthYear);       /* MaturityMonthYear */
            }
            add_sint_field(201, &pbuf, q->putOrCall);   /* PutorCall */
            add_dubl_field(202, &pbuf, q->strike);      /* StrikePrice */
            if (q->maturityDay[0]) {
                add_char_field(205, &pbuf, q->maturityDay);     /* MaturityDay */
            }
        }

        if (q->securityDesc[0]) {
            add_char_field(107, &pbuf, q->securityDesc);        /* SecurityDesc */
        }
        if (q->revisionType != ONLY_ASK_QUOTE_SIDE_CHANGED) {
            add_dubl_field(132, &pbuf, q->bidPrice);    /* BidPx */
            add_uint_field(134, &pbuf, q->bidSize);     /* BidSize */
        }
        if (q->revisionType != ONLY_BID_QUOTE_SIDE_CHANGED) {
            add_dubl_field(133, &pbuf, q->askPrice);    /* OfferPx */
            add_uint_field(135, &pbuf, q->askSize);     /* OfferSize */
        }
        add_char_field(336, &pbuf, q->sessionID);       /* TradingSessionID */
    }
    add_sint_field(204, &pbuf, cme->cust_or_firm);      /* CustOrFirm */
    add_sint_field(9773, &pbuf, cme->mm_protect_reset);
    add_sint_field(9702, &pbuf, cme->cti_code);
    add_char_field(9771, &pbuf, cme->mm_account);
    add_char_field(117, &pbuf, cme->quoteID);   /* QuoteID */

    m.name = target_comp;
    m.name_len = strlen(m.name);
    m.len = pbuf - fix_buffer;
    m.data = fix_buffer;

    gettimeofday(&stop, 0);
    record_latency(fix_tran, timedif(&start, &stop));

    gettimeofday(&start, 0);
    if (send_fix_message(&m, "i", sizeof(m))) {
        printf("oops! %s(%d)\n", __FILE__, __LINE__);
    }
    gettimeofday(&stop, 0);
    record_latency(fix_send, timedif(&start, &stop));
}

/* the Ctrl-C signal handler */
void sig_handler(int sig_num)
{
    fprintf(stdout, "<Ctrl-C>\n\n");
    calc_percentiles("FIX send ", fix_send);
    calc_percentiles("FIX xform", fix_tran);
    calc_percentiles("FIX total", fix_totl);
    fflush(stdout);
    exit(0);
}
