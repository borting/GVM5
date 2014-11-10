#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ErrDbg.h"
#include "MgrMsg.h"
#include "sockop.h"

int main(int argc, char *argv[])
{
    int connfd;
    int retval;
    MgrMsg reqMsg;
    int type, req;
    unsigned short pvt_port;
    pid_t pid;

    if (argc != 3)
	errexit("Usage: %s host_address host_port`\n", argv[0]);

    /* create socket and connect to server */
    connfd = connectsock(argv[1], argv[2], "tcp");
    if (connfd < 0)
	stderrexit(-1, "Connect to vAppMgr failed");
    else
	printf("Connect to vAppMgr\n");

    do {
	/* get input */
	printf("Input request: ");
	scanf("%d %d", &type, &req);

	/* prepare reqMsg */
	reqMsg.type = type;
	if (type == -1)
	    continue;
	else if (type == 3)	/* port */
	    reqMsg.req.pvt_port = (unsigned short)req;
	else
	    reqMsg.req.pid = req;

	/* send request */
	retval = write(connfd, &reqMsg, sizeof(MgrMsg));
	if (retval < 0)
	    errexit("Error: write()\n");

	/* get reply if necessary */
	if (type == 1) {
	    read(connfd, &pvt_port, sizeof(unsigned short));
	    printf("Pid %d gets port %hu\n", req, pvt_port);
	}

	if (type == 3) {
	    read(connfd, &pid, sizeof(pid_t));
	    printf("Pid %d has port %hu\n", pid, (unsigned short)req);
	}

    } while (type > -1);

    /* close client socket */
    close(connfd);
    printf("Disconnect with vAppMgr\n");

    return 0;
}

