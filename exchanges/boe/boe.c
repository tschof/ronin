#include "server_objects/dart_constants.h"
#include "server_objects/debug_logger.h"
#include "ex_common/ex_common.h"
#include "ex_common/ex_config_helper.h"
#include "ex_db/db_layer.h"
#include "smtp/mailer.h"

#include "BoeInterface.h"
#include "conf_util.h"
#include "transaction_recorder_tools.h"
#include "EnterOrder.h"
#include "ModifyOrder.h"
#include "CancelOrder.h"
#include "DataTypes.h"
#include "ConversionRules.h"

#include <string.h>
#include <errno.h>
#include <ctype.h>
#include <limits.h>
#include <dirent.h>

static char *firmp = 0, *pDefRouteInst = 0;
static size_t firmn = 0, nDefRouteInst = 0;

struct MatchingUnitSeqNo {
    uint8_t matchingUnit;
    uint32_t sequenceNumber;
    char *filename;
    struct MatchingUnitSeqNo *next;
};

static const char INCOMINGPREFIX[] = "matchingunit-";
static const char INCOMINGSUFFIX[] = "_incoming.log";

static int parseNameForMatchingUnitNumber(const char *str)
{
    char *pMatchUnit = strstr(str, INCOMINGPREFIX);
    if (pMatchUnit == 0 || pMatchUnit != str)
        return -1;
    char *pSuffix = strstr(str, INCOMINGSUFFIX);
    if (pSuffix == 0)
        return -1;
    size_t nlen = strlen(str);
    const size_t nonDigitLength =
        (sizeof INCOMINGPREFIX - 1) + (sizeof INCOMINGSUFFIX - 1);
    if (nlen <= nonDigitLength)
        return -1;
    nlen -= nonDigitLength;
    char tmp[nlen + 1];
    char *pDigits = pMatchUnit + (sizeof INCOMINGPREFIX - 1);
    memcpy(tmp, pDigits, nlen);
    tmp[nlen] = '\0';
    errno = 0;
    long int i = strtol(tmp, 0, 10);
    if (errno != 0)
        return -1;
    if (i > SCHAR_MAX)
        return -1;
    return (int) i;
}

static void addToList(struct MatchingUnitSeqNo **head,
                      uint8_t matchingUnit, size_t seqno,
                      const char *filename)
{
    struct MatchingUnitSeqNo *tmp = malloc(sizeof *tmp);
    tmp->matchingUnit = matchingUnit;
    tmp->sequenceNumber = seqno;
    tmp->filename = strdup(filename);
    tmp->next = 0;
    if (*head == 0) {
        *head = tmp;
    } else {
        struct MatchingUnitSeqNo *current;
        for (current = *head; current->next != 0; current = current->next);
        current->next = tmp;
    }
}

static void freeList(struct MatchingUnitSeqNo *head)
{
    while (head != 0) {
        struct MatchingUnitSeqNo *tmp = head->next;
        free(head->filename);
        free(head);
        head = tmp;
    }
}

static size_t countListNodes(struct MatchingUnitSeqNo *head)
{
    size_t n;
    for (n = 0; head != 0; ++n, head = head->next);
    return n;
}

static struct MatchingUnitSeqNo *listMatchingUnits(void)
{
    DIR *dirp = opendir(".");   /* Open the local directory. */
    if (dirp == 0) {
        perror("opening local directory");
        return 0;
    }
    struct dirent *entryp =
        malloc(offsetof(struct dirent, d_name) +
               pathconf(".", _PC_NAME_MAX) + 1);
    struct dirent *result;
    int retval;
    struct MatchingUnitSeqNo *head = 0;
    while ((retval = readdir_r(dirp, entryp, &result)) == 0 && result != 0) {
        int nmatchunit = parseNameForMatchingUnitNumber(result->d_name);
        if (nmatchunit > -1) {
            const char *psuffix = strstr(result->d_name, INCOMINGSUFFIX);
            if (psuffix == 0)
                continue;
            ssize_t seqno =
                retrieve_inbound_seqno_from_file(result->d_name,
                                                 psuffix - result->d_name);
            addToList(&head, (uint8_t) nmatchunit, seqno, result->d_name);
        }
    }
    return head;
}

/* Because of the unusual nature of BATS Order Entry there can be more
 * than one inbound sequence file (one for each matching engine at
 * BATS).  We cannot predict in advance how many or what numbers they
 * will be (they are numbered from 0 to 255). The easiest thing to do
 * is simply delete all inbound sequence files that match a given
 * pattern. */
static void deleteSequenceFiles(void)
{
    struct MatchingUnitSeqNo *sequences = listMatchingUnits(), *pseq;
    for (pseq = sequences; pseq != 0; pseq = pseq->next)
        if (unlink(pseq->filename) == -1)
            perror("deleting sequence file");
    freeList(sequences);
}

static int boe_disconnectedState(struct BoeInterface *rei,
                                 struct BoeHeader *header,
                                 int *drop, struct parse_con *pc);

static const struct BoeHeader headerDefaults = {
    .startOfMessage = 0xBABA,
    .messageLength = 0x0000,
    .messageType = 0x0,
    .matchingUnit = 0x0,
    .sequenceNumber = 0x00000000,
};

struct error_report {
    char *message;
    int free;
};

#define declare_error_report(name) struct error_report name = { 0, 0 }
#define destroy_error_report(name) do { if (name.free != 0) free(name.message); } while (0);

#define BOE_PRICE_INITIALIZER {'0','0','0','0','0','0','0','0','0','0'}
#define BOE_NUMERIC_INITIALIZER {'0','0','0','0','0','0'}

static void sendMessageAndLog(pthread_mutex_t * sendMutex, char *p,
                              size_t n, const char *name,
                              size_t nameLength, const char *modName,
                              size_t modNameLength)
{
    char *end = p + n, *logstart = p;
    int nwritten;
    if (pthread_mutex_lock(sendMutex) == 0) {
        do {
            if ((nwritten =
                 linux_only_dart_send(name, nameLength, p, end - p)) > 0) {
                p += nwritten;
            } 
        } while (p < end);
        pthread_mutex_unlock(sendMutex);
    } else {
        send_debug_message("%s:%d Cannot lock mutex", __FILE__, __LINE__);
    }
    struct message logMsg = { (char *) name, nameLength, logstart, n };
    dart_rolling_log_going(&logMsg, modName, modNameLength);
    record_outbound_message(dup_message(&logMsg));
}

static void dart_order_obj__init(dart_order_obj * o)
{
    set_rom_field(o, 0, "E", 1);
    size_t i;
    for (i = 0; i < ROM_LEN; ++i) {
        o->positions[i].beg = o->positions[i].end =
            o->positions[i].val_len = 0;
        o->positions[i].name = 0;
        o->positions[i].len = 0;
    }
    o->rep = 0;
}

static void boe_databaseInit(struct database_conf *dbconf,
                             const char *destID, orderbook * ob)
{
    int res = 0;
    element_t e, temp;
    queue_t q = create_queue();
    e = NULL;
    temp = NULL;
    res = get_db_objects(dbconf, q, destID);
    e = dequeue(q);
    while (e != NULL) {
        struct dart_ex_result_set *ders = 0;
        dart_order_obj *obj = (dart_order_obj *) get_obj(ob->ord_obj);
        dart_order_obj__init(obj);
        ders = (struct dart_ex_result_set *) e->d;
        if (ders != NULL) {
            set_rom_field(obj, ROM_SHARES, &ders->qty, 4);
            int l_len = strlen(ders->orderTag);
            set_rom_field(obj, ROM_CLORDID, ders->orderTag, l_len);
            no_copy_insert(ob->cl_ord_id_map, ders->orderTag, l_len, obj);
            l_len = strlen(ders->omTag);
            set_rom_field(obj, ROM_TAG, ders->omTag, l_len);
            no_copy_insert(ob->rom_ord_id_map, ders->omTag, l_len, obj);
            size_t rom_inst_len = 0;
            char *rom_inst = build_rom_name(ders->omTag, 32,
                                            &rom_inst_len);
            if (rom_inst != NULL) {
                set_rom_field(obj, ROM_SENDER, rom_inst, rom_inst_len);
                free(rom_inst);
            }
            set_rom_field(obj, ROM_BIN_CUM, &ders->cumQty,
                          sizeof ders->qty);
            set_rom_field(obj, ROM_BIN_QTY, &ders->qty, sizeof ders->qty);

            switch (ders->side) {
            case 1:
                set_rom_field(obj, ROM_SIDE, "1", 1);
                break;
            case 2:
                set_rom_field(obj, ROM_SIDE, "2", 1);
                break;
            case 5:
                set_rom_field(obj, ROM_SIDE, "5", 1);
                break;
            }
            int sym_len = strlen(ders->symbol);
            set_rom_field(obj, ROM_SYM, ders->symbol, sym_len);
            int account_len = strlen(ders->clearingAccount);
            set_rom_field(obj, ROM_CLR_ACC, ders->clearingAccount,
                          account_len);
            double d = ders->price;
            d = d * 10000;
            d += .0000001;
            int setter = (int) d;
            set_rom_field(obj, ROM_BIN_PRICE, &setter, sizeof(int));
            switch (ders->orderType) {
            case 1:
                set_rom_field(obj, ROM_TYPE, "1", 1);
                break;
            case 2:
                set_rom_field(obj, ROM_TYPE, "2", 1);
                break;
            case 3:
                set_rom_field(obj, ROM_TYPE, "3", 1);
                break;
            case 5:
                set_rom_field(obj, ROM_TYPE, "5", 1);
                break;
            case 11:
                set_rom_field(obj, ROM_TYPE, "B", 1);
                break;
            }
        }
        temp = e;
        e = temp->next;
        if (ders != NULL) {
            free(ders);
        }
        temp->d = NULL;
        free(temp);
        temp = NULL;
    }
    destroy_queue(q);
}

