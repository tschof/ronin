#ifndef __CONVERSION_RULES_H__
#define __CONVERSION_RULES_H__

#include "dstring.h"
#include "server_objects/dart_order_obj.h"
#include <stdlib.h>

struct ConversionRuleParser {
    struct ConversionRule *rules, *currentRule;
    size_t linen;
    char *linep, *p, *errorp;
};

typedef int (*conversionFunction)(struct ConversionRule *, struct dart_order_obj *,
                                  struct dstring *);

struct ConversionRule {
    int csvfield;
    conversionFunction convert;
    void *data;
    struct ConversionRule *next;
};

void newRule(struct ConversionRuleParser *);
int beginParse(struct ConversionRuleParser *);
void dumpConversionRule(struct ConversionRule *);

extern const struct ConversionRuleParser conversionRuleParserDefaults;

#endif
