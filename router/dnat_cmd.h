#ifndef _DNAT_CMD_H_
#define _DNAT_CMD_H_

typedef struct
{
    int type;	// Request or Reply command type
    unsigned char pvt_ip[4];	// private ip address
    unsigned short pvt_port;	// private port number
    unsigned short pub_port;	// public port number
} DNAT_CMD;

/* DNAT_CMD Request command type */
#define REQ_AppStart	    0	/* User opens an App */
#define REQ_AppClose	    1	/* User closes an App */
#define REQ_VMStart	    2	/* User powers on a VM */
#define REQ_VMClose	    3	/* User powers off a VM */
#define REQ_SearchPrivate   4	/* Search private IP:port associated to a public port in iptables */
#define REQ_SearchPublic    5	/* Search public port associated to a private IP:port in iptables */

/* DNAT_CMD Reply command type */
#define REP_OK		    0	/* Success */
#define REP_IPinList	    -1	/* Request IP is already in list */
#define REP_IPoutList	    -2	/* Request IP is not in list */
#define REP_WrongSub	    -3	/* Request IP is not in the subnet of private network */
#define REP_DblReclaim	    -9	/* The public port has already be reclaimed */
#define REP_NotReclaim	    -10	/* The public port has not been reclaimed */
#define REP_EmptyPool	    -11	/* No available public port */
#define REP_NoPrivarte	    -17	/* Cannot find a private IP:port associated to request public port */
#define REP_NoPublic	    -18	/* Cannot find a public port forward packets to request private IP:port */
#define REP_RuleExist	    -19	/* iptables already has a rule associated to request private IP:port */
#define REP_RouterError	    -32	/* Router interal operation failure*/
#define REP_REQTypeError    -64	/* Request type is not defined */

#endif /* _DNAT_CMD_H_ */