/* Returns non-zero if all bytes are sent, zero if not */
static int boe_sendMessage(pthread_mutex_t * mutex, char *p, size_t n,
                           char *name, int name_len)
{
    char *end = p + n;
    int nwritten;
    if (pthread_mutex_lock(mutex) == 0) {
        do {
            if ((nwritten =
                 linux_only_dart_send(name, name_len, p, end - p)) > 0)
                p += nwritten;
        } while (p < end);
        pthread_mutex_unlock(mutex);
    } else {
        send_debug_message("%s:%d Cannot lock mutex", __FILE__, __LINE__);
    }
    return p == end;
}

static const char log_prefix[] = "Order ID ";

#define resetErrorReport(er) do { er->free=0; er->message = 0; } while(0)
#define reportErrno(er) do { er->message = strerror(errno); er->free = 0; } while(0);

static int setPegOrder(struct EnterOrder *enterOrder,
                       struct dart_order_obj *csvmsg,
                       struct error_report *err, char pegtype)
{
    int n;
    const char *p;

    if (setOptionalAlphaNumeric
        (&enterOrder->optionalFields, EnterOrderOrdType, "P", 1, 1) == -1)
        return -1;

    if (setOptionalAlphaNumeric
        (&enterOrder->optionalFields, EnterOrderExecInst, &pegtype, 1,
         1) == -1) {
        return -1;
    }

    /* Peg orders are implicitly hidden, so no need to set the Display
     * Indicator field.  However we will reject orders that have any
     * other Display Instructions set. */
    p = get_rom_field(csvmsg, ROM_DISPLAY_INSTRUCTIONS, &n);
    if (n > 0 && (n != 1 || p[0] != 'I')) {
        err->message = "Peg orders must be hidden";
        return -1;
    }

    /* Pegged orders allow an offset */
    p = get_rom_field(csvmsg, ROM_PEGPRICE_OFFSET, &n);
    if (n > 0
        && setOptionalPrice(&enterOrder->optionalFields,
                            EnterOrderPegDifference, p, n) == -1) {
        return -1;
    } else {
        int64_t *offsetp =
            enterOrder->optionalFields.iovp[EnterOrderPegDifference].
            iov_base;
        printf("Peg offset %ld\n", *offsetp);
    }

    return 0;
}

static int isPegOrder(struct dart_order_obj *csvmsg)
{
    int n;
    const char *pexecinst = get_rom_field(csvmsg, ROM_EXEC_INST, &n);
    if (n == 1) {
        switch (pexecinst[0]) {
        case 'L':
        case 'M':
        case 'P':
        case 'R':
            return pexecinst[0];
        }
    }
    return 0;
}

static int setTimeInForce(struct EnterOrder *enterOrder,
                          struct dart_order_obj *csvmsg,
                          struct error_report *err)
{
    int n;
    const char *p;
    p = get_rom_field(csvmsg, ROM_TYPE, &n);
    if ((n == 1 && p[0] == '5') || (n == 2 && p[0] == '1' && p[1] == '1')) {
        return setOptionalAlphaNumeric(&enterOrder->optionalFields,
                                       EnterOrderTimeInForce, "7", 1, 1);
    }

    p = get_rom_field(csvmsg, ROM_TIF, &n);
    if (n == 0) {
        err->message = "Must specify time in force";
        return -1;
    }
    if (n == 1) {
        switch (p[0]) {
        case '0':              /* DAY */
        case '1':              /* GTC */
        case '2':              /* OPG */
        case '3':              /* IOC */
        case '5':              /* GTX */
            return setOptionalAlphaNumeric(&enterOrder->optionalFields,
                                           EnterOrderTimeInForce, p, n, 1);
        }
    }
    err->message = "Invalid time in force";
    return -1;
}

/* Socket is disconnected, no session. */
static const uintptr_t DISCONNECTED = 0;

/* Made a connection attempt.  No response received yet. */
static const uintptr_t SOCKET_CONNECTION_ATTEMPTED = 1;

/* Received notification that socket is connected.  Sent a
 * LoginRequest.  Received a LoginAccepted. */
static const uintptr_t SESSION_UP = 2;

static uintptr_t sessionState = 0;

static int cancelOrder(struct TradingInterface *ei,
                       struct message *message,
                       struct parse_con *connparser, void *token,
                       struct error_report *err,
                       struct parse_con *connection)
{
    resetErrorReport(err);
    struct dart_order_obj *csvmsg = token;
    /* Get the ClOrdID */
    int n;
    char *p;
    p = get_rom_field(csvmsg, ROM_TAG, &n);
    struct ob *book = ei->book;
    struct dart_order_obj *orderobj = 0;
    void *booklookup = 0;
    pthread_mutex_lock(&book->mutex);
    int lookupResult =
        get_map_entry(book->rom_ord_id_map, p, n, &booklookup);
    pthread_mutex_unlock(&book->mutex);
    if (lookupResult == 0) {
        err->message = "Cannot find order";
        return -1;
    }
    orderobj = booklookup;
    p = get_rom_field(orderobj, ROM_CLORDID, &n);
    struct CancelOrder cancelOrder;
    if (initCancelOrder(&cancelOrder, p, n) == -1) {
        err->message = "Problem with ClOrdID";
        return -1;
    }
    /* Log in the rolling log */
    boe_sendMessage(&book->send_mutex, (void *) &cancelOrder,
                    sizeof cancelOrder, book->fix_con_name,
                    book->fix_con_len);
    struct message logmsg = {
        .name = book->fix_con_name,
        .name_len = book->fix_con_len,
        .data = (char *) &cancelOrder,
        .len = sizeof cancelOrder,
    };
    dart_rolling_log_going(&logmsg, book->module_name,
                           book->module_name_len);
    return 0;
}

static int sendIovec(struct TradingInterface *ei, struct iovec *sendiovp,
                     size_t sendiovn)
{
    /* The idea here is to write the total iovec, retrying at most 32
     * times, only for the case of benign errors indicating the socket
     * is still connected and disconnecting if the error is serious. */
    int fd =
        get_sock_for_con(ei->book->fix_con_name, ei->book->fix_con_len);
    size_t nfailed = 0;
    struct iovec *endiovp = sendiovp + sendiovn;

    /* Acquire a lock on the outgoing connection */
    int threrror = pthread_mutex_lock(&ei->book->send_mutex);
    if (threrror) {
        errno = threrror;
        perror("locking mutex");
        return -1;
    }
    while (sendiovp != endiovp && nfailed < 32000) {
        ssize_t nsent = writev(fd, sendiovp, sendiovn);
        if (nsent > 0) {
            while (nsent > 0) {
                if (nsent >= sendiovp->iov_len) {
                    nsent -= sendiovp->iov_len;
                    ++sendiovp;
                    --sendiovn;
                } else {
                    sendiovp->iov_base += nsent;
                    sendiovp->iov_len -= nsent;
                    nsent = 0;
                }
            }
        } else if (nsent == 0) {
            break;              /* There may be more iovecs in our array, but they
                                 * do not hold any data. Do not bother to iterate
                                 * them as writev has already done that. Just exit
                                 * this send loop. */
        } else {
            switch (errno) {
            case EWOULDBLOCK:
            case ENOBUFS:
            case EINTR:
                ++nfailed;
                break;
            default:
                perror("sending on socket");
                kick_off_connection(ei->book->fix_con_name,
                                    ei->book->fix_con_len);
                sessionState = DISCONNECTED;
                ((struct BoeInterface *) ei)->handleMessage =
                    boe_disconnectedState;
                goto done_sending;
            }
        }
    }

  done_sending:
    threrror = pthread_mutex_unlock(&ei->book->send_mutex);
    if (threrror) {
        errno = threrror;
        perror("unlocking mutex");
        return -1;
    }
    return 0;
}

static int modifyOrder(struct TradingInterface *ei,
                       struct message *message,
                       struct parse_con *connparser, void *token,
                       struct error_report *err,
                       struct parse_con *connection)
{
    resetErrorReport(err);
    struct dart_order_obj *csvmsg = token;

    int n;
    char *p;

    p = get_rom_field(csvmsg, ROM_TAG, &n);
    if (n == 0) {
        err->message = "No order tag found";
        return -1;
    }

