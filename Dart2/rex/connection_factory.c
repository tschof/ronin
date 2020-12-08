/*************************************************************************
 * Copyright (c) 2013 by DART executions, LLC
 *
 * All Rights Reserved
 * ***********************************************************************/

#include "connection_factory.h"
#include "fix_parser_factory.h"
#include "rex_allocator.h"
#include "fix_handler.h"
#include "ex_config_helper.h"


static int *set_cpus(char *cfg_cpu, int cfg_cpu_len, int *ncpus)
{
    int i = 1;
    char *off = cfg_cpu;
    int cur_len = cfg_cpu_len;
    while ((off = memchr(off, ',', cur_len)) != NULL) {
        ++i;
        off += 1;
        cur_len = cfg_cpu_len - (off - cfg_cpu);
    }
    *ncpus = i;
    int *cpus = calloc(i * sizeof(int), 1);
    char *cpu_tok = cfg_cpu;
    char *c = memchr(cpu_tok, ',', cfg_cpu_len);
    int j = 0;
    cur_len = cfg_cpu_len;
    while (c) {
        cpus[j] = atoi(cpu_tok);
        cpu_tok = c + 1;
        cur_len = cfg_cpu_len - (cpu_tok - cfg_cpu);
        ++j;
        c = memchr(cpu_tok, ',', cur_len);
    }
    if (j < i) {
        cpus[j] = atoi(cpu_tok);
    }
    return cpus;
}

pserver* start_base_services(ex_config* bc)
{
    otl* o = create_otl(bc->directory, bc->dir_len);
    int ncpus = 0;
    int *cpus = set_cpus(bc->cpus, bc->cpulen, &ncpus);
    pserver* ps = start_server(bc->num_threads, bc->time_out,
                                 cpus, 512, o);
    return ps;
}

pserver* start_lib_base_services(ex_config* bc)
{
    otl* o = create_otl(bc->directory, bc->dir_len);
    if(o) {
        int ncpus = 0;
        int *cpus = set_cpus(bc->cpus, bc->cpulen, &ncpus);
        pserver* ps = start_lib_server(bc->num_threads, bc->time_out,
                                     cpus, 512, o);
    return ps;
    } else {
        return NULL;
    }
}

login_info* build_logon_info_from_config(ex_config* ex)
{

    login_info* li = 0;
    int ret = 0;
    REX_ALLOC(li, 8, sizeof(login_info), ret);
    memcpy(li->name, ex->username, ex->ulen);
    if(ex->plen > 0) {
        memcpy(li->password, ex->password, ex->plen);
    }
    li->plen = ex->plen;
    li->last_logon = ex->last_logon;
    li->nlen = ex->ulen;
    li->reset_hour = ex->reset_hour;
    li->reset_min = ex->reset_min;
    li->reset_day  = ex->reset_day;
    li->create_new = ex->create_new;
    li->new_incoming = ex->new_incoming;
    li->new_outgoing = ex->new_outgoing;
    return li;
}


