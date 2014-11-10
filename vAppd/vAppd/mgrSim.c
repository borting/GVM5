#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ErrDbg.h"
#include "MgrMsg.h"
#include "sockop.h"

void procRoutine(int connfd)
{
    unsigned short pvt_port;
    pid_t pid;
    int retval;
    MgrMsg mgrMsg;

    while(1) {
	/* receive request */
	retval = read(connfd, &mgrMsg, sizeof(MgrMsg));
	if (retval < 0) {
	    stderrexit(-2, "read data from vAppd failed");
	}
	else if (retval == 0)
	    break;

	printf("%d: retval = %d\n", getpid(), retval);

	/* handle process */

	printf("%d %d\n", retval, mgrMsg.type);
	switch (mgrMsg.type) {
	    case 0:
		printf("%d: (0)Request type = %d, pid = %d\n", 
			getpid(), mgrMsg.type, mgrMsg.req.pid);
		break;
	    case 1:
		printf("%d: (1)Request type = %d, pid = %d\n", 
			getpid(), mgrMsg.type, mgrMsg.req.pid);
		printf("Reply pvt_port = ");
		scanf("%hu", &pvt_port);
		write(connfd, &pvt_port, sizeof(unsigned short));
		break;
	    case 2:
		printf("%d: (2)Request type = %d, pid = %d\n", 
			getpid(), mgrMsg.type, mgrMsg.req.pid);
		break;
	    case 3:
		printf("%d: (3)Request type = %d, pvt_port = %hu\n", 
			getpid(), mgrMsg.type, mgrMsg.req.pvt_port);
		printf("Reply pid = ");
		scanf("%d", &pid);
		write(connfd, &pid, sizeof(pid_t));
		break;
	    case 4:
		printf("%d: (4)Request type = %d, pid = %d\n", 
			getpid(), mgrMsg.type, mgrMsg.req.pid);
		break;
	    default:
		printf("%d: Request type = %d", getpid(), mgrMsg.type);
	}
    };

    printf("exit\n");
}

int main (int argc, char *argv[])
{
    int sockfd, connfd;
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);
    pid_t pid;
    
    if (argc < 2) {
	stderrexit(-1, "%s <port>\n", argv[0]);
    }
    else
	printf("pid = %d\n", getpid());

    /* create server socket */
    sockfd = passivesock(argv[1], "tcp", 10);
    if (sockfd < 0) {
	stderrexit(-1, "create server socket failed");
    }

    /* process routine */
    while(1) {
	connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);
	if (connfd < 0) {
	    stderrexit(-1, "Error: accept()\n");
	}

	/* fork process */
	pid = fork();
	if (pid < 0) {
	    stderrexit(-1, "fork process failed");
	}
	else if (pid > 0) { // parent
	    close(connfd);
	    continue;
	}
	else { // child
	    close(sockfd);
	    procRoutine(connfd);
	    close(connfd);
	    exit(0);
	}
    }

    close(sockfd);

    return 0;
}