    struct ob *book = ei->book;
    struct dart_order_obj *orderobj = 0;
    pthread_mutex_lock(&book->mutex);
    void *booklookup = 0;
    if (get_map_entry(book->rom_ord_id_map, p, n, &booklookup) == 0) {
        err->message = "Cannot find order";
        return -1;
    }
    orderobj = booklookup;
    pthread_mutex_unlock(&book->mutex);

    if (orderobj->rep != 0) {
        err->message = "Replace already pending";
        return -1;
    }

    struct ModifyOrder modifyOrder;
    initModifyOrder(&modifyOrder, ei->book->databufs);

    /* OrigClOrdID */
    p = get_rom_field(orderobj, ROM_CLORDID, &n);
    if (n == 0) {
        err->message = "Cannot find ClOrdID";
        return -1;
    }
    setAlphaNumeric(&modifyOrder.fixedFields.origClOrdID, p, n,
                    sizeof modifyOrder.fixedFields.origClOrdID);


    /* Order Qty */
    p = get_rom_field(csvmsg, ROM_SHARES, &n);
    if (n > 0
        && setOptionalBinary(&modifyOrder.optionalFields, ModifyOrderQty,
                             p, n, 4) == -1)
        return -1;

    /* Limit Price */
    p = get_rom_field(csvmsg, ROM_PRICE, &n);
    if (n > 0
        && setOptionalPrice(&modifyOrder.optionalFields, ModifyOrderPrice,
                            p, n) == -1)
        return -1;

    /* Type (only limit to market allowed) */
    /* Can only change from Limit to Market, so if the replacement is
     * market, set the modify type to market; either the order is
     * being changed from limit to market (which is okay) or from peg
     * to market (which should be rejected) or from market to market
     * (no effect). */
    p = get_rom_field(csvmsg, ROM_TYPE, &n);
    if (n == 1 && p[0] == '1'
        && setOptionalAlphaNumeric(&modifyOrder.optionalFields,
                                   ModifyOrderType, p, n, 1) == -1)
        return -1;

    /* Side (only short sell to long sell and vice versa) */
    p = get_rom_field(csvmsg, ROM_SIDE, &n);
    if (n == 1 && (p[0] == '2' || p[0] == '5')
        && setOptionalAlphaNumeric(&modifyOrder.optionalFields,
                                   ModifyOrderSide, p, n, 1) == -1)
        return -1;

    struct dstring id;
    dstring_attach(&id, modifyOrder.fixedFields.clOrdID,
                   sizeof modifyOrder.fixedFields.clOrdID - 1, 0);
    int nclordid = ei->id_generator->generate_id(ei->id_generator, &id);
    cond_error_report(nclordid != -1, err, "Internal error generating ID");

    /* Need to store the new ClOrdID in our order ID map so we can
     * find it on the BATS response. */
    pthread_mutex_lock(&ei->book->mutex);
    no_copy_insert(ei->book->cl_ord_id_map,
                   modifyOrder.fixedFields.clOrdID, nclordid, orderobj);
    pthread_mutex_unlock(&ei->book->mutex);

    orderobj->rep = csvmsg;

    prepModifyOrderForSending(&modifyOrder, 0);

    dart_rolling_log_going_iovec(modifyOrder.iov, narray(modifyOrder.iov),
                                 ei->book->fix_con_name,
                                 ei->book->fix_con_len,
                                 ei->book->module_name,
                                 ei->book->module_name_len);

    if (sendIovec(ei, modifyOrder.iov, narray(modifyOrder.iov)) == -1) {
        fprintf(stderr, "Error sending\n");
        return -1;
    }

    destroyModifyOrder(&modifyOrder);
    return 0;
}

struct ConversionRule *acctRule = 0;

static int placeOrder(struct TradingInterface *ei,
                      struct message *message,
                      struct parse_con *connparser, void *token,
                      struct error_report *err,
                      struct parse_con *connection)
{
    int retval = -1;
    resetErrorReport(err);
    err->message = "Internal error";
    struct dart_order_obj *csvmsg = token;
    set_rom_field(csvmsg, ROM_SENDER, message->name, message->name_len);

    struct EnterOrder enterOrder;
    initEnterOrder(&enterOrder, ei->book->databufs);

    int n;
    const char *p;

    /* Set some fixed fields */
    p = get_rom_field(csvmsg, ROM_SIDE, &n);
    if (setSide(&enterOrder, p, n) == -1) {
        err->message = "Invalid side";
        goto cleanup;
    }

    p = get_rom_field(csvmsg, ROM_SHARES, &n);
    if (setBinaryNumFromStr(&enterOrder.fixedFields.orderQty, p, n,
                            sizeof enterOrder.fixedFields.orderQty) ==
        -1) {
        err->message = "Invalid order qty";
        goto cleanup;
    }
    set_rom_field(csvmsg, ROM_BIN_QTY, &enterOrder.fixedFields.orderQty,
                  sizeof enterOrder.fixedFields.orderQty);
    uint32_t cumQty = 0;
    set_rom_field(csvmsg, ROM_BIN_CUM, &cumQty, sizeof cumQty);

    /* Map the symbol, looking for a symbol suffix, if present. */
    p = get_rom_field(csvmsg, ROM_SYM, &n);
    if (n == 0) {
        err->message = "No symbol";
        goto cleanup;
    }
    const char *pslash = memchr(p, '/', n);
    if (pslash == 0) {
        setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                EnterOrderSymbol, p, n, 8);
    } else {
        const size_t symlen = pslash - p;
        ++pslash;
        const size_t suffixlen = (p + n) - pslash;
        setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                EnterOrderSymbol, p, symlen, 8);
        setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                EnterOrderSymbolSfx, pslash, suffixlen, 8);
    }

    if (setTimeInForce(&enterOrder, csvmsg, err) == -1) {
        goto cleanup;
    }

    /* Routing instructions, this handling mimics that from the BATS
     * FIX exchange interface. */
    p = get_rom_field(csvmsg, ROM_INSTR, &n);
    if (n > 0) {
        if (n == 1 && p[0] == 'A') {
            /* Add liquidity only */
            setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                    EnterOrderRoutingInst, pDefRouteInst,
                                    nDefRouteInst, 4);
            setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                    EnterOrderDisplayIndicator, "V", 1, 1);
        } else if (n == 1 && p[0] == 'I') {
            setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                    EnterOrderRoutingInst, pDefRouteInst,
                                    nDefRouteInst, 4);
            setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                    EnterOrderDisplayIndicator, "I", 1, 1);
        } else {
            setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                    EnterOrderRoutingInst, p, n, 4);
        }
    } else if (pDefRouteInst != 0) {
        setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                EnterOrderRoutingInst, pDefRouteInst,
                                nDefRouteInst, 4);
    }

    /* Handle iceberg orders */
    p = get_rom_field(csvmsg, ROM_MAX_FLOOR, &n);
    if (n > 0) {
        if (p[0] == '0') {
            /* Take this as a hidden order as well */
            setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                    EnterOrderDisplayIndicator, "I", 1, 1);
        } else {
            setOptionalBinary(&enterOrder.optionalFields,
                              EnterOrderMaxFloor, p, n, 4);
        }
    }

    p = get_rom_field(csvmsg, ROM_MIN_QTY, &n);
    if (n > 0) {
        setOptionalBinary(&enterOrder.optionalFields, EnterOrderMinQty,
                          p, n, 4);
    }

    /* Set the clearing firm */
    p = get_rom_field(csvmsg, ROM_CLR_ID, &n);
    if (setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                EnterOrderClearingFirm, firmp, firmn,
                                4) == -1) {
        goto cleanup;
    }

    /* Send the clearing account */
    struct dstring acct;
    if (acctRule->convert(acctRule, csvmsg, &acct) == -1) {
        goto cleanup;
    }
    if (setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                EnterOrderClearingAcct,
                                acct.p, acct.nlen, 4) == -1) {
        goto cleanup;
    }

    p = get_rom_field(csvmsg, ROM_CLR_ACC, &n);
    if (n == 0) {
        goto cleanup;
    }
    if (setOptionalAlphaNumeric
        (&enterOrder.optionalFields, EnterOrderAccount, p, n, 16) == -1) {
        goto cleanup;
    }

    p = get_rom_field(csvmsg, ROM_CAP, &n);
    if (setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                EnterOrderCapacity, p, n, 1) == -1) {
        goto cleanup;
    }

    int pegtype;
    if ((pegtype = isPegOrder(csvmsg)) != 0) {
        if (setPegOrder(&enterOrder, csvmsg, err, pegtype) == -1) {
            goto cleanup;
        }
    } else {
        /* Check if market or limit */
        p = get_rom_field(csvmsg, ROM_TYPE, &n);
        cond_error_report(n == 1, err, "Invalid order type");
        cond_error_report(p[0] == '1'
                          || p[0] == '2', err, "Invalid order type");
        if (setOptionalAlphaNumeric
            (&enterOrder.optionalFields, EnterOrderOrdType, p, 1,
             1) == -1) {
            goto cleanup;
        }
        if (p[0] == '2') {
            p = get_rom_field(csvmsg, ROM_PRICE, &n);
            if (n == 0) {
                err->message = "Price needed on limit order";
                goto cleanup;
            }
            if (setOptionalPrice
                (&enterOrder.optionalFields, EnterOrderPrice, p, n) == -1)
                goto cleanup;
        }

        /* Map ExecInst */
        p = get_rom_field(csvmsg, ROM_EXEC_INST, &n);
        if (n == 0) {
            /* Default ExecInst to 'w' (Do Not DRT). */
            setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                    EnterOrderExecInst, "w", 1, 1);
        } else if (n > 1) {
            /* Only single character ExecInst are allowed. */
            err->message = "Invalid ExecInst";
            goto cleanup;
        } else if (p[0] == 'f') {
            /* Reject, Intermarket Sweep Orders are never allowed in
             * our system. */
            err->message = "ISO orders not allowed";
            goto cleanup;
        } else {
            setOptionalAlphaNumeric(&enterOrder.optionalFields,
                                    EnterOrderExecInst, p, n, 1);
        }
    }

    /* Set an order ID */
    struct dstring id;
    dstring_attach(&id, enterOrder.fixedFields.clOrdID,
                   sizeof enterOrder.fixedFields.clOrdID - 1, 0);
    int nclordid = ei->id_generator->generate_id(ei->id_generator, &id);
    cond_error_report(nclordid != -1, err, "Internal error generating ID");
    set_rom_field(csvmsg, ROM_CLORDID, id.p, nclordid);

    /* We are going to send this, store it in the order book. */
    pthread_mutex_lock(&ei->book->mutex);
    no_copy_insert(ei->book->cl_ord_id_map, enterOrder.fixedFields.clOrdID,
                   strnlen(enterOrder.fixedFields.clOrdID,
                           sizeof enterOrder.fixedFields.clOrdID), csvmsg);
    p = get_rom_field(csvmsg, ROM_TAG, &n);
    no_copy_insert(ei->book->rom_ord_id_map, p, n, csvmsg);
    pthread_mutex_unlock(&ei->book->mutex);

    prepEnterOrderForSending(&enterOrder, 0x00000000);

    dart_rolling_log_going_iovec(enterOrder.iov, narray(enterOrder.iov),
                                 ei->book->fix_con_name,
                                 ei->book->fix_con_len,
                                 ei->book->module_name,
                                 ei->book->module_name_len);

    /* Warning: sendIovec potentially modifies the iovec as it sends
     * it, but in this particular case we do not care because we do
     * not need the iovec intact anymore because we are not doing any
     * outbound order sequencing. */
    sendIovec(ei, enterOrder.iov, narray(enterOrder.iov));

    err->message = 0;
    retval = 0;

  cleanup:
    destroyEnterOrder(&enterOrder);
    return retval;
}

