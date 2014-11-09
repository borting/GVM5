#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "dnat_cmd.h"
#include "sockop.h"

int main (int argc, char *argv[])
{
    int retval;
    DNAT_CMD cmd, rpy;
    int connfd;

    if (argc != 7)
	errexit("Usage: %s host_address host_port type pub_port pvt_ip pvt_port\n", argv[0]);

    sscanf(argv[3], "%d", &cmd.type);
    sscanf(argv[4], "%hd", &cmd.pub_port);
    sscanf(argv[5], "%hhu.%hhu.%hhu.%hhu", 
	    &cmd.pvt_ip[0], &cmd.pvt_ip[1], &cmd.pvt_ip[2], &cmd.pvt_ip[3]);
    sscanf(argv[6], "%hd", &cmd.pvt_port);

    /* create socket and connect to server */
    connfd = connectsock(argv[1], argv[2], "tcp");

    /* write message to server */
    if ((retval = write(connfd, &cmd, sizeof(DNAT_CMD))) == -1)
	errexit("Error: write()\n");

    /* read message from the server and print */
    if ((retval = read(connfd, &rpy, sizeof(DNAT_CMD))) == -1)
	errexit("Error: read()\n");

    printf("Reply: \n");
    printf("Reply Code   : %d\n", rpy.type);
    printf("private IP   : %hhu.%hhu.%hhu.%hhu\n", 
	    cmd.pvt_ip[0], cmd.pvt_ip[1], cmd.pvt_ip[2], cmd.pvt_ip[3]);
    printf("public port  : %hd\n", rpy.pub_port);
    printf("private port : %hd\n", rpy.pvt_port);

    /* close client socket */
    close(connfd);

    return 0;
}
