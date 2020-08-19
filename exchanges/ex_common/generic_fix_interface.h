#ifndef __GENERIC_FIX_INTERFACE_H__
#define __GENERIC_FIX_INTERFACE_H__

struct parse_args;
typedef int (*on_start_func) (ex_config * exchange_config,
                              orderbook * pbook);
typedef char *(*id_generator_func) (size_t * nlen);
typedef int (*on_connect_func) (struct parse_args * pa);
typedef void (*on_session_status_change_func) (con_obj * connection,
                                               int is_connected, char *log,
                                               int len);

struct fix_destination_interface {
    /** Name of the configuration file to be used. Must be located in the
        current working directory. */
    const char *config_file_name;

    /** Length, in bytes of the config_file_name member. */
    int file_name_length;

    /** Callback function when started before configuration or the application
        enters a running state. Customization of the overrides should be done
        here. */
    on_start_func on_start;

    /** Callback function called when socket is connected. This is defaulted to
        send_logon, which sends a basic FIX logon message, but can be overrided
        if the FIX logon message needs to be customized or if other actions need
        to be taken. */
    on_connect_func on_connect;

    /** Callback function called when session is established or is broken. */
    on_session_status_change_func on_session_status_change;

    /** Callback function called before the individual Dart message field
        translation callbacks, usually used to apply more header fields. */
    fieldxlatefunc pre_translate_func;

    /** Function used to generate unique IDs for the destination, mainly
        ClOrdIDs. */
    id_generator_func id_generator;
};


/** \brief Main program entry point when using the generic framework. Customize
    the handler as needed in this callback.
    \param argc The number of command-line arguments - the same as that passed
    by the OS into the program's main entrypoint.
    \param argv The array of program arguments - the same as that passed
    by the OS into the program's main entrypoint.
    \param handler The handler defines some customization points for this
    particular interface and can be changed in this callback function to
    change the behavior.
 */
int generic_fix_interface_main(int argc, char *argv[],
                               struct fix_destination_interface *handler);

void default_on_session_status_changed(con_obj * connection,
                                       int is_established, char *log,
                                       int len);

#endif