static void record_incoming(struct message *message)
{
    struct incoming_message *incoming_message =
        malloc(sizeof *incoming_message);
    incoming_message->name =
        memdup_nullterminate(message->name, message->name_len);
    incoming_message->name_len = message->name_len;
    incoming_message->num = 1;
    record_inbound_message(incoming_message);
}

static void onDartCsvMessage(void *csvwrapper, struct message *csvmsg,
                             void *b)
{
    struct parse_con *connection = b;
    struct ob *orderbook = connection->book;
    int convert_result = -1;
    struct message outgoing_message = {
        .name = orderbook->fix_con_name,
        .name_len = orderbook->fix_con_len,
        .data = 0,
        .len = 0,
    };
    record_incoming(csvmsg);
    dart_rolling_log_coming(csvmsg, orderbook->module_name,
                            orderbook->module_name_len);
    struct databuf *msgbuf = 0;
    declare_error_report(err);
    if (orderbook->can_send_orders == 0) {
        err.message = "Not connected to exchange";
        goto reject;
    }
    switch (csvmsg->data[0]) {
    case 'E':
        set_rom_field(csvwrapper, ROM_SENDER, csvmsg->name,
                      csvmsg->name_len);
        convert_result = placeOrder(orderbook->ei, csvmsg,
                                    connection, csvwrapper,
                                    &err, connection);
        break;
    case 'C':
        convert_result = cancelOrder(orderbook->ei, csvmsg,
                                     connection, csvwrapper,
                                     &err, connection);
        break;
    case 'R':
        convert_result = modifyOrder(orderbook->ei, csvmsg,
                                     connection, csvwrapper,
                                     &err, connection);
        break;
    }
    if (convert_result != -1) {
        return;
    }

  reject:
    msgbuf = get_obj(orderbook->databufs);
    if (create_rom_reject(csvmsg->data, msgbuf, err.message) > 0) {
        outgoing_message.name = csvmsg->name;
        outgoing_message.name_len = csvmsg->name_len;
        sendMessageAndLog(&orderbook->send_mutex, msgbuf->rd_ptr,
                          databuf_unread(msgbuf), csvmsg->name,
                          csvmsg->name_len, orderbook->module_name,
                          orderbook->module_name_len);
    } else {
        send_debug_message("%s:%d Error creating ROM reject", __FILE__,
                           __LINE__);
    }
    destroy_error_report(err);
    destroy_elem(orderbook->databufs, msgbuf);
}

static struct init_args *loadConfig(const char *filename, int file_len)
{
    struct init_args *in_args = malloc(sizeof(struct init_args));
    in_args->dc = initialize_config(filename, file_len);
    char *reset_hour, *reset_minute, *incoming, *outgoing, *last_run_date;
    char *accountp;
    struct conf_map mappings[] = {
        {"BOE", "account_name", &(in_args->name), &(in_args->name_len)},
        {0, 0, &(in_args->username), &(in_args->un_len)},
        {"BOE", "password", &(in_args->password), &(in_args->p_len)},
        {0, 0, &(in_args->target_comp), &(in_args->tc_len)},
        {0, 0, &(in_args->sender_comp), &(in_args->sc_len)},
        {0, 0, &(in_args->target_sub), &(in_args->ts_len)},
        {"BOE", "SessionSubID", &(in_args->sender_sub),
         &(in_args->ss_len)},
        {0, 0, &(in_args->fix_version), &(in_args->fv_len)},
        {"BOE", "Reset Incoming", &incoming, 0},
        {"BOE", "Reset Outgoing", &outgoing, 0},
        {"TIMES", "Reset Hour", &reset_hour, 0},
        {"TIMES", "Reset Min", &reset_minute, 0},
        {"TIMES", "Last Run Day", &last_run_date, 0},
        {"BOE", "Firm", &firmp, &firmn},
        {"BOE", "DefRouteInst", &pDefRouteInst, &nDefRouteInst},
        {"BOE", "Account", &accountp, 0}
    };
    conf_map_to_vars(in_args->dc, mappings,
                     sizeof mappings / sizeof mappings[0]);
    struct seq_reset_time *reset_time = malloc(sizeof *reset_time);
    reset_time->reset_hour = reset_hour != 0 ? atoi(reset_hour) : 0;
    free(reset_hour);
    reset_time->reset_min = reset_minute != 0 ? atoi(reset_minute) : 0;
    free(reset_minute);
    reset_time->new_incoming = incoming != 0 ? atoi(incoming) : 0;
    free(incoming);
    reset_time->new_outgoing = outgoing != 0 ? atoi(outgoing) : 0;

    char current_run_date[9] = { '\0' };
    struct tm local;
    int retval =
        make_current_date_string(current_run_date, sizeof current_run_date,
                                 &local);
    reset_time->create_new = 1;
    if (retval != -1 && retval <= sizeof current_run_date
        && last_run_date != 0
        && strncmp(current_run_date, last_run_date, 8) == 0) {
        reset_time->create_new =
            should_reset(&local, reset_time->reset_hour,
                         reset_time->reset_min, 5);
    }

    if (reset_time->create_new)
        deleteSequenceFiles();

    in_args->reset_time = reset_time;
    set_val_for_tag(in_args->dc, "TIMES", 5, "Last Run Day", 12,
                    current_run_date, 8);
    free(last_run_date);

    /* Handle the account mappings */
    if (accountp != 0) {
        struct ConversionRuleParser parser = conversionRuleParserDefaults;
        parser.linep = parser.p = accountp;
        newRule(&parser);
        if (beginParse(&parser) == -1) {
            fprintf(stderr, "Account config parsing failed\n");
            exit(EXIT_FAILURE);
        }
        acctRule = parser.rules;
    }
    free(accountp);

    return in_args;
}

#define on_entry printf("%s\n", __FUNCTION__)

static int TradingInterface_is_connected(void)
{
    /* TODO: Implement */
    return 0;
}

static void create_databuffs(queue_t glob, queue_t q, int num)
{
    int i;
    for (i = 0; i < num; ++i) {
        databuf_t *buff = databuf_alloc(500);
        enqueue(q, buff);
    }
}

void destroy_db(void *d)
{
    databuf_t *buff = (databuf_t *) d;
    databuf_destroy(buff);
}

