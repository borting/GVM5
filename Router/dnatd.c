#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "dnat_cmd.h"
#include "iplist.h"
#include "iptables_ctrl.h"
#include "portarray.h"
#include "sockop.h"

#define qLength	    1024
#define portMAX	    9000
#define portBase    20000

/* print error then exit */
#define stderrexit(errcode,format,arg...) \
    do { \
	fprintf(stderr, format "\n",##arg); \
	exit(errcode); \
    } while (0)

/* print to stderr */
#define printErr(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__);

/* print for debug */
#ifdef DEBUG
#define DEBUG_FLAG 1
#else
#define DEBUG_FLAG 0
#endif

#define printDebug(fmt, ...) \
    do { \
	if (DEBUG_FLAG) fprintf(stderr, "%s:%d:%s(): " \
	    fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); \
    } while(0)

/* initial once variables */
//char IPList[128] = {0};
char IPList[128] = "IPListfile";
unsigned char subnet[4] = {192, 168, 20, 0};
unsigned char mask[4] = {255, 255, 255, 0};
//char iface[16] = {0};
char *iface = "eth0";
//char ctrlPort[8] = {0}
char *ctrlPort;

/* global variables */
char ipStr[16] = {0};
PortArray pArray;

inline void digit2str(unsigned char *ip, char *ipStr)
{
    sprintf(ipStr, "%hhu.%hhu.%hhu.%hhu", ip[0], ip[1], ip[2], ip[3]);
}

inline void str2digit(unsigned char *ip, char *ipStr)
{
    sscanf(ipStr, "%hhu.%hhu.%hhu.%hhu", &ip[0], &ip[1], &ip[2], &ip[3]);
}

int openApp(DNAT_CMD *req)
{
    int retval;
    unsigned short *pub_port = &req->pub_port;
    unsigned short *pvt_port = &req->pvt_port;
    unsigned short tempPort;
    char tempIP[16] = {0};

    /* clear public ports */
    *pub_port = 0;

    /* transfer ip digits to string */
    digit2str(req->pvt_ip, ipStr);

    /* check whether ip in list */
    retval = checkIPinList(ipStr, IPList);
    switch (retval) {
	case 0:	    /* private IP in list */
	    break;  /* check private port */
	case 1:	    /* private IP is not in list */
	    printErr("Request IP %s is not in list", ipStr);
	    return REP_IPoutList;
	default:
	    return REP_RouterError;
    }

    /* check private IP & port are not in iptables */
    retval = findPubPort(pub_port, ipStr, *pvt_port);
    switch (retval) {
	case -3:    /* rule not existed, OK */
	    break;
	case 0:	    /* old rule not clean */
	    printErr("Iptables already has a rule %hd --> %s:%hd associated to request private IP:port",
		    *pub_port, ipStr, *pvt_port);
	    return REP_RuleExist;
	default:
	    return REP_RouterError;
    }

    /* search available public port */
    /* retval -2: never occured in this program */
    retval = searchPort(&pArray, pub_port);
    if (retval < 0) { // retval = -1;
	printErr("No available public port to map %s:%hd", ipStr, *pvt_port);
	return REP_EmptyPool;
    }

    /* check public port is not used in iptables */
    retval = findPvtIPPort(*pub_port, tempIP, &tempPort);
    switch (retval) {
	case -3:    /* port is not used in iptables, OK */
	    break;
	case 0:	    /* port ha not been reclaimed */
	    printErr("Public port %hd has not been reclaimed", *pub_port);
	    return REP_NotReclaim;
	default:
	    return REP_RouterError;
    }

    /* add new dnat rules */
    retval = addDNATrule(iface, *pub_port, ipStr, *pvt_port);
    /* retval 2: Never happened, since IP & port are checked */
    if (retval < 0)
	return REP_RouterError;
    else
	return REP_OK;
}

int closeApp(DNAT_CMD *req)
{
    int retval;
    unsigned short *pub_port = &req->pub_port;
    unsigned short *pvt_port = &req->pvt_port;

    /* clear public ports */
    *pub_port = 0;

    /* transfer ip digits to string */
    digit2str(req->pvt_ip, ipStr);

    /* check whether ip in list */
    retval = checkIPinList(ipStr, IPList);
    switch (retval) {
	case 0:	    /* private IP in list */
	    break;  /* check rule existed */
	case 1:	    /* private IP is not in list */
	    printErr("Request IP %s is not in list", ipStr);
	    return REP_IPoutList;
	default:
	    return REP_RouterError;
    }

    /* search public port */
    retval = findPubPort(pub_port, ipStr, *pvt_port);
    switch (retval) {
	case 0:	/* success */
	    break;  /* execute delete DNAT rule */
	case -3: /* search failed */
	    printErr("Cannot find a public port forward packets to %s:%hd", 
		    ipStr, *pvt_port);
	    return REP_NoPublic;
	default:
	    return REP_RouterError;
    }

    /* delete DNAT rule */
    retval = delDNATrule(iface, *pub_port, ipStr, *pvt_port);
    /* retval 1: rule has been search by findPubPort(), so rule is existed
     * retval 2: ip & port has been validated
     */
    if (retval < 0)
	return REP_RouterError;

    /* reclaim public port */
    retval = reclaimPort(&pArray, *pub_port);
    /* retval -2: pub_port is retrieved from iptables, so must in port range */
    if (retval < 0) {	// retval = -1
	printErr("Public port %hd has already been reclaimed", *pub_port);
	return REP_DblReclaim;
    }

    return REP_OK;
}

int poweronVM(DNAT_CMD *req)
{
    int retval;

    /* clear public & private ports */
    req->pub_port = 0;
    req->pvt_port = 0;

    /* check whether request ip address is in private network */
    retval = validateIP(req->pvt_ip, subnet, mask);
    if (retval < 0) {
	printErr("Request IP %s is not in the subnet of private network", 
		ipStr);
	return REP_WrongSub;
    }

    /* transfer ip digits to string */
    digit2str(req->pvt_ip, ipStr);

    /* add request ip address to IPList */
    retval = addIPtoList(ipStr, IPList);
    switch (retval) {
	case 0:
	    return REP_OK;
	case 1:
	    printErr("Request IP %s is already in list", ipStr);
	    return REP_IPinList;
	default:
	    return REP_RouterError;
    }
}

int poweroffVM(DNAT_CMD *req)
{
    int retval;

    /* clear public & private ports */
    req->pub_port = 0;
    req->pvt_port = 0;

    /* transfer ip digits to string */
    digit2str(req->pvt_ip, ipStr);

    /* delete ip address in IPList */
    /* no need to validate IP in advance since wrong IP is not in list */
    retval = delIPinList(ipStr, IPList);
    switch (retval) {
	case 0:	/* delete success */
	    break;  /* delete DNAT rules */
	case 1: /* target not in list */
	    printErr("Request IP %s is not in %s", ipStr, IPList);
	    return REP_IPoutList;
	default:
	    return REP_RouterError;
    }

    /* delete all rules set for this ip address */
    /* delIPinDNAT() also calls reclaimPort() */
    retval = delIPinDNAT(iface, ipStr);

    if (retval < 0)
	return REP_RouterError;
    else
	return retval;	/* return numbers of closed (port >= 0) */
}

int searchPvtIPPort(DNAT_CMD *req)
{
    int retval;

    /* clear private IP & port */
    req->pvt_port = 0;
    str2digit(req->pvt_ip, "0.0.0.0");

    /* search iptables */
    retval = findPvtIPPort(req->pub_port, ipStr, &(req->pvt_port));
    switch (retval) {
	case 0:
	    /* transfer ip string to digits */
	    str2digit(req->pvt_ip, ipStr); 
	    return REP_OK;
	case -3:
	    printErr("Cannot find a private IP:port associated to public port %hd", req->pub_port);
	    return REP_NoPrivarte;
	default:
	    return REP_RouterError;
    }
}

int searchPubPort(DNAT_CMD *req)
{
    int retval;

    /* clear public port */
    req->pub_port = 0;

    /* transfer ip digits to string */
    digit2str(req->pvt_ip, ipStr);

    /* check whether ip in list */
    retval = checkIPinList(ipStr, IPList);
    switch (retval) {
	case 0:	    /* private IP in list */
	    break;  /* check rule existed */
	case 1:	    /* private IP is not in list */
	    printErr("Request IP %s is not in list", ipStr);
	    return REP_IPoutList;
	default:
	    return REP_RouterError;
    }

    /* search iptables */
    retval = findPubPort(&(req->pub_port), ipStr, req->pvt_port);
    switch (retval) {
	case 0:
	    return REP_OK;
	case -3:
	    printErr("Cannot find a public port forward packets to %s:%hd", 
		    ipStr, req->pvt_port);
	    return REP_NoPublic;
	default:
	    return REP_RouterError;
    }
}

inline int procRequest(DNAT_CMD *req)
{
    int retval;

    switch (req->type) {
	case REQ_AppStart:	/* User opens an App */
	    retval = openApp(req);
	    break;
	case REQ_AppClose:	/* User closes an App */
	    retval = closeApp(req);
	    break;
	case REQ_VMStart:	/* User powers on a VM */
	    retval = poweronVM(req);
	    break;
	case REQ_VMClose:	/* User powers off a VM */
	    retval = poweroffVM(req);
	    break;
	case REQ_SearchPrivate:	/* Search the private IP & port of a DNAT rule*/
	    retval = searchPvtIPPort(req);
	    break;
	case REQ_SearchPublic:	/* Search the public port of a DNAT rule*/
	    retval = searchPubPort(req);
	    break;
	default: /* Request type is not defined */
	    retval = REP_REQTypeError;
    }

    /* fill reply type */
    req->type = retval;

    return retval;
}

int main(int argc, char *argv[])
{
    int retval;
    DNAT_CMD cmd;
    //int *type = &cmd.type;
    //unsigned char *pvt_ip = cmd.pvt_ip;
    //unsigned short *pub_port = &cmd.pub_port;
    //unsigned short *pvt_port = &cmd.pvt_port;
    
    /* Socket Connection */
    int sockfd, connfd; /* socket descriptor */
    struct sockaddr_in addr_cln;
    socklen_t sLen = sizeof(addr_cln);

    /**************************************************************/
    /* input parsing */
    /**************************************************************/
    if (argc != 2)
	stderrexit(-1, "Usage: %s port\n", argv[0]);
    else
	ctrlPort = argv[1];

    /**************************************************************/
    /* initialization */
    /**************************************************************/
    /* port array */
    retval = newPortArray(&pArray, portMAX, portBase);
    if (retval < 0)
	stderrexit(-1, "create port array failed");

    /* iplist */
    retval = createIPList(IPList);
    if (retval < 0)
	stderrexit(-1, "create IPList failed");

    /* create server socket */
    /* TODO: restrict to control server's IP */
    sockfd = passivesock(ctrlPort, "tcp", qLength);

    /**************************************************************/
    /* process routine */
    /* single thread process */
    /**************************************************************/
    while(1)
    {
	/* waiting for connection */
	connfd = accept(sockfd, (struct sockaddr *) &addr_cln, &sLen);
	if (connfd == -1)
	    stderrexit(-1, "Error: accept()\n");

	/* TODO: set read() timeout */

	/* read request from ctrl server */
	retval = read(connfd, &cmd, sizeof(DNAT_CMD));
	if (retval != sizeof(DNAT_CMD)) {
	    printErr("DNAT_CMD read failed, read %d bytes", retval);
	    close(connfd);
	    continue;
	}

	/* exec handle routine */
	procRequest(&cmd);

	/* debug */
	printf("Reply: \n");
	printf("Reply Code   : %d\n", cmd.type);
	digit2str(cmd.pvt_ip, ipStr);
	printf("private IP   : %s\n", ipStr);
	printf("public port  : %hd\n", cmd.pub_port);
	printf("private port : %hd\n", cmd.pvt_port);
	printf("=========================================================\n");

	/* write reply back to control server */
	retval = write(connfd, &cmd, sizeof(DNAT_CMD));
	if (retval != sizeof(DNAT_CMD)) {
	    printErr("DNAT_CMD write failed, write %d bytes", retval);
	}

	/* close connection */
	close(connfd);
    }

    /**************************************************************/
    /* clean up routine */
    /**************************************************************/

    /* close server port */
    close(sockfd);

    /* port array */
    delPortArray(&pArray);

    /* iplist */
    retval = deleteIPList(IPList);
    if (retval < 0)
	stderrexit(-2, "delete IPList failed");

    return 0;
}
