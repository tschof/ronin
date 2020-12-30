#ifndef __PARSER_H__
#define __PARSER_H__

struct ruleparser;
struct ruleparser *make_rule_parser(void);
void free_rule_parser(struct ruleparser *);

#endif
