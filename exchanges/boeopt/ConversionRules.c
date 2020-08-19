#include "ConversionRules.h"
#include <stdio.h>
#include <ctype.h>

const struct ConversionRuleParser conversionRuleParserDefaults = {
    .rules = 0,
    .currentRule = 0,
    .linen = 1,
    .linep = 0,
    .p = 0,
    .errorp = 0,
};

#define eatWhitespace(parserp) for (;isspace(*parserp->p) != 0; ++parserp->p)

struct Slice {
    int begin, end;
};

int sliceString(struct ConversionRule *rule, struct dart_order_obj *csvmsg,
                struct dstring *result)
{
    result->p = 0;
    result->nlen = 0;
    result->flags = 0x0000;
    struct Slice *slice = rule->data;
    int n;
    char *p = get_rom_field(csvmsg, rule->csvfield, &n);
    if (n == 0) {
        return -1;
    }
    if (slice->begin == -1) {
        result->p = p;
    } else if (slice->begin > (n - 1)) {
        return -1;
    } else {
        result->p = p + slice->begin;
    }
    if (slice->end == -1) {
        result->nlen = n;
    } else if (slice->end > (n - 1)) {
        result->p = 0;
        return -1;
    } else {
        result->nlen = slice->end - slice->begin;
    }
    return 0;
}

void dumpConversionRule(struct ConversionRule *rule)
{
    printf("Field %d,", rule->csvfield);
    if (rule->convert == sliceString) {
        printf(" slice string");
        if (rule->data == 0) {
            printf(" no args");
        } else {
            struct Slice *slicep = rule->data;
            printf(" begin=%d, end=%d", slicep->begin, slicep->end);
        }
    } else {
        printf(" unknown conversion");
    }
    printf("\n");
}

static const struct ConversionRule conversionRuleDefaults = {
    .csvfield = -1,
    .convert = 0,
    .data = 0,
    .next = 0,
};

void newRule(struct ConversionRuleParser *parser)
{
    struct ConversionRule *rule = malloc(sizeof *rule);
    *rule = conversionRuleDefaults;
    if (parser->currentRule == 0)
        parser->currentRule = parser->rules = rule;
    else {
        parser->currentRule->next = rule;
        parser->currentRule = parser->currentRule->next;
    }
}

int reportError(struct ConversionRuleParser *parser, const char *msg)
{
    fprintf(stderr, "%zd:%zd %s\n", parser->linen,
            parser->p - parser->linep + 1, msg);
    return -1;
}

int unsignedInteger(struct ConversionRuleParser *parser)
{
    eatWhitespace(parser);
    int i = 0, multiplier;
    char c;
    char *p = parser->p;
    for (multiplier = 1, c = *p++;
         isdigit(c) != 0; multiplier = 10, c = *p++) {
        c -= '0';
        i *= multiplier;
        i += c;
    }
    --p;
    if (p == parser->p)
        return -1;
    parser->p = p;
    return i;
}

int slice(struct ConversionRuleParser *parser)
{
    eatWhitespace(parser);
    if (*parser->p != '(')
        return reportError(parser, "expecting slice (starting with '('");
    ++parser->p;
    eatWhitespace(parser);
    struct Slice *slicep = malloc(sizeof *slicep);
    slicep->begin = unsignedInteger(parser);
    eatWhitespace(parser);
    if (*parser->p != ':')
        return reportError(parser, "Invalid slice syntax, no ':'");
    ++parser->p;
    eatWhitespace(parser);
    slicep->end = unsignedInteger(parser);
    if (slicep->begin != -1 && slicep->end != -1
        && slicep->end <= slicep->begin) {
        free(slicep);
        return reportError(parser, "invalid slice");
    }
    eatWhitespace(parser);
    if (*parser->p != ')') {
        free(slicep);
        return reportError(parser, "expecting closing parenthesis");
    }
    parser->currentRule->data = slicep;
    parser->currentRule->convert = sliceString;
    return 0;
}

int dartCSVField(struct ConversionRuleParser *parser)
{
    eatWhitespace(parser);
    if ((parser->currentRule->csvfield = unsignedInteger(parser)) == -1)
        return reportError(parser, "No DART CSV field number specified");
    return slice(parser);
}

int beginParse(struct ConversionRuleParser *parser)
{
    eatWhitespace(parser);
    if (*parser->p++ != '#')
        return reportError(parser, "Expecing DART CSV field");
    else
        return dartCSVField(parser);
}
