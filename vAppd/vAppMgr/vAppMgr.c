#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "appList.h"
#include "ErrDbg.h"	/* error output & debug */
#include "MgrMsg.h"
#include "portPool.h"
#include "sockop.h"
#include "vAppMgr.h"

#define poolSize    1000
#define portBase    20000
#define sockqLen    1024

char appList[32] = "appList";
PortPool pPool;

#define LOCK(X)	    pthread_mutex_lock(&X##_LOCK)
#define UNLOCK(X)   pthread_mutex_unlock(&X##_LOCK)

pthread_mutex_t pPool_LOCK = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t appList_LOCK = PTHREAD_MUTEX_INITIALIZER;

int mgrfd;
char* mgrIPStr;
char* mgrPortStr;
char* vAppdPortStr;
char* ctrlIPStr;
char* ctrlPortStr;

int main(int argc, char *argv[])
{
    /* local variable declaration */
    int *connfd;
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);

    pthread_t tid;

    int retval;

    /* input parsing */
    if (argc != 6)
	stderrexit(-1, "Usage: %s mgr_IP mgr_port vAppd_port ctrl_IP ctrl_port\n", argv[0]);

    mgrIPStr = argv[1];
    mgrPortStr = argv[2];
    vAppdPortStr = argv[3];
    ctrlIPStr = argv[4];
    ctrlPortStr = argv[5];

    /* initialization */
    init();

    /* fork vAppd */
    fork_vAppd();

    /* server routine */
    while (1)
    {
	/* wait for connnection */
	retval = accept(mgrfd, (struct sockaddr *) &addr_cln, &sLen);
	if (retval < 0) {
	    stderrexit(-1, "Error: accept()\n");
	}

	/* retrieve connection fd */
	connfd = (int *)malloc(sizeof(int));
	*connfd = retval;

	/* execute handling thread */
	retval = pthread_create(&tid, NULL, reqHandler, (void *)connfd);
	if (retval == 0) {
	    pthread_detach(tid);    // detach thread
	}
	else {
	    close(*connfd); // close client
	    free(connfd);   // free memory
	    printErr("Error: Create thread failed.");
	}
    }

    /* deconstruction */
    cleanup();

    return 0;
}

/* initialization */
void init()
{
    int retval;

    /* port pool */
    retval = newPortPool(&pPool, poolSize, portBase);
    if (retval < 0)
	stderrexit(-1, "create portPool failed");

    /* app list */
    retval = newAppList(appList);
    if (retval < -1)
	stderrexit(-1, "create appList failed");

    /* manager server socket */
    mgrfd = passivesock(mgrPortStr, "tcp", sockqLen);
    if (mgrfd < 0)
	stderrexit(-1, "create manager server failed");
}

/* fork vAppd */
void fork_vAppd()
{
    // use mgrPortStr, vAppdPortStr

}

int procReqPort(int connfd, pid_t pid)
{
    int retval;
    unsigned short pvt_port = 0;

    /* check whether pid in appList */
    //LOCK(appList);
    retval = searchApp(pid, appList);
    //UNLOCK(appList);

    if (retval < -1) { // pipe error
	printErr("Error: search appList failed");
	retval= -5;
	goto REPLY_REQPORT;
    }

    if (retval == -1) { // pid not existed
	printErr("Error: App's pid not in appList ");
	retval = -6;
	goto REPLY_REQPORT;
    }

    /* request pvt_port */
    LOCK(pPool);
    retval = searchPort(&pPool, &pvt_port);
    UNLOCK(pPool);

    if (retval < 0) {
	printErr("Error: No available private port");
	retval = -4;
	goto REPLY_REQPORT;
    }

    /* update app's port */
    printDebug("pvt_port = %hu\n", pvt_port);
    LOCK(appList);
    retval = setAppPort(pid, pvt_port, appList);
    UNLOCK(appList);

    if (retval < 0) { // pipe error
	printErr("Error: set App's port failed");
	retval = -5;

	/* reclaim port */
	LOCK(pPool);
	reclaimPort(&pPool, pvt_port);	/* Do not matter the return */
	UNLOCK(pPool);
	pvt_port = 0;	/* indicate failure ion reply message */
	goto REPLY_REQPORT;
    }

    /* update app's state */
    LOCK(appList);
    retval = setAppState(pid, 1, appList);
    UNLOCK(appList);

    if (retval < 0) { // pipe error
	printErr("Error: set App's state failed");
	retval = -5;

	/* reclaim port */
	LOCK(pPool);
	reclaimPort(&pPool, pvt_port);	/* Do not matter the return */
	UNLOCK(pPool);
	pvt_port = 0;	/* indicate failure ion reply message */
    }

REPLY_REQPORT:
    /* reply pvt_port */
    write(connfd, &pvt_port, sizeof(unsigned short));

    return retval;
}

