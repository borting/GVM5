#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "ErrDbg.h"
#include "MgrMsg.h"
#include "procReq.h"
#include "sockop.h"

/* create socket for ctrl Server forwarding request */
int sockfd;
char* vAppdPortStr;

/* for process reaper to connect vAppMgr & report dead child */ 
/* long-term connection */
static int mgrfd;
static MgrMsg mgrMsg;
char* mgrPortStr;

/* process reaper */
void Reaper(int sig)
{
    pid_t pid;
    int status;

    /* reaper zombie process */
    while((pid = waitpid(-1, &status, WNOHANG)) > 0) {
	mgrMsg.req.pid = pid;	/* notify vAppMgr child pid */
	write(mgrfd, &mgrMsg, sizeof(MgrMsg));	// DONOT check
    }
}

void init()
{
    /* create server socket */
    sockfd = passivesock(vAppdPortStr, "tcp", 10);
    if (sockfd < 0)
	stderrexit(-1, "create vAppd server socket failed");

    /* coonect to vAppMgr */
    mgrfd = connectsock("127.0.0.1", mgrPortStr, "tcp");
    if (mgrfd < 0)
	stderrexit(-1, "connect to manager server failed");

    /* set protocol to vAppMgr */
    memset(&mgrMsg, 0, sizeof(MgrMsg));
    mgrMsg.type = 4;	/* child process killed, reclaim port */

    // TODO: set sockfd & mgrfd closed soon after fork in child process

    /* register signal handler */
    signal(SIGCHLD, Reaper);

    /* graphic env setting */
    putenv("COLORTERM=gnome-terminal");
    putenv("DISPLAY=localhost:10.0");

    /* broadway env setting */
    putenv("GDK_BACKEND=broadway");
    putenv("UBUNTU_MENUPROXY=");
    putenv("LIBOVERLAY_SCROLLBAR=0");
    putenv("BROADWAY_DISPLAY=");    // assign port here

    /* init variables for procRoutine() */
    initProcRoutine();
}

void destroy()
{
    close(sockfd);
    close(mgrfd);
}

void disableReaper()
{
    struct sigaction action;

    memset(&action, 0, sizeof(struct sigaction));
    action.sa_handler = SIG_DFL;

    sigaction(SIGCHLD, &action, NULL);
}

int main(int argc, char *argv[])
{
    /* local variable declaration */
    int connfd;
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);

    pid_t pid;
    int retval;

    /* input parsing */
    if (argc != 3)
	stderrexit(-1, "Usage: %s vAppd_port mgr_port\n", argv[0]);

    vAppdPortStr = argv[1];
    mgrPortStr = argv[2];

    /* initialization */
    init();

    /* server routine */
    while (1)
    {
	/* wait for connnection */
	connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);
	if (connfd < 0) {
	    stderrexit(-1, "Error: accept()\n");
	}

	/* fork dedicate service process */
	pid = fork();
	if (pid < 0) {
	    printErr("Error: fork process failed");
	    close(connfd);
	    // TODO: Q: Is this handle okay?
	}
	else if (pid > 0) { // parent process
	    /* close connection of client */
	    close(connfd);
	}
	else { // pid = 0,  child process
	    /* pre-process */
	    close(sockfd);
	    close(mgrfd);   /* only for proc reaper, closed after fork */
	    disableReaper();

	    /* process request */
	    retval = procRoutine(connfd, mgrPortStr);

	    /* post-process */
	    close(connfd);
	    exit(retval);
	}
    }

    /* clean up serviecs */
    destroy();

    return 0;
}
