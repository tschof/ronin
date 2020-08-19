#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

#include "version.h"
#include "startup.h"
#include "dartglob.h"
#include "jigglypuff.h"


enum program_return_codes {
    ok = 0,
    bad_args,
    bad_config,
};
static void clean_interfaces(queue_t q,
        struct dart_control_structs* dcs)
{
    element_t e, temp;
    e = dequeue(q);
    while (e != NULL) {
        temp = e;
        e = temp->next;
        con_interface *in =(con_interface*)temp->d;
        remove_obj_by_client_id(dcs->gk, in->module_name,
                             in->module_name_len);

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
                "usage: dartglob glob.dat glob.conf oats.dat dest_map_file\n");
        return bad_args;
    }

    struct dartglob_entry *globs;
    if ((globs = dartglob_parse_config(argv[2])) == 0) {
        fprintf(stderr, "bad config\n");
        return bad_config;
    }

    if (dartglob_load_shared_objects(globs) == -1) {
        fprintf(stderr, "failed to load some or all shared objects");
    }

    /*if (dartglob_startup(globs, argc, argv) == -1) {
        fprintf(stderr, "could not run startup\n");
    }*/

    struct dart_control_structs* dcs = dartglob_startup(globs, argc, argv);
    if(dcs == NULL || dcs->ass == NULL) {
        fprintf(stderr, "bad config need at least 3 threads, one for poll, one for file writes and one for work\n");
        return bad_config;
    } else {
        join_n_clean(dcs->ass);
    }
    clean_interfaces(dcs->interfaces, dcs);
    clean_obj(dcs->gk);
    destroy_oats(dcs->oat);
    destroy_jiggly((jp_limits_checker*)dcs->jp);
    free(dcs->ass);
    free(dcs->gk);
    destroy_life_cycle_master(dcs->cm);
    destroy_master_config(dcs->mc);
    /* In the future we will have to do more than pause, we will have
     * to calculate the shutdown time and loop here trying to
     * reconnect if needed, until the shutdown time. */
   // pause();
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