int procExecApp(pid_t pid)
{
    int retval;

    /* check whether pid in appList */
    //LOCK(appList);
    retval = searchApp(pid, appList);
    //UNLOCK(appList);

    if (retval < -1) { // pipe error
	printErr("Error: search appList failed");
	return -5;
    }

    if (retval == -1) { // pid not existed
	printErr("Error: App's pid not in appList ");
	return -6;
    }

    /* update app state */
    LOCK(appList);
    retval = setAppState(pid, 2, appList);
    UNLOCK(appList);

    if (retval < 0) { // pipe error
	printErr("Error: set App's state failed");
	return -5;
    }

    return 0;
}

int procKillApp(int connfd, unsigned short pvt_port)
{
    int retval;
    pid_t pid = 0;

    /* DONOT searchApp() first, getAppPid() has self-check */

    /* get App's pid using pvt_port */
    //LOCK(appList);
    retval = getAppPid(pvt_port, appList);
    //UNLOCK(appList);

    if (retval < -1) { // pipe error
	printErr("Error: get App's pid failed");
	retval = -5;
    }
    else if (retval == -1) { // App not in List
	printErr("Error: App not in appList");
	retval = -6;
    }
    else {
	pid = retval;
	retval = 0;
	/* update app state */
	/* DO NOT matter the result since we already have pid */
	LOCK(appList);
	setAppState(pid, 3, appList);
	UNLOCK(appList);
    }

    /* reply pid */
    write(connfd, &pid, sizeof(pid_t));

    return retval;
}

int notifyCtrlServer(unsigned short pvt_port)
{
    int retval;
    int connfd;
    char ctrlMsg[64] = {0};
    int len;

    connfd = connectsock(ctrlIPStr, ctrlPortStr, "tcp");
    if (connfd < 0) {
	printErr("Error: Connect to Control server failed");
	return -8;
    }

    len = sprintf(ctrlMsg, "0 5 0 %s %hu", mgrIPStr, pvt_port);

    retval = write(connfd, &ctrlMsg, len);
    if (retval < 0) {
	printErr("Error: Write to Control server failed");
	return -8;
    }

    close(connfd);

    return 0;
}

int procPortReclaim(pid_t pid)
{
    int retval;
    unsigned short pvt_port, state;

    /* DONOT searchApp() first, getAppPort() has self-check */

    /* get App's state */
    //LOCK(appList);
    retval = getAppState(pid, appList);
    //UNLOCK(appList);

    if (retval < -1) { // pipe error
	printErr("Error: get App's state failed");
	return -5;
    }
    else if (retval == -1) { // App not in List
	printErr("Error: App not in appList");
	return -6;
    }
    else {
	state = (unsigned short)retval;
    }

    /* get App's pvt_port */
    //LOCK(appList);
    retval = getAppPort(pid, appList);
    //UNLOCK(appList);

    if (retval < 0) { // pipe error, -1 (not existed) is not possible
	printErr("Error: get App's port failed");
	return -5;
    }
    else {
	pvt_port = (unsigned short)retval;
    }

    /* delete App from appList */
    LOCK(appList);
    delApp(pid, appList); // DONOT matter whether it is successed
    UNLOCK(appList);

    /* reclaim port */
    if (state != 0) {
	LOCK(pPool);
	retval = reclaimPort(&pPool, pvt_port);
	UNLOCK(pPool);
	if (retval < 0)
	    printErr("Error: reclaim port %hu failed", pvt_port);

        /* check state then notify */
        if ((state == 2) | (state == 3)) { /* should notify Ctrl server */
	    retval = notifyCtrlServer(pvt_port);
	    printDebug("%s", "Notified Ctrl server\n");
        }
    }

    return retval;
}

