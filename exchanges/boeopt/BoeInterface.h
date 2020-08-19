#ifndef __BOETRADINGINTERFACE_H__
#define __BOETRADINGINTERFACE_H__

#include "TradingInterface.h"
#include "boe_util.h"
#include "FileFilter.h"
#include "boemsg.h"

struct BoeInterface;
struct parse_con;

typedef int (*boe_handleMessage) (struct BoeInterface *,
                                  struct BoeHeader *,
                                  int * /*cut connection */ ,
                                  struct parse_con *);

struct BoeInterface {
    struct TradingInterface base;
    boe_handleMessage handleMessage;
    struct boe_timestamp_converter *tsconverter;
    struct FileFilter *logFileFilter;
};

#endif
