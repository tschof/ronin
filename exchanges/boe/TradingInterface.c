#include "TradingInterface.h"
#include "server_objects/dart_orderbook.h"
#include "server_objects/rom_handler.h"
#include "ex_common/ex_config_helper.h"
#include "smtp/mailer.h"

#include <stdio.h>

void rom_on_connection_update(con_obj * co, int is_connected, char *log,
                              int len)
{
}

void rom_on_idle(con_obj * connection)
{
    printf("%s\n", __FUNCTION__);
}

void place_holder_from_rom_func(struct message *mess, void *b)
{
}

void TradingInterface_initROMConnection(struct TradingInterfaceConnection
                                        *rom)
{
    rom->on_connection_update = rom_on_connection_update;
    rom->frame_message = parse_rom_message;
    rom->on_message = place_holder_from_rom_func;
    rom->on_idle = rom_on_idle;
    rom->parse_sequence = build_offsets;
    rom->create_reset_pad = create_rom_resend_pad;
    rom->check_idle_flag = 0;
    rom->heartbeat_interval = 30;
    rom->connection = 0;

    /* Configure the ROM Parser Helper. */
    rom->stream_state = create_rph(ph_build_dart_obj);
    rom->clone_stream_state = copy_rom_parser_helper;
    rom->on_message_using_stream_state = 0;
}

static int TradingInterface_listen(struct TradingInterface *ei)
{
    int retval = -1;
    ex_config *conf = ei->exchange_conf;
    if ((retval =
         start_dart_mail_service(conf->mail_server, conf->mail_server_len,
                                 conf->biz_group, conf->b_len,
                                 conf->tech_group, conf->t_len,
                                 conf->end_hour, conf->end_min,
                                 conf->end_day)) == 0) {
        struct TradingInterfaceConnection *rom = &(ei->server_connection);
        retval =
            start_dart_client_services_2(rom->on_message, conf->mod_name,
                                         conf->mod_name_len,
                                         conf->rom_port, ei->rom_conf,
                                         conf->rom_ip, conf->rom_ip_len,
                                         conf->end_hour, conf->end_min,
                                         conf->end_day,
                                         rom->heartbeat_interval, ei->book,
                                         rom->on_connection_update,
                                         ph_parse_rom_message,
                                         rom->parse_sequence, rom->on_idle,
                                         rom->create_reset_pad,
                                         rom->check_idle_flag,
                                         rom->stream_state,
                                         rom->clone_stream_state,
                                         rom->
                                         on_message_using_stream_state);
    }
    return retval;
}

time_t TradingInterface_getStopTime(struct TradingInterface * ei)
{
    time_t now = time(0), retval = 0;
    struct tm stop_time;
    if (localtime_r(&now, &stop_time) != 0) {
        stop_time.tm_sec = 0;
        stop_time.tm_min = ei->exchange_conf->end_min;
        stop_time.tm_hour = ei->exchange_conf->end_hour;
        retval = mktime(&stop_time);
    }
    return retval;
}

void TradingInterfaceConnection__init(struct TradingInterfaceConnection *p)
{
    p->on_connection_update = 0;
    p->frame_message = 0;
    p->on_message = 0;
    p->on_idle = 0;
    p->parse_sequence = 0;
    p->create_reset_pad = 0;
    p->on_connection_established = 0;
    p->check_idle_flag = 0;
    p->heartbeat_interval = -1;
    p->connection = 0;
}

int TradingInterface_configure(struct TradingInterface *ti,
                               const char *str)
{
    ti->stopTime = TradingInterface_getStopTime(ti);
    return -1;
}

static int TradingInterface_connect(struct TradingInterface *ei)
{
    struct TradingInterfaceConnection *orderdest =
        &(ei->client_connection);
    struct ex_con *conf = ei->exchange_conf;
    return dart_connect(orderdest->on_message, conf->mod_name,
                        conf->mod_name_len, conf->fix_port, conf->fix_ip,
                        conf->fix_ip_len, conf->end_hour, conf->end_min,
                        conf->end_day,
                        ei->client_connection.heartbeat_interval, ei,
                        orderdest->on_connection_update,
                        orderdest->frame_message, ei->session_conf,
                        orderdest->on_connection_established,
                        orderdest->parse_sequence, orderdest->on_idle,
                        orderdest->check_idle_flag);
}

void TradingInterface_init(struct TradingInterface *interface)
{
    interface->configure = TradingInterface_configure;
    interface->listen = TradingInterface_listen;
    interface->connect = TradingInterface_connect;
    interface->is_connected = 0;
    interface->send_heartbeat = 0;
    interface->run = 0;
    interface->exchange_conf = 0;
    interface->session_conf = 0;
    interface->rom_conf = 0;
    interface->stopTime = -1;
    TradingInterfaceConnection__init(&(interface->server_connection));
    TradingInterfaceConnection__init(&(interface->client_connection));
}
