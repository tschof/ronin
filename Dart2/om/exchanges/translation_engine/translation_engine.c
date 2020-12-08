#include "translation_engine.h"
#include "rdonly_csv_msg.h"

void free_translation_engine(translation_engine_t * p)
{
    if (p != 0) {

    }
    free(p);
}

#define require(condition, label)               \
    if (!(condition)) {                         \
        goto label;                             \
    }

static int translation_engine_translate(translation_engine_t * engine,
                                        translation_unit_t * unit)
{
    rule_t *rules = engine->rules;
    int ret = -1;
    while (rules != 0) {
        scalar_t result = { 0, 0 };
        if ((ret = run_operations(rules->ops, unit, &result)) == -1) {
            break;
        }
        if ((ret = unit->to->set(unit->to, rules->outfield, result)) == -1) {
            break;
        }
        rules = rules->next;
    }
    return ret;
}

translation_engine_t *make_translation_engine(istream_t * istream)
{
    translation_engine_t *ret = 0;
    require(istream != 0, done);
    ret = malloc(sizeof *ret);
    require(ret != 0, done);
    ret->translate = translation_engine_translate;
    proto_rule_parser_t *parser = make_proto_rule_parser(parse_entrypoint);
    ret->free = free_translation_engine;
    require(parser != 0, done);
    ret->global_vars = make_symbol_table();
    require(ret->global_vars != 0, done);

    /* Now that we are setup, parse the input stream. */
    parse_state_t *state = parser->parse(parser, istream);
    if (state == 0 || state->rules == 0) {
        ret->free(ret);
        ret = 0;
        goto done;
    }
    parser->free(parser);
    /* Freeing the tokenizer also frees the input stream that was
     * passed to it. */
    state->tokenizer->free(state->tokenizer);
    ret->global_vars = state->global_vars;
    ret->rules = state->rules;
    ret->output_layout = state->layout;
    free(state);
  done:
    return ret;
}

#ifdef COMPILE_UNIT_TESTS
#include "../../unit-tester/unit-tester.h"
#include "rw_fix_msg.h"

declare_unit_test(translation_engine)
{
    translation_engine_t *engine
        =
        make_translation_engine(make_file_inputstream("parseinput-2.in"));
    check(engine != 0);
    if (engine != 0) {
        rdonlymsg_t *csv =
            make_dartcsv_rdonly_msg
            ("E,,FIXH3TRA,11856000711,1,BRK/B,953,71.2,2,5,A,501,AFH21209,145,,RND,,FIXH3TRA-90130503-275,,,,,H3TRA,501,H3TRA,,,FIXH3TRA,,,,,,,,E,,,,,,145,,,,,,,,,20130503-12:03:30,,,,,,USD,,,,,,,,,,,,,,,,,,,,,,,foo\n");
        check(csv != 0);
        rw_msg_t *fix = make_rw_fix_msg(engine->output_layout);
        check(fix != 0);
        if (csv != 0 && fix != 0) {
            translation_unit_t *unit =
                make_translation_unit(csv, fix, engine->global_vars);
            check(unit != 0);
            if (unit != 0) {
                check(engine->translate != 0);
                if (engine->translate != 0) {
                    int result = engine->translate(engine, unit);
                    check(result == 0);
                    if (!result) {
                        fix->dump(fix, stdout);
                    } else {
                        /* fprintf(stderr, "Error translating: %s\n", unit->get_error_text(unit)); */
                    }
                }
                free_translation_unit(unit);
            }
        }
        check(engine->free != 0);
        if (engine->free != 0) {
            engine->free(engine);
        }
    }

}

declare_unit_test(inet)
{
    translation_engine_t *engine
        = make_translation_engine(make_file_inputstream("inet.rules"));
    check(engine != 0);
    if (engine != 0) {
        rdonlymsg_t *csv =
            make_dartcsv_rdonly_msg
            ("E,,SIMPGRK1,63941193,2,ARE/PRE,199,24.0900,5,0,A,501,AFM21209,146,0,Direct+-O,,SIMPGRK2-10130711-146197,,,,,ETFC2,GSEC,SIMPG,,,SIMPGRK2,SIMPGRK2-10130711-146197-0,,,,,,,E,,,,,,146,,,,,,,,,,,,,,,,,,,,,,,,,,,,,,13475,,,,,,");
        /* ("E,,SIMPGRK1,63941193,2,ARE/PRE,199,24.0900,2,0,A,501,AFM21209,146,0,,,SIMPGRK2-10130711-146197,,,,,ETFC2,GSEC,SIMPG,,,SIMPGRK2,SIMPGRK2-10130711-146197-0,,,,,,,E,,,,,,146,,,,,,,,,,,,,,,,M,,,,,,,,,,,,,,13475,,,,,,"); */
        check(csv != 0);
        rw_msg_t *fix = make_rw_fix_msg(engine->output_layout);
        check(fix != 0);
        if (csv != 0 && fix != 0) {
            translation_unit_t *unit =
                make_translation_unit(csv, fix, engine->global_vars);
            check(unit != 0);
            if (unit != 0) {
                check(engine->translate != 0);
                if (engine->translate != 0) {
                    int result = engine->translate(engine, unit);
                    check(result == 0);
                    if (!result) {
                        fix->dump(fix, stdout);
                        printf("\n");
                    } else {
                        /* fprintf(stderr, "Error translating: %s\n", unit->get_error_text(unit)); */
                    }
                }
                free_translation_unit(unit);
            }
        }
        check(engine->free != 0);
        if (engine->free != 0) {
            engine->free(engine);
        }
    }

}

#endif
