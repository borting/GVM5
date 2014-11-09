#include <stdio.h>
#include <stdlib.h>

#include "iptables_ctrl.h"
#include "portarray.h"

extern PortArray pArray;

/**
 * @brief use popen() to execute a shell script
 *
 * @param type iptables command type
 * @param iface network interface
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval >0 exec result: failed
 * @retval 0 exec result: success
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
static inline int configDNATrule(char type, char *iface, 
	unsigned short pub_port, char *pvt_ip, unsigned short pvt_port)
{
    FILE *fd;
    char cmd[256] = {0};
    int retval = 0;

    /* create command */
    /* redirect stderr to /dev/null */
    sprintf(cmd, "sh configDNATrule.sh %c %s %hd %s %hd 2> /dev/null", 
	    type, iface, pub_port, pvt_ip, pvt_port);

    /* exec command */
    fd = popen(cmd, "r");
    if (fd == NULL)
	return -1;

    /* get return value */
    /* Shell execution always has a return */
    if (fscanf(fd, "%d", &retval) != 1) // shell script not found or other error
	retval = -2;

    pclose(fd);
    return retval;
}

//// init iptables
//int initIPtables(char *filepath);
//
//// clean iptables
//int cleanIPtables();
//
//// save iptables
//int saveIPtables(char *filepath);
//
//// restore iptables
//int restoreIPtables(char *filepath);

/**
 * @brief add new DNAT rule to iptables
 *
 * @param iface network interface
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval 2 bad IP/port be specified
 * @retval 0 add success
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int addDNATrule(char *iface, unsigned short pub_port, 
	char *pvt_ip, unsigned short pvt_port)
{
    return configDNATrule('A', iface, pub_port, pvt_ip, pvt_port);
}

/**
 * @brief check whether DNAT rule in iptables
 *
 * @param iface network interface
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval 2 bad IP/port be specified
 * @retval 1 rule not existed
 * @retval 0 rule existed
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int checkDNATrule(char *iface, unsigned short pub_port, 
	char *pvt_ip, unsigned short pvt_port)
{
    return configDNATrule('C', iface, pub_port, pvt_ip, pvt_port);
}

/**
 * @brief delete DNAT rule in iptables
 *
 * @param iface network interface
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval 2 bad IP/port be specified
 * @retval 1 no match rule be found
 * @retval 0 delete success
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int delDNATrule(char *iface, unsigned short pub_port, 
	char *pvt_ip, unsigned short pvt_port)
{
    return configDNATrule('D', iface, pub_port, pvt_ip, pvt_port);
}

/**
 * @brief find the local port number of a DNAT rule
 *
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval 0 local port found
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 * @retval -3 local port not found
 */
int findPubPort(unsigned short *pub_port, 
	char *pvt_ip, unsigned short pvt_port)
{
    FILE *fd;
    char cmd[256] = {0};

    /* create command */
    sprintf(cmd, "sh findPubPort.sh %s %hd 2> /dev/null", pvt_ip, pvt_port);

    /* exec command */
    fd = popen(cmd, "r");
    if (fd == NULL)
	return -1;

    /* get public port */
    if (fscanf(fd, "%hd", pub_port) == 1) {
	pclose(fd);
	if (*pub_port)
	    return 0;
	else
	    return -3;
    }
    else {	// shell execution doesn't return expected value
	pclose(fd);
	return -2;
    }
}

/**
 * @brief find the forward IP and port number of a DNAT rule
 *
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval 0 forward IP & port found
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 * @retval -3 forward IP & port not found
 */
int findPvtIPPort(unsigned short pub_port, 
	char *pvt_ip, unsigned short *pvt_port)
{
    FILE *fd;
    char cmd[256] = {0};

    /* create command */
    sprintf(cmd, "sh findPvtIPPort.sh %hd 2> /dev/null", pub_port);

    /* exec command */
    fd = popen(cmd, "r");
    if (fd == NULL)
	return -1;

    /* get return value */
    if (fscanf(fd, "%s %hd", pvt_ip, pvt_port) == 2) {
	pclose(fd);
	if (*pvt_port)
	    return 0;
	else
	    return -3;
    }
    else {	// shell execution doesn't return expected value
	pclose(fd);
	return -2;
    }
}

/**
 * @brief list DNAT rules set for a particular IP address
 *
 * @param pvt_ip forward destination IP address
 * @retval fd stdout of list execution
 */
static inline FILE *listIPinDNAT(char *pvt_ip)
{
    FILE *fd;
    char cmd[256] = {0};

    /* create command */
    sprintf(cmd, "sh listIPinDNAT.sh %s 2> /dev/null", pvt_ip);

    /* exec command */
    fd = popen(cmd, "r");

    return fd;
}

/**
 * @brief delete DNAT rules set for a particular IP address
 *
 * @param iface network interface
 * @param pvt_ip forward destination IP address
 * @retval count number of deleted rules
 * @retval -1 popen failed
 */
int delIPinDNAT(char *iface, char *pvt_ip)
{
    FILE *fd;
    int count = 0;
    unsigned short pub_port, pvt_port;

    /* list all DNAT rules set for a particular IP */
    fd = listIPinDNAT(pvt_ip);
    if (fd == NULL)
	return -1;

    /* TODO:
     * 1. how to check script not found
     * 2. how to check return value of reclaimPort
     * */
    while (fscanf(fd, "%hd %hd", &pub_port, &pvt_port) == 2) {
	/* delete rules */
	/* No need to check, listIPinDNAT() list all port pairs in table  */
	delDNATrule(iface, pub_port, pvt_ip, pvt_port);
	/* reclaim port */
	reclaimPort(&pArray, pub_port);
	count++;
    }

    /* close pipe created by listIPinDNAT() */
    pclose(fd);

    return count;
}