static orderbook *boe_book_init(struct TradingInterface *ei)
{
    char *(*generate_unique_token) (size_t *) = 0;
    orderbook *book = no_order_mng_book(generate_unique_token,
                                        TradingInterface_is_connected, 0,
                                        0, 0,
                                        ei->exchange_conf->
                                        order_allocation,
                                        ei->exchange_conf->mod_name,
                                        ei->exchange_conf->mod_name_len,
                                        ei->session_conf->name,
                                        ei->session_conf->name_len,
                                        0, 0);
    book->ei = ei;
    book->ord_obj =
        create_dart_obj_fact(ei->exchange_conf->order_allocation,
                             destroy_doo, create_dart_obj_func, clean_doo);

    int initialamt = 1000;
#ifdef OBJ_FACT_LEAK_CHECK
    initialamt = 0;
#endif
    book->databufs =
        create_dart_obj_fact(initialamt, destroy_db, create_databuffs,
                             databuf_clear);

    /* Outgoing are not sequenced so there's no point in logging
     * them. */
    ei->session_conf->log_outgoing = 0;

    /* TODO: Check on the effect of this flag. Seems to be for the ROM
     * handler resend functionality to restrict resending only
     * cancels. Since we shouldn't be sending cancels to the ROM and
     * since we want all status messages to be reported back to ROM, I
     * am assuming this should be false. */
    ei->session_conf->cancel_only = 0;

    book->in_args = copy_init_args(ei->session_conf);
    /* TODO: DB initialization here. */
    return book;
}

static int boe_configure(struct TradingInterface *ei, const char *filename)
{
    on_entry;
    int retval = 0;
    ei->exchange_conf = get_config(filename, strlen(filename), "BOE", 3);
    if (ei->exchange_conf != 0
        && (ei->session_conf = loadConfig(filename, strlen(filename)))
        && (ei->rom_conf = create_queue()) != 0) {
        if (firmp == 0) {
            fprintf(stderr, "Must specify Firm in config\n");
            return -1;
        } else if (firmn != 4) {
            fprintf(stderr, "Firms must 4 char MPID\n");
            return -1;
        }
        load_rom_config(ei->exchange_conf->dc, ei->rom_conf);
        /* Configure BOE specific stuff */
        struct BoeInterface *boe = (struct BoeInterface *) ei;
        ei->book = boe_book_init(ei);
        ei->id_generator = make_dart_standard_id_generator();

        /* Load orders from the database. */
        boe_databaseInit(&ei->exchange_conf->db,
                         ei->exchange_conf->dest_id, ei->book);

        /* Create a timestamp converter */
        if ((boe->tsconverter = create_boe_timestamp_converter()) == 0)
            retval = -1;

        /* TODO: Allow overrides in the log file */
        if ((boe->logFileFilter =
             makeFileFilter("MatchingUnit[0-9]+_outgoing.log")) == 0) {
            perror("creating file filter");
            retval = -1;
        }

        boe->base.stopTime =
            TradingInterface_getStopTime((struct TradingInterface *) boe);
    }
    return retval;
}

static int boe_run(struct TradingInterface *exch)
{
    printf("%s\n", __FUNCTION__);
    int retval = -1;
    time_t now, stop = exch->stopTime;
    for (time(&now); now < stop; time(&now)) {
        if (exch->is_connected(exch) != 0) {
            sleep(1);
            if (sessionState == SESSION_UP)
                exch->send_heartbeat(exch);
        } else {
            ((struct BoeInterface *) exch)->handleMessage =
                boe_disconnectedState;
            sessionState = SOCKET_CONNECTION_ATTEMPTED;
            exch->connect(exch);
            if (exch->is_connected(exch) == 0)
                sleep(30);      /* Reconnect interval of 30 seconds. */
        }
    }
    return retval;
}

static const struct boe_ClientHeartbeat clientHeartbeat = {
    .startOfMessage = {0xBA, 0xBA},
    .messageLength = 8,
    .messageType = 0x03,
    .matchingUnit = 0x00,
    .sequenceNumber = 0x00000000,
};

static int boe_sendHeartbeat(struct TradingInterface *ei)
{
    return boe_sendMessage(&ei->book->send_mutex,
                           (char *) &clientHeartbeat,
                           sizeof clientHeartbeat, ei->book->fix_con_name,
                           ei->book->fix_con_len);
}

int boe_connect_flag = 0;

pthread_mutex_t boe_connect_mutex = PTHREAD_MUTEX_INITIALIZER;

static void boe_set_connection_flag(int connect_flag)
{
    printf("%s\n", __FUNCTION__);
    int retval;
    if ((retval = pthread_mutex_lock(&boe_connect_mutex)) == 0) {
        boe_connect_flag = connect_flag;
        if ((retval = pthread_mutex_unlock(&boe_connect_mutex)) != 0) {
            errno = retval;
            perror("pthread_mutex_unlock");
        }
    } else {
        errno = retval;
        perror("pthread_mutex_lock");
    }
}

static void boe_on_connection_update(con_obj * connection,
                                     int is_connected, char *log, int len)
{
    printf("%s(%p, %d, %p, %d)\n", __FUNCTION__, connection, is_connected,
           log, len);
    if (is_connected == 0) {
        sessionState = DISCONNECTED;

    }
    boe_set_connection_flag(is_connected);
}

static int boe_is_connected(struct TradingInterface *ei)
{
    if (sessionState == DISCONNECTED)
        return 0;
    else
        return 1;
}

static int destadaptor_getOrderObjectByID(orderbook * book,
                                          const char *client_order_id,
                                          size_t length,
                                          dart_order_obj ** order_object)
{
    int retval = -1;
    if (pthread_mutex_lock(&book->mutex) == 0) {
        retval =
            get_map_entry(book->cl_ord_id_map, client_order_id, length,
                          (void **) order_object);
        pthread_mutex_unlock(&book->mutex);

        /* The return codes for get_map_entry are the number of bytes
         * of the payload, which should be 8 on 64-bit systems, the
         * same as a pointer size, but is hard-coded to 4 to match the
         * 32-bit Windows we used to run on. So, a return of 0 is bad,
         * 4 is good, so we turn that into the more common return
         * codes of -1 and 0. */
        if (retval == 0)
            retval = -1;
        else if (retval > 0)
            retval = 0;
    }
    return retval;
}

struct StatusFields;

typedef void (*cleanup_callback) (struct StatusFields *);

struct StatusFields {
    rdonlystr_t sender;
    rdonlystr_t dartOrderID;
    rdonlystr_t status;
    rdonlystr_t clientOrderID;
    rdonlystr_t destOrderID;
    rdonlystr_t timestamp;
    rdonlystr_t execPrice;
    rdonlystr_t execShares;
    rdonlystr_t execID;
    rdonlystr_t liqCode;
    rdonlystr_t desc;
    cleanup_callback cb;
    void *privateData1;
    void *privateData2;
};

static struct databuf *createCsvStatusMessage(struct StatusFields *sf,
                                              struct ob *book)
{
    struct databuf *msg = get_obj(book->databufs);
    databuf_memcpy(msg, "S,,,,,,,,,,,,,,", 15);
    databuf_memcpy(msg, sf->status.p, sf->status.n);    /* 14 */
    databuf_memcpy(msg, ",", 1);
    databuf_memcpy(msg, sf->clientOrderID.p, sf->clientOrderID.n);      /* 15 */
    databuf_memcpy(msg, ",,", 2);
    databuf_memcpy(msg, sf->dartOrderID.p, sf->dartOrderID.n);  /* 17 */
    databuf_memcpy(msg, ",,,,,,,,,", 9);
    databuf_memcpy(msg, sf->desc.p, sf->desc.n);
    databuf_memcpy(msg, ",,", 2);
    databuf_memcpy(msg, sf->execID.p, sf->execID.n);
    databuf_memcpy(msg, ",,,,,,,,,,,,,,", 14);
    databuf_memcpy(msg, sf->execShares.p, sf->execShares.n);
    databuf_memcpy(msg, ",,", 2);
    databuf_memcpy(msg, sf->execPrice.p, sf->execPrice.n);
    databuf_memcpy(msg, ",,,,,,", 6);
    databuf_memcpy(msg, sf->timestamp.p, sf->timestamp.n);
    databuf_memcpy(msg, ",,,,,,,,,,,,,,,,,,,", 19);
    databuf_memcpy(msg, sf->liqCode.p, sf->liqCode.n);
    databuf_memcpy(msg, ",,,,,,,,,,,\n", 12);
    return msg;
}

