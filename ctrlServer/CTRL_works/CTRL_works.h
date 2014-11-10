#ifndef CTRL_WORK
#define CTRL_WORK

#include "CTRL_data_op.h"
#include "ds_tcp_sock.h"

// WEB =========================================================
// =============================================================

const extern char		*WEB_IP;
const extern unsigned short	WEB_PORT;

int WEB_APP_CLOSE( unsigned short VM_WWW_PORT );

// VAPPD =======================================================
// =============================================================

const extern unsigned short	VAPPD_PORT;

typedef struct{
	
	unsigned int CMD_TYPE;
	unsigned int CMD_LEN;
	char	CMD[BUFF_SIZE];

}VAPPD_CMD;

typedef struct{

	unsigned int CMD_TYPE;
	unsigned char VM_PVT_IP[4];
	unsigned short VM_PVT_PORT;

}VAPPD_CMD_CLOSE;

typedef struct{
	
	unsigned int CMD_TYPE;
	int CMD_RES;
	char VM_PUB_IP[4];
	unsigned short VM_PUB_PORT;

}VAPPD_RES;

int VAPPD_APP_START( unsigned int VM_PVT_IP, char *CMD );
int VAPPD_APP_CLOSE( unsigned int VM_PVT_IP, unsigned short VM_PVT_PORT );

// MYSQL =======================================================
// =============================================================

const extern char             *MYSQL_host;
const extern char             *MYSQL_user;
const extern char             *MYSQL_pass;
const extern char             *MYSQL_dbase;
const extern char             *MYSQL_socket;

const extern unsigned int     MYSQL_port;
const extern unsigned long    MYSQL_client_flag;

const extern char *CTRL_PM_TABLE_NAME;
const extern char *CTRL_VM_TABLE_NAME;

PM_ENTRY *MYSQL_get_PM( PM_ENTRY *PM, int NUM_CPU, int NUM_MEM );
PM_ENTRY *MYSQL_heartbeat_PM( PM_ENTRY *PM, unsigned int PM_IP );

VM_ENTRY *MYSQL_get_VM( VM_ENTRY *VM, unsigned int PUB_IP );
VM_ENTRY *MYSQL_del_VM( VM_ENTRY *VM, unsigned int PUB_IP );
const VM_ENTRY *MYSQL_add_VM( VM_ENTRY *VM, int NUM_CPU, int NUM_MEM, 
			unsigned int PHY_IP, unsigned int PVT_IP, unsigned int PUB_IP );

// DNAT ========================================================
// =============================================================

#define REQ_AppStart        0   /* User opens an App */
#define REQ_AppClose        1   /* User closes an App */
#define REQ_VMStart         2   /* User powers on a VM */
#define REQ_VMClose         3   /* User powers off a VM */
#define REQ_AppGetPVTPort   4   /* Get private port of an App*/

typedef struct{

	int 		TYPE;
	unsigned char	PUB_IP[4];
	unsigned short	PUB_PORT;
	unsigned short	WWW_PORT;

} DNAT_REQ;

const extern char 		*DNAT_IP;
const extern unsigned short	DNAT_PORT;

int DNAT_VM_Start( unsigned int PUB_IP );
int DNAT_VM_Close( unsigned int PUB_IP );

int DNAT_APP_Start( unsigned int PUB_IP, unsigned short PVT_PORT );
int DNAT_APP_Close( unsigned int PUB_IP, unsigned short PUB_PORT );

int DNAT_APP_GET_PVT_PORT( 	unsigned short WWW_PORT, 
				unsigned int *PUB_IP, unsigned short *PUB_PORT); 

// CMND ========================================================
// =============================================================

const extern unsigned int	CMND_PORT;

VM_ENTRY *CMND_VM_START( VM_ENTRY *data, unsigned int CMND_IP, int NUM_CPU, int NUM_MEM );
VM_ENTRY *CMND_VM_SHUTDOWN( VM_ENTRY *data, unsigned int CMND_IP, unsigned int VM_PUB_IP );
VM_ENTRY *CMND_VM_REBOOT( VM_ENTRY *data, unsigned int CMND_IP, unsigned int VM_PUB_IP );
#endif
