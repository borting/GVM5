#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "appList.h"

/**
 * @brief use popen() to execute a shell script
 *
 * @param script script name
 * @param argn number of variables
 * @retval 1 exec result: failed
 * @retval 0 exec result: success
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int execCMD(char *script, int argn, ...)
{
    /* exec command */
    FILE *fd;
    char cmd[256] = {0};

    /* variable argument lists */
    va_list args;
    int int_arg;
    char *char_arg;

    /* create command */
    int index;
    int i;

    int retval;

    /* create command */
    index = sprintf(cmd, "sh %s", script);

    /* parse integer (argn - 1) */
    va_start(args, argn);
    for (i = 0; i < argn - 1; i++) {
	int_arg = va_arg(args, int);
	index += sprintf(cmd+index, " %d", int_arg);
    }

    /* parese filename */
    char_arg = va_arg(args, char *);
    index += sprintf(cmd+index, " %s", char_arg);

    va_end(args);

    /* redirect stderr to /dev/null */
    sprintf(cmd+index, " 2> /dev/null");

    /* exec command */
    fd = popen(cmd, "r");
    if (fd == NULL)
	return -2;

    /* get return value */
    if (fscanf(fd, "%d", &retval) != 1) 
	retval = -3;

    pclose(fd);
    return retval;
}

/* create app list */
int newAppList(char *filename)
{
    return execCMD("newAppList.sh", 1, filename);
}

/* delete app list */
int delAppList(char *filename)
{
    return execCMD("delAppList.sh", 1, filename);
}

/* add new App to list */
int addApp(pid_t pid, char *filename)
{
    return execCMD("addApp.sh", 4, pid, 0, 0, filename);
}

/* delete App from list */
int delApp(pid_t pid, char *filename)
{
    return execCMD("delApp.sh", 2, pid, filename);
}

/* get the state of App */
int getAppState(pid_t pid, char *filename)
{
    return execCMD("getAppState.sh", 2, pid, filename);
}

/* set the state of App */
int setAppState(pid_t pid, ushort state, char *filename)
{
    return execCMD("setAppState.sh", 3, pid, state, filename);
}

/* get the pvt_port of App */
int getAppPort(pid_t pid, char *filename)
{
    return execCMD("getAppPort.sh", 2, pid, filename);
}

/* set the pvt_port of App */
int setAppPort(pid_t pid, ushort pvt_port, char *filename)
{
    return execCMD("setAppPort.sh", 3, pid, pvt_port, filename);
}

/* get the pid of App by pvt_port */
int getAppPid(ushort pvt_port, char *filename)
{
    return execCMD("getAppPid.sh", 2, pvt_port, filename);
}

/* check App existed */
int searchApp(pid_t pid, char *filename)
{
    return execCMD("searchApp.sh", 2, pid, filename);
}