static void sendStatusMessageViaEmail(const struct StatusFields *sf)
{
    struct databuf *email = databuf_alloc(512);
    if (sf->timestamp.n > 0)
        databuf_write(email, "Time    %.*s\n", (int) sf->timestamp.n,
                      sf->timestamp.p);
    if (sf->dartOrderID.n > 0)
        databuf_write(email, "ROM Tag %.*s\n", (int) sf->dartOrderID.n,
                      sf->dartOrderID.p);
    if (sf->status.n > 0)
        databuf_write(email, "Status  %.*s\n", (int) sf->status.n,
                      sf->status.p);
    if (sf->clientOrderID.n > 0)
        databuf_write(email, "ClOrdID %.*s\n", (int) sf->clientOrderID.n,
                      sf->clientOrderID.p);
    if (sf->execPrice.n > 0)
        databuf_write(email, "Price   %.*s\n", (int) sf->execPrice.n,
                      sf->execPrice.p);
    if (sf->execShares.n > 0)
        databuf_write(email, "Shares  %.*s\n", (int) sf->execShares.n,
                      sf->execShares.p);
    if (sf->execID.n > 0)
        databuf_write(email, "ExecID  %.*s\n", (int) sf->execID.n,
                      sf->execID.p);
    if (sf->desc.n > 0)
        databuf_write(email, "%.*s\n", (int) sf->desc.n, sf->desc.p);
    char *title = "Bust or Correction from BATS BOE";
    send_biz_message(email->rd_ptr, databuf_unread(email), title,
                     strlen(title));
    databuf_destroy(email);
}

static const char incomingLogFilePrefix[] = "matchingunit-";

static int boe_connectedState(struct BoeInterface *boe,
                              struct BoeHeader *header, int *drop,
                              struct parse_con *pc)
{
    int sequenceOrder = 1, retval = 0;

    /* Filter all but message types we know how to handle. */
    switch (header->messageType) {
    case 0x0A:
        break;
    case 0x0B:
        sequenceOrder = 0;
        break;
    case 0x0C:
        break;
    case 0x0E:
        sequenceOrder = 0;
        break;
    case 0x0F:
        break;
    case 0x10:
        sequenceOrder = 0;
        break;
    case 0x11:
        break;
    case 0x12:
        break;
    default:
        printf("Unknown message type: %02x\n", header->messageType);
        return -1;
    }

    /* All messages we know how to process from BATS have a common
     * header. I call this the Response Header. */
    struct ResponseHeader *response = (struct ResponseHeader *) header;

    /* Find the order on our book */
    struct dart_order_obj *orderobj;
    size_t clordidlen =
        strnlen(response->clOrdID, sizeof response->clOrdID);
    if (destadaptor_getOrderObjectByID
        (boe->base.book, response->clOrdID, clordidlen, &orderobj) == -1) {
        printf("Cannot find ClOrdID %.*s on order book\n",
               (int) strnlen(response->clOrdID, sizeof response->clOrdID),
               response->clOrdID);
        goto done;
    }

    /* There is a limited set of fields that we report on Status
     * messages. These fields are encapsulated in a Status Fields
     * object. */
    struct StatusFields sf;
    memset(&sf, 0, sizeof sf);

    struct ob *book = boe->base.book;

    int n;
    char *p;

    p = get_rom_field(orderobj, ROM_SENDER, &n);
    if (n == 0) {
        fprintf(stderr, "No sender in order object for %.*s\n",
                (int) clordidlen, response->clOrdID);
        retval = -1;
        goto done;
    }

    sf.sender.p = p;
    sf.sender.n = n;

    p = get_rom_field(orderobj, ROM_TAG, &n);
    if (n == 0) {
        retval = -1;
        goto done;
    }

    sf.dartOrderID.p = p;
    sf.dartOrderID.n = n;

    sf.clientOrderID.p = response->clOrdID;
    sf.clientOrderID.n = clordidlen;

    char ts[TIMESTAMP_LEN_WITH_MICROS];
    memset(ts, 0, sizeof ts);
    if (binaryDateTimeToStr(response->transactionTime, ts, sizeof ts) ==
        -1) {
        fprintf(stderr, "Error converting timestamp to string: %zu\n",
                response->transactionTime);
    }

    sf.timestamp.p = ts;
    sf.timestamp.n = sizeof ts;

    /* Based on the semantics of the individual message we may or may
     * not be done with this order. */
    int removeFromBook = 0;

    switch (header->messageType) {
    case 0x0A:
        {
            /* Acknowledgement */
            sf.status.p = "13";
            sf.status.n = 2;
        }
        break;
    case 0x0B:
        {
            /* Reject */
            sf.status.p = "8";
            sf.status.n = 1;
            struct OrderReject *reject = (struct OrderReject *) header;
            sf.desc.p = reject->text;
            sf.desc.n = strnlen(reject->text, sizeof reject->text);
            removeFromBook = 1;
        }
        break;
    case 0x0C:
        {
            /* Order Successfully Modified */
            sf.status.p = "5";
            sf.status.n = 1;
            struct OrderModified *orderModified =
                (struct OrderModified *) header;
            char tmpbuf[128];
            struct databuf dbuf;
            if (databuf_attach(&dbuf, tmpbuf, sizeof tmpbuf) == -1) {
                retval = -1;
                goto done;
            }
            sf.destOrderID.p = dbuf.wr_ptr;
            if (databuf_write(&dbuf, "%lu", orderModified->orderID) == -1) {
                retval = -1;
                goto done;
            }
            sf.destOrderID.n = dbuf.wr_ptr - sf.destOrderID.p;

            /* Remove previous ClOrdID mapping to this order
             * object. */
            p = get_rom_field(orderobj, ROM_CLORDID, &n);
            if (n > 0) {
                pthread_mutex_lock(&book->mutex);
                remove_from_map(book->cl_ord_id_map, p, n);
                pthread_mutex_unlock(&book->mutex);
            } else {
                fprintf(stderr, "No ClOrdID on this order object\n");
            }

            /* Change the ClOrdID on the order object to the new
             * ClOrdID. */
            set_rom_field(orderobj, ROM_CLORDID,
                          orderModified->responseHeader.clOrdID,
                          strnlen(orderModified->responseHeader.clOrdID,
                                  sizeof orderModified->responseHeader.
                                  clOrdID));

            /* Apply possibly changed fields to original order. */
            struct dart_order_obj *replaced = orderobj->rep;

            if (replaced != 0) {
                /* Order Qty */
                p = get_rom_field(replaced, ROM_SHARES, &n);
                if (n > 0) {
                    set_rom_field(orderobj, ROM_SHARES, p, n);
                    /* Replace the binary qty */
                    uint32_t replaceQty = strtol(p, 0, 10);
                    set_rom_field(orderobj, ROM_BIN_QTY, &replaceQty,
                                  sizeof replaceQty);
                }

                /* Limit Price */
                p = get_rom_field(replaced, ROM_PRICE, &n);
                if (n > 0)
                    set_rom_field(orderobj, ROM_PRICE, p, n);

                /* Ord Type */
                p = get_rom_field(replaced, ROM_TYPE, &n);
                if (n > 0)
                    set_rom_field(orderobj, ROM_TYPE, p, n);

                /* Side */
                p = get_rom_field(replaced, ROM_SIDE, &n);
                if (n > 0)
                    set_rom_field(orderobj, ROM_SIDE, p, n);


                /* Free the replacement order object */
                destroy_elem(book->ord_obj, orderobj->rep);
                orderobj->rep = 0;
            }
        }
        break;
    case 0x0E:
        {
            /* Order Modification Rejected */
            sf.status.p = "20";
            sf.status.n = 2;

            struct OrderModificationRejected *modRejected
                = (struct OrderModificationRejected *) header;
            sf.desc.p = modRejected->text;
            sf.desc.n =
                strnlen(modRejected->text, sizeof modRejected->text);

            /* Remove the mapping from the replacement ClOrdID to the
             * order object. */
            pthread_mutex_lock(&book->mutex);
            remove_from_map(book->cl_ord_id_map,
                            modRejected->responseHeader.clOrdID,
                            strnlen(modRejected->responseHeader.clOrdID,
                                    sizeof modRejected->responseHeader.
                                    clOrdID));
            pthread_mutex_unlock(&book->mutex);

            /* Free the replacement order object */
            destroy_elem(book->ord_obj, orderobj->rep);
            orderobj->rep = 0;
        }
        break;
    case 0x0F:
        {
            /* Cancelled */
            sf.status.p = "4";
            sf.status.n = 1;
            removeFromBook = 1;
        }
        break;
    case 0x10:
        {
            /* Cancel rejected */
            sf.status.p = "14";
            sf.status.n = 2;
            struct CancelReject *cancelReject =
                (struct CancelReject *) header;
            sf.desc.p = cancelReject->text;
            sf.desc.n =
                strnlen(cancelReject->text, sizeof cancelReject->text);
        }
        break;
    case 0x11:
        {
            /* Executed */
            sf.status.p = "1";
            sf.status.n = 1;
            struct OrderExecution *orderExecution =
                (struct OrderExecution *) header;
            p = get_rom_field(orderobj, ROM_BIN_CUM, &n);
            if (n == 0 || n != sizeof(uint32_t)) {
                retval = -1;
                goto done;
            }
            uint32_t *cumqtyp = (uint32_t *) p;
            *cumqtyp += orderExecution->lastShares;

            p = get_rom_field(orderobj, ROM_BIN_QTY, &n);
            if (n == 0 || n != sizeof(uint32_t)) {
                retval = -1;
                goto done;
            }
            uint32_t *qtyp = (uint32_t *) p;
            if (*cumqtyp >= *qtyp) {
                sf.status.p = "2";
                sf.status.n = 1;
                removeFromBook = 1;
            }

            char tmpbuf[128];
            struct databuf dbuf;
            if (databuf_attach(&dbuf, tmpbuf, sizeof tmpbuf) == -1) {
                retval = -1;
                goto done;
            }

            sf.execShares.p = dbuf.wr_ptr;
            if (databuf_write(&dbuf, "%u", orderExecution->lastShares) ==
                -1) {
                retval = -1;
                goto done;
            }
            sf.execShares.n = dbuf.wr_ptr - sf.execShares.p;

            sf.execPrice.p = dbuf.wr_ptr;
            if (binaryPriceToString(orderExecution->lastPx, &dbuf) == -1) {
                retval = -1;
                goto done;
            }
            sf.execPrice.n = dbuf.wr_ptr - sf.execPrice.p;

            sf.execID.p = dbuf.wr_ptr;
            if (databuf_write(&dbuf, "%ld", (long) orderExecution->execID)
                == -1) {
                retval = -1;
                goto done;
            }
            sf.execID.n = dbuf.wr_ptr - sf.execID.p;

            sf.liqCode.p = dbuf.wr_ptr;
            if (databuf_write
                (&dbuf, "%c",
                 orderExecution->baseLiquidityIndicator) == -1) {
                retval = -1;
                goto done;
            }
            if (isprint(orderExecution->subLiquidityIndicator) != 0 &&
                databuf_write(&dbuf, "%c",
                              orderExecution->subLiquidityIndicator) ==
                -1) {
                retval = -1;
                goto done;
            }
            sf.liqCode.n = dbuf.wr_ptr - sf.liqCode.p;
        }
        break;
    case 0x12:
        {
            /* Trade bust or correction */
            char tmpbuf[128];
            struct databuf dbuf;
            if (databuf_attach(&dbuf, tmpbuf, sizeof tmpbuf) == -1) {
                retval = -1;
                goto done;
            }
            struct OrderBustOrCorrection *bustCorrection
                = (struct OrderBustOrCorrection *) header;
            sf.execID.p = dbuf.wr_ptr;
            databuf_write(&dbuf, "%ld", bustCorrection->execRefID);
            sf.execID.n = dbuf.wr_ptr - sf.execID.p;

            sf.liqCode.p = &bustCorrection->baseLiquidityIndicator;
            sf.liqCode.n = 1;

            sf.execPrice.p = dbuf.wr_ptr;
            binaryPriceToString(bustCorrection->lastPx, &dbuf);
            sf.execPrice.n = dbuf.wr_ptr - sf.execPrice.p;

            sf.execShares.p = dbuf.wr_ptr;
            databuf_write(&dbuf, "%u", bustCorrection->correctedPx);
            sf.execShares.n = dbuf.wr_ptr - sf.execShares.p;
            sendStatusMessageViaEmail(&sf);
        }
        break;
    }