int procForkChild(pid_t pid)
{
    int retval;

    /* check whether pid in appList */
    //LOCK(appList);
    retval = searchApp(pid, appList);
    //UNLOCK(appList);

    if (retval < -1) { // pipe error
	printErr("Error: search appList failed");
	return -5;
    }
    
    if (retval == 0) { // pid existed
	printErr("Warning: App's pid existed ");
	/* delete old one */
	procPortReclaim(pid); // DONOT matter return
	//if ((delApp(pid, appList)) < 0) { // pipe error
	//    printErr("Error: del App from appList failed");
	//    return -5;
	//}
    }

    /* add App to list */;
    LOCK(appList);
    retval = addApp(pid, appList);
    UNLOCK(appList);

    if (retval < 0) { // pipe error
	printErr("Error: Add App to appList failed");
	return -5;
    }

    return 0;
}

/* process client request */
int processReq(int connfd, MgrMsg *reqMsg)
{
    int retval = -3;

    switch (reqMsg->type) {
	case 0:	    /* fork child process */
	    retval = procForkChild(reqMsg->req.pid);
	    break;
	case 1:	    /* request port for creating new App */
	    retval = procReqPort(connfd, reqMsg->req.pid);
	    break;
	case 2:	    /* notify App is created */
	    retval = procExecApp(reqMsg->req.pid);
	    break;
	case 3:	    /* ask App's pid for kill App (request from UI) */
	    retval = procKillApp(connfd, reqMsg->req.pvt_port);
	    break;
	case 4:	    /* post-process after App is closed */
	    retval = procPortReclaim(reqMsg->req.pid);
	    break;
	default:
	    break;
    }

    return retval;
}

/* process routine of request handler */
void *reqHandler(void *fd)
{
    int connfd;
    int retval;
    int readByte;
    MgrMsg *reqMsg;

    reqMsg = malloc(sizeof(MgrMsg));

    /* assign connection fd */
    connfd = *((int *)fd);
    free(fd);

    /* process request */
    while (1) {
	/* reset */
	readByte = 0;

	/* read routine */
	do {
	    retval = read(connfd, reqMsg + readByte, sizeof(MgrMsg) - readByte);

	    if (retval > 0) 
		readByte += retval;
	    else 
		break;
	} while (readByte != sizeof(MgrMsg));
    
	/* check */
	if (retval < 0) {
	    printErr("Error: read()");
	    break;
	}
	else if	((retval == 0) && (readByte == 0)) {/* client close normally */
	    break;
	}
	else if (readByte != sizeof(MgrMsg)) {
	    printErr("Error: read unsufficient bytes");
	    break;
	}
	else {	/* readByte == sizeof(MgrMsg)), client request normally */
	    /* process request and reply */
	    retval = processReq(connfd, reqMsg);
	    //if (retval != 0) {
	    if ((retval != 0) && (reqMsg->type != 4)){	// handler never die
		printErr("Error: process request failed");
		break;
	    }
	}
    }

    /* cleanup */
    close(connfd);
    free(reqMsg);

    pthread_exit(0);

    return 0;
}

/* deconstruction */
void cleanup()
{
    int retval;

    /* close manager server */
    close(mgrfd);

    /* delete port pool */
    delPortPool(&pPool);

    /* delete app list */
    retval = delAppList(appList);
    if (retval != 0)
	stderrexit(-2, "delete appList failed");
}
