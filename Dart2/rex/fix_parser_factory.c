/*************************************************************************
 * Copyright (c) 2013 by DART executions, LLC
 *
 * All Rights Reserved
 * ***********************************************************************/

#include "fix_parser_factory.h"
#include "rex_allocator.h"
#include "fix_trans.h"


static ff_desc* create_ff_out_array(int* len, char* dir, int d_len)
{
    char* full_path = calloc(1, d_len + 9);
    ff_desc* ffd = calloc(16, sizeof(struct fix_file_desc));
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "NewOrder", 8);
    ffd[0].filename = full_path;
    full_path = calloc(1, d_len + 7);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Cancel", 6);
    ffd[1].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Replace", 7);
    ffd[2].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "GapFill", 7);
    ffd[3].filename = full_path;
    full_path = calloc(1, d_len + 6);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Logon", 5);
    ffd[4].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Logout", 6);
    ffd[5].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "ResendReq", 9);
    ffd[6].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "MLegNew", 7);
    ffd[7].filename = full_path;
    full_path = calloc(1, d_len + 8);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "TestReq", 7);
    ffd[8].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "MLegReplace", 11);
    ffd[9].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Heartbeat", 9);
    ffd[10].filename = full_path;
    full_path = calloc(1, d_len + 10);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "SecDefReq", 9);
    ffd[11].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "Exreport", 8);
    ffd[12].filename = full_path;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "CancelRej", 9);
    ffd[13].filename = full_path;
    *len = 14;
    return ffd;
}

static ff_desc* create_ff_in_array(int* len, char* dir, int d_len)
{
    char* full_path = calloc(1, d_len + 16);
    ff_desc* ffd = calloc(15, sizeof(struct fix_file_desc));
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomExe", 11);
    ffd[0].filename = full_path;
    ffd[0].fix_type = 0x38;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomCnlRej", 14);
    ffd[1].filename = full_path;
    ffd[1].fix_type = 0x39;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomNORej", 13);
    ffd[2].filename = full_path;
    ffd[2].fix_type = 0x14;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomLogon", 13);
    ffd[3].filename = full_path;
    ffd[3].fix_type = 0x41;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomSecDef", 14);
    ffd[4].filename = full_path;
    ffd[4].fix_type = 0x64;
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomMail", 12);
    ffd[5].filename = full_path;
    ffd[5].fix_type = 0x42;
    //BizRej
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomBizRej", 14);
    ffd[6].filename = full_path;
    ffd[6].fix_type = 0x6A;
    //
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomSesRej", 14);
    ffd[7].filename = full_path;
    ffd[7].fix_type = 0x33;
    //done
    //New
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomNew", 11);
    ffd[8].filename = full_path;
    ffd[8].fix_type = 0x44;
    //done
    //Cancel
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomCnl", 11);
    ffd[9].filename = full_path;
    ffd[9].fix_type = 0x46;
    //done
    //Replace
    full_path = calloc(1, d_len + 16);
    memcpy(full_path, dir, d_len);
    memcpy(full_path + d_len, "FixToRomRpl", 11);
    ffd[10].filename = full_path;
    ffd[10].fix_type = 0x47;
    //done
    *len = 11;
    return ffd;
}


fixp* create_fix_parse_block(void* pass_back_pointer, 
                        void (*call_back)(void* pbp, dart_order_obj* doj),
                        otl* o,char* config_directory, int dir_len, int alloc)
{
    int out_len = 0;
    int in_len = 0;
    ff_desc* out_types = create_ff_out_array(&out_len,config_directory, dir_len);
    ff_desc* in_types = create_ff_in_array(&in_len,config_directory, dir_len);
    trans_t* t = create_translator(out_types,out_len,
                                    in_types, in_len);
    dartfact* d = create_dart_order_factory(alloc);
    fixp* fp = create_fix_parser(pass_back_pointer,
                                 call_back,d, o, t, alloc);
    return fp;
}

