#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "ComProtocol.h"
#include "ErrDbg.h"
#include "hive.h"
#include "MgrMsg.h"
#include "procReq.h"
#include "sockop.h"

/* for connecting to vAppMgr & reporting process status */
static int mgrfd;
static MgrMsg mgrMsg;

/* for processing request forwarded from ctrl server*/
VAPP_REQ REQMsg;
VAPP_REP REPMsg;

/* init variables for procRoutine() */
void initProcRoutine()
{
    memset(&mgrMsg, 0, sizeof(MgrMsg));
    memset(&REQMsg, 0, sizeof(VAPP_REQ));
    memset(&REPMsg, 0, sizeof(VAPP_REP));
}

int replyOpenApp(int connfd, int retval, unsigned short pvt_port)
{
    REPMsg.retval = retval;
    REPMsg.msg.open_app.pvt_port = pvt_port;

    if ((write(connfd, &REPMsg, sizeof(REPMsg))) < 0) {
	printErr("Error: reply to Ctrl server failed");
	return -1;
    }
    else 
	return 0;
}

int procOpenApp(int connfd)
{
    DATA_OBJ *appData = &(REQMsg.cmd.open_app.app);
    char *rawCMD;
    unsigned short pvt_port;
    int retval;
    pid_t pid, ppid;
    char *cmd;
    char *args[256] = {0};
    char *envs[256] = {0};

    /* read app's name */
    retval = readDataObj(connfd, appData);  //TODO: simplify it
    if (retval != 0) {
	printErr("read request data from client failed");
	return -3;  // connection error, close w/o reply
    }
    
    /* parse appName */
    rawCMD = appData->name;

    retval = searchGene(rawCMD, &cmd, args, envs);
    if (retval == -1) {	// cannot find a match in DB
	replyOpenApp(connfd, -1, 0);	// App is not supported
	return -6;
    }
    if (retval < 0) { // internal error
	replyOpenApp(connfd, -4, 0);	// Internal Error
	return -10;
    }

    /* request pvt_port */
    mgrMsg.type = 1;
    pid = getpid();
    mgrMsg.req.pid = pid;

    retval = write(mgrfd, &mgrMsg, sizeof(MgrMsg));
    if (retval < 0) {
	printErr("write to manager server failed");
	replyOpenApp(connfd, -4, 0);	// Internal Error
	return -2;
    }

    /* read port from vAppMgr */
    retval = read(mgrfd, &pvt_port, sizeof(unsigned short));
    if (retval < 0) {
	printErr("read reply from manager server failed");
	replyOpenApp(connfd, -4, 0);	// Internal Error
	return -2;
    }
    if (pvt_port == 0) { // no available pvt_port
	replyOpenApp(connfd, -2, 0);	// no available pvt_port
	return -7;
    }

    /* fork process & create App */
    retval = fork();
    if (retval < 0) {
	printErr("Error: fork process failed");
	replyOpenApp(connfd, -4, 0);	// Internal Error
	return -8;  // die anayway, vAppd will handle port reclaim
	// TODO
    }
    else if (retval > 0) { // parent process --> create App
	/* pre-process */
	close(connfd);
	close(mgrfd);

	/* exec App */
	retval = spawnApp(cmd, args, envs, pvt_port);
	printErr("Process(%d) executing App failed\n", getpid());

	/* exit anyway */
	exit(retval);
    }
    else { // retval == 0, child process --> monitor & reply
	/* pre-process */
	ppid = pid;
	pid = getpid();

	/* execute monitor routine */
	retval = babySitter(ppid, cmd, pvt_port);
    }

    /* child process ONLY */
    /* post-process for babySitter() */
    if (retval == 0) { // success
	if (replyOpenApp(connfd, 0, pvt_port) < 0) { // write failed
	    /* reply to ctrl server failed, kill process */
	    kill(ppid, SIGKILL);
	    return -3;
	}
	else {
	    /* notify vAppMgr */
	    mgrMsg.type = 2;
	    mgrMsg.req.pid = ppid;
	    write(mgrfd, &mgrMsg, sizeof(MgrMsg));  // DONOT matter the result
						    // run anyway
	    return 0;
	}
    }
    else if ((retval == -1) || (retval == -2)) {
	replyOpenApp(connfd, -3, 0); // process creation failed
	printErr("Error: Process(%d) creation failed", ppid);
	return -11;
    }
    else {
	replyOpenApp(connfd, -4, 0);	// Internal Error
	printErr("Error: babySitter(%d) check proc(%d) failed", pid, ppid);
	return -9;
    }
}