    struct databuf *sendBuf = createCsvStatusMessage(&sf, book);
    sendMessageAndLog(&book->send_mutex, sendBuf->rd_ptr,
                      databuf_unread(sendBuf), sf.sender.p, sf.sender.n,
                      book->module_name, book->module_name_len);
    if (removeFromBook) {
        pthread_mutex_lock(&book->mutex);
        remove_from_map(book->cl_ord_id_map, sf.clientOrderID.p,
                        sf.clientOrderID.n);
        remove_from_map(book->rom_ord_id_map, sf.dartOrderID.p,
                        sf.dartOrderID.n);
        pthread_mutex_unlock(&book->mutex);
        destroy_elem(book->ord_obj, orderobj);
    }
    destroy_elem(book->databufs, sendBuf);

  done:
    if (sequenceOrder != 0) {
        struct incoming_message *recordMessage =
            malloc(sizeof *recordMessage);
        recordMessage->num = 1;
        recordMessage->name =
            calloc(sizeof incomingLogFilePrefix + 2, sizeof(char));
        memcpy(recordMessage->name, incomingLogFilePrefix,
               sizeof incomingLogFilePrefix);
        recordMessage->name[sizeof incomingLogFilePrefix - 1]
            = (header->matchingUnit / 10) + '0';
        recordMessage->name[sizeof incomingLogFilePrefix]
            = (header->matchingUnit % 10) + '0';
        recordMessage->name_len = sizeof incomingLogFilePrefix + 1;
        record_inbound_message(recordMessage);
    }

    return retval;
}

static int processingReplay(struct BoeInterface *boe,
                            struct BoeHeader *header, int *drop,
                            struct parse_con *pc)
{
    int retval = 0;
    if (header->messageType == 0x13) {
        boe->base.book->can_send_orders = 1;
        boe->handleMessage = boe_connectedState;
        fprintf(stderr, "Replay done\n");
    } else {
        fprintf(stderr, "Replay message type = %d from matching unit %d\n",
                (int) header->messageType, (int) header->matchingUnit);
        retval = boe_connectedState(boe, header, drop, pc);
    }
    return retval;
}

static int dumpBinary(const void *const data, size_t n, databuf_t * buf)
{
    const unsigned char *p = data;
    size_t i;
    const char *delimiter = "";
    for (i = 0; i < n; ++i) {
        if (i != 0)
            delimiter = " ";
        if (databuf_write(buf, "%s%02x", delimiter, p[i]) == -1)
            return -1;
    }
    return 0;
}

static int dumpHeader(struct BoeHeader *header, databuf_t * buf)
{
    databuf_write(buf, "StartOfMessage='");
    if (dumpBinary
        (&header->startOfMessage, sizeof header->startOfMessage,
         buf) == -1)
        return -1;
    databuf_write(buf, "';");
    databuf_write(buf, "MessageLength='%u';",
                  (unsigned int) header->messageLength);
    databuf_write(buf, "MessageType='%02x';", header->messageType);
    databuf_write(buf, "MatchingUnit='%u';",
                  (unsigned int) header->matchingUnit);
    if (databuf_write
        (buf, "SequenceNumber='%u';",
         (unsigned int) header->sequenceNumber) == -1)
        return -1;
    return 0;
}

static void boe_dumpLoginResponse(const struct BoeLoginResponse
                                  *loginResponse, databuf_t * buf)
{
    databuf_write(buf, "Login Response Code %c, ", loginResponse->status);
    switch (loginResponse->status) {
    case 'A':
        databuf_write(buf, "Login Accepted");
        break;
    case 'B':
        databuf_write(buf, "Session in use");
        break;
    case 'D':
        databuf_write(buf, "Session is disabled");
        break;
    case 'F':
        databuf_write(buf, "Invalid Return Bitfield in login message");
        break;
    case 'I':
        databuf_write(buf, "Invalid unit given in Login message");
        break;
    case 'M':
        databuf_write(buf, "Invalid Login Request message structure");
        break;
    case 'N':
        databuf_write(buf, "Not authorized (invalid username/password)");
        break;
    case 'Q':
        databuf_write(buf, "Sequence ahead in Login message");
        break;
    case 'S':
        databuf_write(buf, "Invalid session");
        break;
    default:
        databuf_write(buf, "Unknown");
        break;
    }
    if (loginResponse->text[sizeof loginResponse->text - 1] == '\0') {
        databuf_write(buf, ", Message from BATS: %s", loginResponse->text);
    } else {
        databuf_write(buf, ", Message from BATS: %.*s",
                      (int) sizeof loginResponse->text,
                      loginResponse->text);
    }
}

/* The BOE session is currently disconnected. A socket connection has
 * been made and a login request has been sent. */
static int boe_disconnectedState(struct BoeInterface *rei,
                                 struct BoeHeader *header,
                                 int *drop, struct parse_con *pc)
{
    int result = -1;
    databuf_t *logBuf = get_obj(rei->base.book->databufs);
    /* We expect this to be a Login Response. */
    if (header->messageType == 0x07) {
        if (header->messageLength < sizeof(struct BoeLoginResponse)) {
            send_debug_message
                ("Received message length %u less than that "
                 "expected for a LoginResponse (%zu)",
                 (unsigned int) header->messageLength,
                 sizeof(struct BoeLoginResponse));
            *drop = 1;
            return -1;
        }
        struct BoeLoginResponse *loginResponse =
            (struct BoeLoginResponse *) header;
        boe_dumpLoginResponse(loginResponse, logBuf);
        if (loginResponse->status == 'A') {
            rei->handleMessage = processingReplay;
            sessionState = SESSION_UP;
            result = 0;
        } else {
            *drop = 1;
        }
        send_debug_message("Received Login Response from BATS: %s",
                           logBuf->rd_ptr);
    } else {
        /* Unexpected message, just log it */
        dumpHeader(header, logBuf);
        databuf_write(logBuf, "Raw data='");
        const unsigned char *start =
            (const unsigned char *) &header->messageLength;
        const unsigned char *end = start + header->messageLength;
        dumpBinary(start, end - start, logBuf);
        send_debug_message("Unexpected message: %s'", logBuf->rd_ptr,
                           databuf_unread(logBuf));
    }
    destroy_elem(rei->base.book->databufs, logBuf);
    return result;
}

