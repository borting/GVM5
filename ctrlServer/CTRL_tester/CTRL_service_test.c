#include <stdio.h>
#include "CTRL_data.h"
#include "CTRL_service.h"

char             *MYSQL_host 	= "localhost";
char             *MYSQL_user	= "root";
char             *MYSQL_pass 	= "temp";
char             *MYSQL_dbase	= "CTRL_DATA";
unsigned int     MYSQL_port 	= 0;
char             *MYSQL_socket 	= NULL;
unsigned long    MYSQL_client_flag = 0;

char *CTRL_PM_TABLE_NAME = "PM_TABLE";
char *CTRL_VM_TABLE_NAME = "VM_TABLE";

int main(){

	printf( "%d\n", DROP_TABLE( CTRL_PM_TABLE_NAME ));
	printf( "%d\n", CREATE_PM_TABLE( ));

	printf( "%d\n", DROP_TABLE( CTRL_VM_TABLE_NAME ));
	printf( "%d\n", CREATE_VM_TABLE( ));

	// PM TEST ============================================

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

	// ====================================================	

	// Get input
	unsigned int REQ_CPU, REQ_MEM;
	scanf("%d %d", &REQ_CPU, &REQ_MEM);
	CTRL_CREATE_VM( REQ_CPU, REQ_MEM );

	scanf("%d %d", &REQ_CPU, &REQ_MEM);
	CTRL_CREATE_VM( REQ_CPU, REQ_MEM );
	
	int ip_temp[4];
	scanf("%d.%d.%d.%d",ip_temp,ip_temp+1,ip_temp+2,ip_temp+3);
	ip_temp[0] = ip_temp[0]*256 + ip_temp[1];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[2];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[3];
	CTRL_REBOOT_VM( ip_temp[0] );

	scanf("%d.%d.%d.%d",ip_temp,ip_temp+1,ip_temp+2,ip_temp+3);
	ip_temp[0] = ip_temp[0]*256 + ip_temp[1];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[2];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[3];
	CTRL_REBOOT_VM( ip_temp[0] );

	scanf("%d.%d.%d.%d",ip_temp,ip_temp+1,ip_temp+2,ip_temp+3);
        ip_temp[0] = ip_temp[0]*256 + ip_temp[1];
        ip_temp[0] = ip_temp[0]*256 + ip_temp[2];
        ip_temp[0] = ip_temp[0]*256 + ip_temp[3];
        CTRL_SHUTDOWN_VM( ip_temp[0] );

	scanf("%d.%d.%d.%d",ip_temp,ip_temp+1,ip_temp+2,ip_temp+3);
        ip_temp[0] = ip_temp[0]*256 + ip_temp[1];
        ip_temp[0] = ip_temp[0]*256 + ip_temp[2];
        ip_temp[0] = ip_temp[0]*256 + ip_temp[3];
        CTRL_SHUTDOWN_VM( ip_temp[0] );
		
	// Finalizaiotn
	return 0;
}
