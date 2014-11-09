#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>

#include "iplist.h"

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
static inline int execCMD(char *script, int argn, ...)
{
    /* exec command */
    FILE *fd;
    char cmd[256] = {0};

    /* variable argument lists */
    va_list args;
    char *arg;

    /* create command */
    int index;
    int i;

    int retval;

    /* create command */
    index = sprintf(cmd, "sh %s", script);

    /* parse function arguments */
    va_start(args, argn);
    for (i = 0; i < argn; i++) {
	arg = va_arg(args, char *);
	index += sprintf(cmd+index, " %s", arg);
    }
    va_end(args);

    /* redirect stderr to /dev/null */
    sprintf(cmd+index, " 2> /dev/null");

    /* exec command */
    fd = popen(cmd, "r");
    if (fd == NULL)
	return -1;

    /* get return value */
    if (fscanf(fd, "%d", &retval) != 1) 
	retval = -2;

    pclose(fd);
    return retval;
}

/**
 * @brief validate whether input IP is in the same subnet
 *
 * @param ip input ip address
 * @param subnet subnet
 * @param mask network mask
 * @retval 0 success
 * @retval -1 failure
 */
int validateIP(unsigned char *ip, unsigned char *subnet, unsigned char *mask)
{
    int i;

    /* validate IP format */
    for (i = 0; i < 4; i ++)
	if ((ip[i] & mask[i]) != subnet[i])
	    return -1;

    return 0;
}

/**
 * @brief create a file for saving IP list 
 *
 * @param filename file name
 * @retval 1 file already existed but truncated as new
 * @retval 0 new file create
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int createIPList(char *filename)
{
    return execCMD("createIPList.sh", 1, filename);
}

/** 
 * @brief delete file saving IP list
 *
 * @param filename file name
 * @retval 1 file not existed
 * @retval 0 file deleted
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int deleteIPList(char *filename)
{
    return execCMD("deleteIPList.sh", 1, filename);
}

/** 
 * @brief add a new IP to IPList
 *
 * @param ip ip address
 * @param filename file name
 * @retval 1 ip already in list
 * @retval 0 success
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int addIPtoList(char *ip, char *filename)
{
    return execCMD("addIPtoList.sh", 2, ip, filename);
}

/** 
 * @brief check whether an IP in IPList
 *
 * @param ip ip address
 * @param filename file name
 * @retval 1 ip not in list
 * @retval 0 ip in list
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int checkIPinList(char *ip, char *filename)
{
    return execCMD("checkIPinList.sh", 2, ip, filename);
}

/** 
 * @brief delete an IP in IPList
 *
 * @param ip ip address
 * @param filename file name
 * @retval 1 ip not in list
 * @retval 0 ip deleted
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int delIPinList(char *ip, char *filename)
{
    return execCMD("delIPinList.sh", 2, ip, filename);
}

