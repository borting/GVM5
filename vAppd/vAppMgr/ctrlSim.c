#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "sockop.h"

int main(int argc, char *argv[])
{
    int sockfd, connfd; /* socket descriptor */
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    int retval;
    unsigned short pvt_port;
    int replyMsg = 0;

    if (argc != 2)
	errexit("Usage: %s port\n", argv[0]);

    /* create socket and bind socket to port */
    sockfd = passivesock(argv[1], "tcp", 10);

    while(1)
    {
	/* waiting for connection */
	connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);
	if (connfd == -1)
	    errexit("Error: accept()\n");
	else
	    printf("vAppMgr connected\n");

	/* read message from client */
	do {
	    retval = read(connfd, &pvt_port, sizeof(unsigned short));

	    if (retval < 0)
		errexit("Error: read()\n");
	    else if (retval > 0) {
		printf("private port %hu reclaimed\n", pvt_port);
		write(connfd, &replyMsg, sizeof(int));
	    }
	    else
		printf("vAppMgr disconnected\n");
	} while (retval > 0);

	/* close client connection */
	close(connfd);
    }

    /* close server socket */
    close(sockfd);

    return 0;
}

