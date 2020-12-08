#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>
#include <arpa/inet.h>

#include "tests.h"
#include "testmacros.h"
#include "databuf.h"
#include "dart_constants.h"
#include "dart_order_obj.h"
#include "order_obj_fact.h"
#include "dart_order_obj.h"
#include "connection_interface.h"
#include "ise.h"

void test_next(struct connection_interface* ci,
               dart_order_obj* doj)
{
}
int test_ise_parent_mngt_replace(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    con_interface *con_int = create_empty_interface();
    gatekeeper* gk = create_empty_gatekeeper(8, "ISE", 3,NULL, 1);
    con_int->gk = gk;
    oofact* ofact =  create_order_obj_factory(4);
    dart_order_obj *parent = get_doj(ofact);
    rom_style_reset(parent);
    dart_order_obj *child = get_doj(ofact);
    rom_style_reset(child);
    add_child_obj(parent, child);
    fill_in_rom_field(child, ROM_LEG_MARK, "1", 1); 
    fill_in_rom_field(child, ROM_SHARES, "3", 1);
    fill_in_rom_field(child, ROM_ORIG_SHARES, "3", 1);
    fill_in_rom_field(parent, ROM_SHARES, "2", 1);
    /* We have a 2 contract with 3 of the primary leg. */
    ise_calc_leg_ratio(child, parent);
    long ratio = 0;
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 6);

    fill_in_rom_field(parent, ROM_SHARES, "4", 1);
    reset_shares_on_replace(parent);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 12);
    /* Check with a partial fill*/
    fill_in_rom_field(child, ROM_STATUS, "1", 1); 
    fill_in_rom_field(child, ROM_LAST_SHARES, "1", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    CHKCOND(parent->positions[ROM_LEAVES].iov_len == 0);
    long delta = 0;
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 1);
    /*Another fill to get us in ratio*/
    fill_in_rom_field(child, ROM_LAST_SHARES, "2", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    char* leaves = parent->positions[ROM_LEAVES].iov_base;
    char* last = parent->positions[ROM_LAST_SHARES].iov_base;
    CHKCOND(strncmp(leaves, "3", 1) == 0);
    CHKCOND(strncmp(last, "1", 1) == 0);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 0);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 9);
    STOPTEST;
}
int test_ise_parent_mngt_replace_partials(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    con_interface *con_int = create_empty_interface();
    gatekeeper* gk = create_empty_gatekeeper(8, "ISE", 3,NULL, 1);
    con_int->gk = gk;
    oofact* ofact =  create_order_obj_factory(4);
    dart_order_obj *parent = get_doj(ofact);
    rom_style_reset(parent);
    dart_order_obj *child = get_doj(ofact);
    rom_style_reset(child);
    add_child_obj(parent, child);
    fill_in_rom_field(child, ROM_LEG_MARK, "1", 1); 
    fill_in_rom_field(child, ROM_SHARES, "3", 1);
    fill_in_rom_field(child, ROM_ORIG_SHARES, "3", 1);
    fill_in_rom_field(parent, ROM_SHARES, "1", 1);
    /* We have a 2 contract with 3 of the primary leg. */
    ise_calc_leg_ratio(child, parent);
    long ratio = 0;
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 3);
    fill_in_rom_field(parent, ROM_SHARES, "3", 1);
    reset_shares_on_replace(parent);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 9);
   /*Add a partial fill that is not in ratio*/ 
    fill_in_rom_field(child, ROM_STATUS, "1", 1); 
    fill_in_rom_field(child, ROM_LAST_SHARES, "1", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    CHKCOND(parent->positions[ROM_LEAVES].iov_len == 0);
    long delta = 0;
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 1);
    /*Now another replace comes through*/
    fill_in_rom_field(parent, ROM_SHARES, "5", 1);
    reset_shares_on_replace(parent);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 15);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 1);
    /*Now lets get a fill that gets us to ratio.*/
    fill_in_rom_field(child, ROM_LAST_SHARES, "2", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    char* leaves = parent->positions[ROM_LEAVES].iov_base;
    char* last = parent->positions[ROM_LAST_SHARES].iov_base;
    CHKCOND(parent->positions[ROM_LEAVES].iov_len == 1);
    CHKCOND(parent->positions[ROM_LAST_SHARES].iov_len == 1);
    CHKCOND(strncmp(leaves, "4", 1) == 0);
    CHKCOND(strncmp(last, "1", 1) == 0);
    /*Now get another ratio fill*/
    fill_in_rom_field(child, ROM_LAST_SHARES, "3", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    leaves = parent->positions[ROM_LEAVES].iov_base;
    last = parent->positions[ROM_LAST_SHARES].iov_base;
    CHKCOND(parent->positions[ROM_LEAVES].iov_len == 1);
    CHKCOND(parent->positions[ROM_LAST_SHARES].iov_len == 1);
    CHKCOND(strncmp(leaves, "3", 1) == 0);
    CHKCOND(strncmp(last, "1", 1) == 0);
    /*Another replace with no share change.*/
    fill_in_rom_field(parent, ROM_SHARES, "5", 1);
    reset_shares_on_replace(parent);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 9);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 0);
    /*Another partial fill out of ratio.*/
    fill_in_rom_field(child, ROM_LAST_SHARES, "2", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 9);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 2);
    /*Replace with no share change (make sure we do not alter ratio or delta.*/
    fill_in_rom_field(parent, ROM_SHARES, "5", 1);
    reset_shares_on_replace(parent);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 9);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 2);
    /*Now lower the shares to 4*/
    fill_in_rom_field(parent, ROM_SHARES, "4", 1);
    reset_shares_on_replace(parent);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 6);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 2);
    /*Now we get a full fill on all the rest.*/
    fill_in_rom_field(child, ROM_STATUS, "2", 1); 
    fill_in_rom_field(child, ROM_LAST_SHARES, "4", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "2", 1);
    leaves = parent->positions[ROM_LEAVES].iov_base;
    last = parent->positions[ROM_LAST_SHARES].iov_base;
    CHKCOND(parent->positions[ROM_LEAVES].iov_len == 1);
    CHKCOND(parent->positions[ROM_LAST_SHARES].iov_len == 1);
    CHKCOND(strncmp(leaves, "0", 1) == 0);
    CHKCOND(strncmp(last, "2", 1) == 0);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 0);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 0);
    STOPTEST;
}
int test_ise_parent_mngt(const char** testname, const char** extrareporting)
{
    STARTTEST;
    databuf_t *buff = databuf_alloc(2000);
    REQCOND(buff != 0);
    con_interface *con_int = create_empty_interface();
    gatekeeper* gk = create_empty_gatekeeper(8, "ISE", 3,NULL, 1);
    con_int->gk = gk;
    oofact* ofact =  create_order_obj_factory(4);
    dart_order_obj *parent = get_doj(ofact);
    rom_style_reset(parent);
    dart_order_obj *child = get_doj(ofact);
    rom_style_reset(child);
    /*Set up a 5 * 3 ratio for the parent and child*/
    fill_in_rom_field(child, ROM_LEG_MARK, "1", 1); 
    fill_in_rom_field(child, ROM_SHARES, "3", 1);
    fill_in_rom_field(child, ROM_ORIG_SHARES, "3", 1);
    fill_in_rom_field(parent, ROM_SHARES, "5", 1);
    ise_calc_leg_ratio(child, parent);
    long ratio = 0;
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 15);
    /*First partial fill of one, no parent message should be sent*/
    fill_in_rom_field(child, ROM_STATUS, "1", 1); 
    fill_in_rom_field(child, ROM_LAST_SHARES, "1", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    CHKCOND(parent->positions[ROM_LEAVES].iov_len == 0);
    long delta = 0;
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 1);
    /*Second partial fill of one, we are closer to ratio but still nothing 
     * should be sent, it will instead be accumulated in the delta.*/
    fill_in_rom_field(child, ROM_LAST_SHARES, "1", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    CHKCOND(parent->positions[ROM_LEAVES].iov_len == 0);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 2);
    /*Third partial fill will bring us to a threashold where we can send a parent
     * fill. We should see the delta reset and the filled amout reflected in the ratio.*/
    fill_in_rom_field(child, ROM_LAST_SHARES, "1", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    CHKCOND(parent->positions[ROM_LEAVES].iov_len == 1);
    char* leaves = parent->positions[ROM_LEAVES].iov_base;
    char* last = parent->positions[ROM_LAST_SHARES].iov_base;
    CHKCOND(strncmp(leaves, "4", 1) == 0);
    CHKCOND(strncmp(last, "1", 1) == 0);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 0);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 12);
    /*Now we simulate a partial fill that crosses the threshold for a one
     * lot fill in the parent but is still not in ratio.*/
    fill_in_rom_field(child, ROM_LAST_SHARES, "4", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 4);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 12);
    /*Now we have a partial fill that is not in ratio itself but with
     * the accumulated delta will make a ratio of 2 parent fills*/
    fill_in_rom_field(child, ROM_LAST_SHARES, "2", 1); 
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 0);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 6);
    leaves = parent->positions[ROM_LEAVES].iov_base;
    last = parent->positions[ROM_LAST_SHARES].iov_base;
    CHKCOND(strncmp(leaves, "2", 1) == 0);
    CHKCOND(strncmp(last, "2", 1) == 0);
    /*Now to finish off the test with a partial that is out of 
     * ratio and then a fill for the remainder*/
    fill_in_rom_field(child, ROM_LAST_SHARES, "2", 1);
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "1", 1);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 2);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 6);
    /*Final fill*/
    fill_in_rom_field(child, ROM_LAST_SHARES, "4", 1);
    ise_handle_parent_fill(child, parent,con_int,
                           test_next, "2", 1);
    memcpy(&delta, parent->positions[ROM_BIN_SHARES_DELTA].iov_base,
            parent->positions[ROM_BIN_SHARES_DELTA].iov_len);
    CHKCOND(delta == 0);
    memcpy(&ratio, parent->positions[ROM_BIN_RATIO].iov_base,
            parent->positions[ROM_BIN_RATIO].iov_len);
    CHKCOND(ratio == 0);
    leaves = parent->positions[ROM_LEAVES].iov_base;
    last = parent->positions[ROM_LAST_SHARES].iov_base;
    CHKCOND(strncmp(leaves, "0", 1) == 0);
    CHKCOND(strncmp(last, "2", 1) == 0);
    STOPTEST;
}
