#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "version.h"
#include "dartglob.h"
#include "oats.h"
#include "balance_checker.h"
#include "gatekeeper.h"
#include "direct_drive.h"


enum program_return_codes {
    ok = 0,
    bad_args,
    bad_config,
};
static void clean_interfaces(queue_t q)
{
    element_t e, temp;
    e = dequeue(q);
    while (e != NULL) {
        temp = e;
        e = temp->next;
        con_int* in =(con_int*)temp->d;
        in->cleansing_rinse(in);
        temp->d = 0;
        free_element(q, temp);
    }
    destroy_queue(q);
}

int main(int argc, char *argv[])
{
    if (argc < 4) {
        fprintf(stderr,
                "usage: dartglob glob.dat glob.conf glob_listen.conf dest_map_file\n");
        return bad_args;
    }
    struct dartglob_entry *listeners;
    struct dartglob_entry *globs;
    if ((globs = dartglob_parse_config(argv[2])) == 0) {
        fprintf(stderr, "bad config\n");
        return bad_config;
    }
    if((listeners = dartglob_parse_config(argv[3])) == 0) {
        fprintf(stderr, "bad config\n");
        return bad_config;
    }

    if (dartglob_load_shared_objects(listeners) == -1) {
        fprintf(stderr, "failed to load some or all shared objects");
    }
    if (dartglob_load_shared_objects(globs) == -1) {
        fprintf(stderr, "failed to load some or all shared objects");
    }

    struct ddart_control_structs* dcs = start_direct_drive(listeners, globs, argc, argv);
    if(dcs == NULL || dcs->ass == NULL) {
        fprintf(stderr, "bad config need at least 2 threads, one for file writes and one for work\n");
        return bad_config;
    } else {
        join_n_clean(dcs->ass);
    }
    clean_interfaces(dcs->interfaces);
    clean_obj(dcs->gk);
    destroy_balance_checker((jp_limits_checker*)dcs->jp);
    free(dcs->ass);
    free(dcs->gk);
    destroy_life_cycle_master(dcs->cm);
    destroy_ex_config(dcs->mc);
    struct dartglob_entry *glob;
    while(globs) {
        glob = globs;
        globs = globs->next;
        free(glob->config_file_name);
        free(glob->shared_object_name);
        free(glob);
    }
    free(dcs);
    return ok;
}

