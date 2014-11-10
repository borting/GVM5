#ifndef CTRL_DATA_OP
#define CTRL_DATA_OP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <my_global.h>
#include <mysql.h>
/*
#ifndef CTRL_MYSQL_CONN

#define MYSQL_host "localhost"
#define MYSQL_user "root"
#define MYSQL_pass "temp"
#define MYSQL_dbase "CTRL_DATA"
#define MYSQL_port 0
#define MYSQL_socket NULL
#define MYSQL_client_flag 0

#define CTRL_PM_TABLE_NAME "PM_TABLE"
#define CTRL_VM_TABLE_NAME "VM_TABLE"

#endif
*/
#ifndef BUFF_SIZE
#define BUFF_SIZE 256
#endif

// Connection
const extern char             *MYSQL_host;
const extern char             *MYSQL_user;
const extern char             *MYSQL_pass;
const extern char             *MYSQL_dbase;
const extern char             *MYSQL_socket;

const extern unsigned int     MYSQL_port;
const extern unsigned long    MYSQL_client_flag;

const extern char *CTRL_PM_TABLE_NAME;
const extern char *CTRL_VM_TABLE_NAME;

// General ====================================================
int CREATE_DATABASE( const char *name );
int DROP_TABLE( const char *name );

// PM TABLE ===================================================
typedef struct{

	unsigned int 	PM_IP;
	unsigned short	PM_STAT;

	unsigned short	IMG_LEFT;
	unsigned short	IMG_USED;
	
	unsigned short	CPU_LEFT;
	unsigned short	CPU_USED;

	unsigned short	MEM_LEFT;
	unsigned short	MEM_USED;

} PM_ENTRY;

int CREATE_PM_TABLE( );

PM_ENTRY *DISP_PM_ENTRY( PM_ENTRY *data );

PM_ENTRY *GET_PM_ENTRY( PM_ENTRY *data, const unsigned int PM_IP ) ;
PM_ENTRY *SET_PM_ENTRY( PM_ENTRY *data, const unsigned int PM_IP ) ;
PM_ENTRY *DEL_PM_ENTRY( PM_ENTRY *data, const unsigned int PM_IP ) ;
const PM_ENTRY *ADD_PM_ENTRY( const PM_ENTRY *data ) ;

PM_ENTRY *GET_PM_AVAILABLE( PM_ENTRY *data, unsigned int REQ_CPU, unsigned int REQ_MEM );
int DECREASE_PM_STAT();

// VM TABLE ===================================================
typedef struct{

	unsigned int	PM_IP;
	unsigned int	VM_PUB_IP;
	unsigned int	VM_PVT_IP;

	unsigned short	VM_CPU;
	unsigned short	VM_MEM;

} VM_ENTRY;

int CREATE_VM_TABLE( );

VM_ENTRY *DISP_VM_ENTRY( VM_ENTRY *data );

VM_ENTRY *GET_VM_ENTRY( VM_ENTRY *data, const unsigned int VM_PUB_IP );
VM_ENTRY *SET_VM_ENTRY( VM_ENTRY *data, const unsigned int VM_PUB_IP );
VM_ENTRY *DEL_VM_ENTRY( VM_ENTRY *data, const unsigned int VM_PUB_IP );
const VM_ENTRY *ADD_VM_ENTRY( const VM_ENTRY *data ) ;

#endif