static const size_t getFramingLength(struct BoeHeader *header)
{
    return header->messageLength + sizeof header->startOfMessage;
}

static unsigned long boe_frame(const char *data, unsigned long nbytes,
                               int *cut_connection,
                               struct parse_con *connection_parse_info)
{
    struct BoeInterface *boe = connection_parse_info->book;
    const char *p = data, *end = data + nbytes;
    while ((end - p) >= sizeof(struct BoeHeader)) {
        struct BoeHeader *pHeader = (struct BoeHeader *) p;
        const size_t messageLength = getFramingLength(pHeader);
        if (pHeader->startOfMessage != 0xbaba) {
            /* Our framing has gone horribly wrong, we must bail out. */
            *cut_connection = 1;
            send_debug_message
                ("%s:%d Framing error, StartOfMessage != 0xbaba, "
                 "cutting connection");
            return 0;
        }
        if ((end - p) < messageLength) {
            /* We do not have a complete message, wait for more. */
            return p - data;
        }
        /* Heartbeats are not processed. */
        if (pHeader->messageType != 0x09) {
            struct message logmsg = {
                .name = boe->base.book->fix_con_name,
                .name_len = boe->base.book->fix_con_len,
                .data = (char *) p,
                .len = messageLength,
            };
            dart_rolling_log_coming(&logmsg, boe->base.book->module_name,
                                    boe->base.book->module_name_len);
            boe->handleMessage(boe, pHeader, cut_connection,
                               connection_parse_info);
        }
        p += messageLength;
    }
    return p - data;
}

static void boe_on_message(struct message *message, void *token)
{
    printf("%s\n", __FUNCTION__);
}

static void boe_on_idle(con_obj * connection)
{
    printf("%s\n", __FUNCTION__);
}

static unsigned long boe_parse_sequence(char *p, unsigned long n,
                                        unsigned long j, unsigned long *k)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

static char *boe_reset_pad(unsigned long i, char *p, size_t * n)
{
    printf("%s\n", __FUNCTION__);
    return 0;
}

static const struct BoeLoginRequest loginRequestDefaults = {
    .header = {
               .startOfMessage = 0xBABA,
               .messageLength = 0,
               .messageType = 0x01,
               .matchingUnit = 0x00,
               .sequenceNumber = 0x00000000},
    .sessionSubID = {0, 0, 0, 0},
    .username = {0, 0, 0, 0},
    .password = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    .noUnspecifiedUnitReplace = 0x00,   /* Replay unspecified units */
    .orderAckFields = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .reserved1 = 0x00,
    .orderRejFields = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .reserved2 = 0x00,
    .orderModFields = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .reserved3 = 0x00,
    .orderRestatedFields = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .reserved4 = 0x00,
    .orderModRejFields = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .reserved5 = 0x00,
    .orderCancelledFields = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .reserved6 = 0x00,
    .cancelRejFields = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .reserved7 = 0x00,
    .orderExecFields = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .reserved8 = 0x00,
    .tradeCancelCorrectFields = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
    .reserved9 = {
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
                  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00},
};

static int initLoginRequest(struct BoeLoginRequest *loginRequest,
                            const char *username, size_t nameLength,
                            const char *password, size_t passLength,
                            const char *sessionSubID, size_t subIdLength)
{
    if (nameLength > sizeof loginRequest->username
        || passLength > sizeof passLength
        || subIdLength > sizeof loginRequest->sessionSubID) {
        send_debug_message
            ("Name, Password, or SessionSubID too long for BATS binary login request");
        return -1;
    }
    memcpy(&loginRequest->username, username, nameLength);
    memcpy(&loginRequest->password, password, passLength);
    memcpy(&loginRequest->sessionSubID, sessionSubID, subIdLength);
    return 0;
}

static int waitForConnectionCompleted(con_obj * connection)
{
    int fd = get_sock(connection);
    if (fd == -1)
        return -1;
    fd_set rdset, wrset;
    FD_ZERO(&wrset);
    FD_SET(fd, &wrset);
    rdset = wrset;
    select(fd + 1, &rdset, &wrset, 0, 0);
    if (FD_ISSET(fd, &wrset) || FD_ISSET(fd, &rdset)) {
        int error = 0;
        socklen_t optvarlen = sizeof error;
        if (getsockopt(fd, SOL_SOCKET, SO_ERROR, &error, &optvarlen) == -1)
            return -1;
        if (error != 0) {
            perror("getsockopt for SO_ERROR on socket fd");
            return -1;
        }
    }
    return 0;
}

static int onConnectionEstablished(struct parse_args *pa)
{
    if (waitForConnectionCompleted(pa->con->co) == -1) {
        close(get_sock(pa->con->co));
        return -1;
    }

    struct BoeInterface *boe = pa->con->book;
    struct ob *book = boe->base.book;
    struct databuf *loginBuffer = get_obj(book->databufs);
    pa->con->init_con(pa->con->co);

    boe->base.client_connection.connection = pa->con->co;
    databuf_memcpy(loginBuffer, &loginRequestDefaults,
                   sizeof loginRequestDefaults);
    struct BoeLoginRequest *loginRequest =
        (struct BoeLoginRequest *) loginBuffer->rd_ptr;
    if (initLoginRequest
        ((struct BoeLoginRequest *) loginBuffer->rd_ptr,
         boe->base.session_conf->name, boe->base.session_conf->name_len,
         boe->base.session_conf->password, boe->base.session_conf->p_len,
         boe->base.session_conf->sender_sub,
         boe->base.session_conf->ss_len) == -1) {
        fprintf(stderr, "%s:%d problem initializing login\n", __FILE__,
                __LINE__);
        return -1;
    }

    /* Setup the matching units */
    struct MatchingUnitSeqNo *mu = listMatchingUnits();
    uint8_t numberOfUnits = countListNodes(mu);
    databuf_memcpy(loginBuffer, &numberOfUnits, sizeof numberOfUnits);

    /* Add each matching unit */
    uint8_t i;
    struct MatchingUnitSeqNo *tmp;
    for (i = 0, tmp = mu; i < numberOfUnits && tmp != 0;
         ++i, tmp = tmp->next) {
        databuf_memcpy(loginBuffer, &tmp->matchingUnit,
                       sizeof tmp->matchingUnit);
        databuf_memcpy(loginBuffer, &tmp->sequenceNumber,
                       sizeof tmp->sequenceNumber);
    }
    freeList(mu);

    loginRequest->header.messageLength =
        databuf_unread(loginBuffer) -
        sizeof loginRequest->header.startOfMessage;
    char *boeConnectionName = (char *) get_name(pa->con->co);
    boe_sendMessage(&book->send_mutex, loginBuffer->rd_ptr,
                    databuf_unread(loginBuffer), boeConnectionName,
                    get_name_len(pa->con->co));
    struct message logmsg = {
        .data = loginBuffer->rd_ptr,
        .len = databuf_unread(loginBuffer),
        .name = boeConnectionName,
        .name_len = get_name_len(pa->con->co),
    };
    dart_rolling_log_going(&logmsg, boe->base.book->module_name,
                           boe->base.book->module_name_len);
    free(boeConnectionName);
    destroy_elem(book->databufs, loginBuffer);
    return 0;
}

static struct BoeInterface *BoeInterface_create(void)
{
    printf("%s\n", __FUNCTION__);
    struct BoeInterface *boe = malloc(sizeof *boe);
    TradingInterface_init(&(boe->base));
    boe->base.configure = boe_configure;
    TradingInterface_initROMConnection(&(boe->base.server_connection));
    boe->base.server_connection.on_message_using_stream_state =
        onDartCsvMessage;
    boe->base.run = boe_run;
    boe->base.is_connected = boe_is_connected;
    boe->base.send_heartbeat = boe_sendHeartbeat;
    boe->handleMessage = boe_disconnectedState;

    struct TradingInterfaceConnection *boeConnection =
        &(boe->base.client_connection);
    boeConnection->on_connection_update = boe_on_connection_update;
    boeConnection->frame_message = boe_frame;
    boeConnection->on_message = boe_on_message;
    boeConnection->on_idle = boe_on_idle;
    boeConnection->parse_sequence = boe_parse_sequence;
    boeConnection->create_reset_pad = boe_reset_pad;
    boeConnection->on_connection_established = onConnectionEstablished;
    boeConnection->check_idle_flag = 0;
    boeConnection->heartbeat_interval = 1;
    return boe;
}

int main(int argc, char *argv[])
{
    int retval = 0;
    if (argc > 1) {
        struct TradingInterface *exch =
            (struct TradingInterface *) BoeInterface_create();
        if ((retval = exch->configure(exch, argv[1])) == 0
            && (retval = exch->listen(exch)) == 0) {
            retval = exch->run(exch);
        }
    } else {
        fprintf(stderr, "usage: boe conf-file\n");
        retval = 1;
    }
    if (retval == -1) {
        retval = 2;
    }
    return retval;
}
