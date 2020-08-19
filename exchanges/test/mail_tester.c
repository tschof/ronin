#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <sys/time.h>

#include "tests.h"
#include "testmacros.h"
#include "databuf.h"
#include "dart_constants.h"
#include "message_queue.h"
#include "mailer.h"





int test_mail_message(const char **testname, const char **extrareporting)
{
    STARTTEST queue_t q = create_queue();
    REQCOND(q != 0);
    char *mail_server = "relay.ronin.roncap.com";
    int mail_server_len = strlen(mail_server);
    char *biz_group = "parnell.flynn@dartexecutions.com";
    int b_len = strlen(biz_group);
    char *tech_group = "parnell.flynn@ronin-capital.com";
    int t_len = strlen(tech_group);
    char *message = "Biz Alert Test Message.";
    char *title = "Unit tests work!!!!";
    int ti_len = strlen(title);
    char *mess2 = "Tech Alert Test Message.";
    int mess2_len = strlen(mess2);
    int len = strlen(message);
    start_dart_mail_service(mail_server, mail_server_len,
                            biz_group, b_len, tech_group,
                            t_len, 23, 23, 7);
    send_biz_message(message, len, title, ti_len);
    send_tech_message(mess2, mess2_len, title, ti_len);

    sleep(5);
STOPTEST}