/*
#define replyCloseApp(a, b) \
    do { \
	retval = a; \
	repMsg = b; \
	goto REP_CLOSEAPP; \
    } while(0)
*/

#define replyCloseApp(a, b) \
    do { \
	retval = a; \
	REPMsg.retval = b; \
	goto REP_CLOSEAPP; \
    } while(0)

int procCloseApp(int connfd)
{
    CONN_OBJ *connObj = (CONN_OBJ *)&(REQMsg.cmd.close_app);
    pid_t pid;
    //int repMsg = 0;  // msg reply to Ctrl server
    int retval;

    /* read connection information */
    retval = readConnObj(connfd, connObj);  //TODO: simplify it
    if (retval != 0) {
	printErr("read request data from client failed");
	return -3;  // connection error, close w/o reply
    }

    /* notify vAppMgr and get PID of the App */
    mgrMsg.type = 3;
    mgrMsg.req.pvt_port = connObj->pvt_port;

    retval = write(mgrfd, &mgrMsg, sizeof(MgrMsg));
    if (retval < 0) {
	printErr("write to manager server failed");
	replyCloseApp(-2, -2);
    }

    retval = read(mgrfd, &pid, sizeof(pid_t));
    if (retval < 0) {
	printErr("read reply from manager server failed");
	replyCloseApp(-2, -2);
    }

    /* check pid */
    if (pid == 0) { // failed
	printErr("The App requested being killed is not existed");
	replyCloseApp(-5, -1);
    }

    /* kill target process */
    /* TODO: Question: not parent can kill? */
    retval = kill(pid, SIGKILL);
    if (retval < 0) {
	printErr("The App requested being killed is not existed");
	replyCloseApp(-5, -1);
    }
    else {
	REPMsg.retval = 0;
    }

REP_CLOSEAPP:
    /* reply client */
    // TODO: check spec
    //write(connfd, &repMsg, sizeof(int));	// DONOT check retval here
    write(connfd, &REPMsg, sizeof(VAPP_REP));	// DONOT check retval here

    return retval;
}

int procRoutine(int connfd, char *mgrPortStr)
{
    int retval;
    unsigned int type;

    /* connect to vAppMgr */
    mgrfd = connectsock("127.0.0.1", mgrPortStr, "tcp");
    if (mgrfd < 0) {
	printErr("connect to manager server failed");
	retval = -1;
	goto PROC_ROUTINE_EXIT;
    }

    /* notify vAppMgr that new process is forked */
    mgrMsg.type = 0;
    mgrMsg.req.pid = getpid();

    retval = write(mgrfd, &mgrMsg, sizeof(MgrMsg));
    if (retval < 0) {
	printErr("write to manager server failed");
	retval = -2;
	goto PROC_ROUTINE_EXIT;
    }
    // TODO: think about if failed here, what vAppMgr would do?
    // should I trsut cmd from reaper anyway?

    /* read request type first */
    retval = read(connfd, &type, sizeof(unsigned int));
    if (retval != sizeof(unsigned int)) {
	printErr("read request type from client failed");
	retval = -3;
	goto PROC_ROUTINE_EXIT;
    }
    REQMsg.type = type;
    REPMsg.type = type;

    /* process request according to type */
    switch (type)
    {
	case 0:	// REQ_OPEN_APP
	    retval = procOpenApp(connfd);
	    break;
	case 4: // REQ_CLOSE_APP
	    retval = procCloseApp(connfd);
	    break;
	default:    // out of range
	    retval = -4;
    }

PROC_ROUTINE_EXIT:
    /* close connection to vAppMgr */
    close(mgrfd);

    return retval;
}
