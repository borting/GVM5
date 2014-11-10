#include "CTRL_works.h"
#include "CTRL_data_op.h"

// Externs ====================================================
// ============================================================

// MYSQL 
const char *MYSQL_host = "localhost";
const char *MYSQL_user = "root";
const char *MYSQL_pass = "temp";
const char *MYSQL_dbase = "CTRL_DATA";
const char *MYSQL_socket = NULL;

const unsigned int     MYSQL_port = 0;
const unsigned long    MYSQL_client_flag = 0;

const char *CTRL_PM_TABLE_NAME = "PM_TABLE";
const char *CTRL_VM_TABLE_NAME = "VM_TABLE";

// DNAT
const char 		*DNAT_IP = "192.168.30.1";
const unsigned short    DNAT_PORT = 10000;

// CMND
const unsigned int CMND_PORT = 30010;

// MAIN =======================================================
// ============================================================

int main(){

	// Creat TABLE
	printf( "Generate test  data base\n" );

	printf( "%d\n", DROP_TABLE( CTRL_PM_TABLE_NAME ));
	printf( "%d\n", CREATE_PM_TABLE( ));

	printf( "%d\n", DROP_TABLE( CTRL_VM_TABLE_NAME ));
	printf( "%d\n", CREATE_VM_TABLE( ));

	// PM TEST ============================================
	printf( "Generate PM TEST\n");

	PM_ENTRY PM_temp[8];
	for( int idx = 0 ; idx < 8 ; idx++ ){

		unsigned int ip_base = (192*256+168)<<16;
		PM_temp[idx].PM_IP = ip_base + idx;
		PM_temp[idx].PM_STAT = 0;
		PM_temp[idx].IMG_LEFT = rand()%5+1;
		PM_temp[idx].IMG_USED = 0;
		PM_temp[idx].CPU_LEFT = (rand()%16+1);
		PM_temp[idx].CPU_USED = 0;
		PM_temp[idx].MEM_LEFT = (rand()%16+1)*1024;
		PM_temp[idx].MEM_USED = 0;
	
		if( ADD_PM_ENTRY( &PM_temp[idx] ) == NULL ) printf("1\n");
	}

	// VM test ============================================
	printf( "Generate VM TEST\n");

	VM_ENTRY VM_temp[8];
	unsigned int ip_base = (192*256+168)<<16;
	for( int idx = 0 ; idx < 8 ; idx++ ){

		VM_temp[idx].PM_IP 	= ip_base + idx;
		VM_temp[idx].VM_PUB_IP	= ip_base + idx + 256;
		VM_temp[idx].VM_PVT_IP 	= ip_base + idx + 512;
		VM_temp[idx].VM_CPU	= rand()%5;
		VM_temp[idx].VM_MEM	= (rand()%10)*128;
	
		if( ADD_VM_ENTRY( VM_temp + idx ) == NULL ) printf("1\n");
	}

	// GET PM
	PM_ENTRY	temp_PM;
	int		temp_cpu;
	int		temp_mem;
	unsigned int	temp_ip[4];

	printf("GET PM\n");
	scanf( "%d %d", &temp_cpu, &temp_mem );
	MYSQL_get_PM( &temp_PM, temp_cpu, temp_mem );
	DISP_PM_ENTRY( &temp_PM );

	printf("ADD VM\n");
	VM_ENTRY	temp_VM;
	MYSQL_add_VM( 	&temp_VM, temp_cpu, temp_mem, 
			temp_PM.PM_IP, temp_PM.PM_IP+256, temp_PM.PM_IP+512 );

	printf("GET VM\n");
	scanf( "%d.%d.%d.%d", &temp_ip[0], &temp_ip[1], &temp_ip[2], &temp_ip[3] );	
	temp_ip[0] = temp_ip[0] * 256 + temp_ip[1];
	temp_ip[0] = temp_ip[0] * 256 + temp_ip[2];
	temp_ip[0] = temp_ip[0] * 256 + temp_ip[3];
	MYSQL_get_VM( &temp_VM, temp_ip[0] );
	DISP_VM_ENTRY( &temp_VM );
		
	printf("DEL VM\n");
	MYSQL_del_VM( &temp_VM, temp_ip[0] );

	printf("Heartbeat\n");
	scanf( "%d.%d.%d.%d", &temp_ip[0], &temp_ip[1], &temp_ip[2], &temp_ip[3] );
        temp_ip[0] = temp_ip[0] * 256 + temp_ip[1];
        temp_ip[0] = temp_ip[0] * 256 + temp_ip[2];
        temp_ip[0] = temp_ip[0] * 256 + temp_ip[3];
	
	temp_PM.PM_IP = temp_ip[0];
	MYSQL_heartbeat_PM( &temp_PM, temp_ip[0] );
	MYSQL_heartbeat_PM( &temp_PM, temp_ip[0] );

	return 0;
}
