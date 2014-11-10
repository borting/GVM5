#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ComProtocol.h"
#include "ErrDbg.h"
#include "sockop.h"

int connfd;

VAPP_REP REPMsg;
VAPP_REQ REQMsg;

void reqOpenApp(char *cmd)
{
    int len;
    int writeLen = sizeof(REQMsg.type);
    int retval;
    unsigned char *ip;

    /* prepare request */
    len = strlen(cmd);
    REQMsg.cmd.open_app.app.len = len;
    memcpy(REQMsg.cmd.open_app.app.name, cmd, len);

    /* send request */
    writeLen += sizeof(REQMsg.cmd.open_app.app.len) + len;
    retval = write(connfd, &REQMsg, writeLen);
    if (retval < 0) {
	stderrexit(-2, "send request to vAppd failed");
    }

    /* receive reply */
    retval = read(connfd, &REPMsg, sizeof(VAPP_REP));
    if (retval < 0) {
	stderrexit(-2, " receive reply from vAppd failed");
    }

    /* print result */
    printf("REPMsg.type = %u\n", REPMsg.type);
    printf("REPMsg.retval = %d\n", REPMsg.retval);
    ip = REPMsg.msg.open_app.ip;
    printf("REPMsg.msg.open_app.ip = %hhu.%hhu.%hhu.%hhu\n", 
	    ip[0], ip[1], ip[2], ip[3]);
    printf("REPMsg.msg.open_app.pvt_port = %hu\n", 
	    REPMsg.msg.open_app.pvt_port);
}

void reqCloseApp(char *cmd)
{
    int writeLen = sizeof(REQMsg.type);
    int retval;

    /* prepare request */
    REQMsg.cmd.close_app.pvt_port = (unsigned short)atoi(cmd);

    /* send request */
    writeLen += sizeof(REQMsg.cmd.close_app);
    retval = write(connfd, &REQMsg, writeLen);
    if (retval < 0) {
	stderrexit(-2, "send request to vAppd failed");
    }

    /* receive reply */
    retval = read(connfd, &REPMsg, sizeof(VAPP_REP));
    if (retval < 0) {
	stderrexit(-2, " receive reply from vAppd failed");
    }

    /* print result */
    printf("REPMsg.type = %u\n", REPMsg.type);
    printf("REPMsg.retval = %d\n", REPMsg.retval);
}

void reqDefault()
{
    int retval;

    /* send all data */
    retval = write(connfd, &REQMsg, sizeof(VAPP_REQ));
    if (retval < 0) {
	stderrexit(-2, "send request to vAppd failed");
    }

    /* receive reply */
    retval = read(connfd, &REPMsg, sizeof(VAPP_REP));
    if (retval < 0) {
	stderrexit(-2, " receive reply from vAppd failed");
    }

    /* print result */
    printf("REPMsg.type = %u\n", REPMsg.type);
    printf("REPMsg.retval = %d\n", REPMsg.retval);
}

int main (int argc, char *argv[])
{
    int type;
    char *cmd;

    if (argc < 5) {
	stderrexit(-1, "%s <vAppd IP> <vAppd port> <type> <cmd>\n", argv[0]);
    }
    else {
	type = atoi(argv[3]);
	cmd = argv[4];
    }

    /* connect to vAppd */
    connfd = connectsock(argv[1], argv[2], "tcp");
    if (connfd < 0)
	stderrexit(-1, "connect tp vAppd failed");

    /* prepare command */
    memset(&REPMsg, 0, sizeof(VAPP_REP));
    memset(&REQMsg, 0, sizeof(VAPP_REQ));

    /* process routine */
    REQMsg.type = type;
    switch (type) {
	case 0:
	    reqOpenApp(cmd);
	    break;
	case 4:
	    reqCloseApp(cmd);
	    break;
	default:
	    reqDefault();
    }

    /* clean up */
    close(connfd);

    return 0;
}
