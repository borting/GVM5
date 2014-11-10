#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#include "ErrDbg.h"
#include "hive.h"
#include "MgrMsg.h"

#define WAIT_TIME	5000000	// usec (10^-6)
#define	CHECK_INT	250000	// usec (10^-6)

#define LEN	256

MgrMsg mgrMsg;
char execCMD[256];

int extractGene(char *nick, char **args, char *geneDB)
{
    FILE *fd;
    int argc = 0;
    char *obj = NULL;
    size_t obj_len;
    ssize_t len;

    sprintf(execCMD, "awk -v str=\"%s\" \'$1 == str {s = \"\"; for (i = 2; i <= NF; i++) {s = $i; print s}}\' %s 2> /dev/null", nick, geneDB);

    /* exec command */
    fd = popen(execCMD, "r");
    if (fd == NULL)
	return -2;

    /* get return value */
    while((len = getline(&obj, &obj_len, fd)) > 0) {
	obj[len - 1] = '\0';  // remove '\n' at [len-1], [len] is '\0'
	args[argc++] = obj;
	obj = NULL;
    }

    pclose(fd);
    args[argc] = NULL; // append NULL to args

    return argc;
}

/* parse raw command */
int searchGene(char *rawCMD, char **cmd, char **args, char **envs)
{
    int retval;
    char *token;
    int argc;
    int i = 0;

    /* parse first string in rawCMD --> cmd */
    token = strtok(rawCMD, " ");
    if (token == NULL) { // error
	printErr("Error: the requested App's name is an empty string");
	return -3;
    }

    /* check supporting App database */
    /* retrieve args */
    retval = extractGene(token, args, "geneARG_DB");
    if (retval < 0) { // pipe error
	printErr("Error: get requested App's args failed");
	return retval;
    }
    else if (retval == 0) { // No match
        printErr("Error: the requested App is not supported");
	return -1;
    }
    else {  // success, record # of arg
	argc = retval;
	*cmd = args[0];
    }

    /* retrieve envs */
    // TODO: should extract envs require checking app existed?
    retval = extractGene(token, envs, "geneENV_DB");
    if (retval < 0) {
	printErr("Error: get requested App's envs failed");
	/* reclaim allocated memory */
	while (args[i]) { // free args
	    free(args[i]);
	}
	return retval;
    }

    /* parse rest string in rawCMD ans store them to args */
    while((token = strtok(NULL, " "))) {
	args[argc++] = token;
    }

    /* append NULL to args */
    args[argc] = NULL;

    return 0;
}

/* exec App */
int spawnApp(char *cmd, char **args, char **envs, unsigned short pvt_port)
{
    char portStr[16] = {0};
    int i = 0;
    int retval;

    /* export pvt_port to env setting */
    sprintf(portStr, "%d", pvt_port);
    setenv("BROADWAY_DISPLAY", portStr, 1);

    /* export additional env settings */
    while (envs[i]) {
	putenv(envs[i]);
	i++;
    }

    /* call exec() */
    retval = execvp(cmd, args);

    return retval;
}

/* check execution, report vAppMgr, reply ctrl server */
int babySitter(pid_t ppid, char *cmd, unsigned short pvt_port)
{
    int times = WAIT_TIME / CHECK_INT;
    FILE *fd;
    int retval;
    char execCMD[256] = {0};

    printDebug("babySitter(%d) takes care process(%d)\n", getpid(), ppid);

    /* setup exec command */
    sprintf(execCMD, "sh checkAppExec.sh %d %s 2> /dev/null", ppid, cmd);

    /* check execution */
    while (times--) {
	/* wait a period of time */
	usleep(CHECK_INT);

	/* check the execution of parent thread */
	/* exec check script */
	fd = popen(execCMD, "r");
	if (fd == NULL) {   // pipe error
	    retval = -3;
	    break;
	}

	/* get result */
	if (fscanf(fd, "%d", &retval) != 1) {
	    retval = -4;
	    break;
	}

	/* close pipe */
	printDebug("babySitter(%d) times = %d\n", getpid(), times);
	pclose(fd);

	/* check result */
	if (retval == -1) // keep waiting
	    continue;
	else	// 0: success, -2: process died unexpectly
	    break;
    }

    /* notify vAppMgr if App ran successfully & ctrl server is replied */
    if (retval < 0) { // failed, kill parent
	/*
	 * DONOT notify vAppMgr again
	 * let vAppd(root) reaper handler be in charge of the killed process
	 */
#ifndef UNIT_TEST
	kill(ppid, SIGKILL);	// DONOT matter the result
#else
	printf("babySitter(%d) kill process (%d) result = %d\n", 
		getpid(), ppid, kill(ppid, SIGKILL));
#endif
    }

    return retval;
}
