#include <stdio.h>
#include "CTRL_data_op.h"

// Externs ====================================================
// ============================================================

const char *MYSQL_host = "localhost";
const char *MYSQL_user = "root";
const char *MYSQL_pass = "temp";
const char *MYSQL_dbase = "CTRL_DATA";
const char *MYSQL_socket = NULL;

const unsigned int     MYSQL_port = 0;
const unsigned long    MYSQL_client_flag = 0;

const char *CTRL_PM_TABLE_NAME = "PM_TABLE";
const char *CTRL_VM_TABLE_NAME = "VM_TABLE";

// MAIN =======================================================
// ============================================================

int main(){

	// Creat TABLE
	printf( "Creating data base\n" );

	printf( "%d\n", DROP_TABLE( CTRL_PM_TABLE_NAME ));
	printf( "%d\n", CREATE_PM_TABLE( ));

	printf( "%d\n", DROP_TABLE( CTRL_VM_TABLE_NAME ));
	printf( "%d\n", CREATE_VM_TABLE( ));

	// PM TEST ============================================
	printf( "\nPM TEST\n");

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

	// Get input
	unsigned int ip_temp[4] = {0};
	scanf("%d.%d.%d.%d",ip_temp,ip_temp+1,ip_temp+2,ip_temp+3);
	ip_temp[0] = ip_temp[0]*256 + ip_temp[1];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[2];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[3];
	printf( "%u\n", ip_temp[0]);

	// Get data
	if(GET_PM_ENTRY( PM_temp, ip_temp )!=NULL){

		char    str_IP_temp[16];
        	sprintf( str_IP_temp,   "%d.%d.%d.%d",
                                PM_temp->PM_IP/256/256/256,
                                PM_temp->PM_IP/256/256%256,
                                PM_temp->PM_IP/256%256,
                                PM_temp->PM_IP%256);
	
		printf( "%s, %d, %d, %d, %d, %d, %d, %d\n",
			str_IP_temp, PM_temp->PM_STAT,
			PM_temp->IMG_LEFT, PM_temp->IMG_USED,
			PM_temp->CPU_LEFT, PM_temp->CPU_USED,
			PM_temp->MEM_LEFT, PM_temp->MEM_USED);
	} else {

		printf( "No found\n");
	}

	// Get input
	scanf("%d.%d.%d.%d",ip_temp,ip_temp+1,ip_temp+2,ip_temp+3);
	ip_temp[0] = ip_temp[0]*256 + ip_temp[1];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[2];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[3];
	printf( "%u\n", ip_temp[0]);

	// Delete 
	if(DEL_PM_ENTRY( PM_temp, ip_temp )!=NULL){

		char    str_IP_temp[16];
        	sprintf( str_IP_temp,   "%d.%d.%d.%d",
                                PM_temp->PM_IP/256/256/256,
                                PM_temp->PM_IP/256/256%256,
                                PM_temp->PM_IP/256%256,
                                PM_temp->PM_IP%256);
	
		printf( "%s, %d, %d, %d, %d, %d, %d, %d\n",
			str_IP_temp, PM_temp->PM_STAT,
			PM_temp->IMG_LEFT, PM_temp->IMG_USED,
			PM_temp->CPU_LEFT, PM_temp->CPU_USED,
			PM_temp->MEM_LEFT, PM_temp->MEM_USED);
	} else {

		printf( "No found\n");
	}

	// Get input
	unsigned int REQ_CPU, REQ_MEM;
	scanf( "%d %d", &REQ_CPU, &REQ_MEM );
	if(GET_PM_AVAILABLE( PM_temp, REQ_CPU, REQ_MEM )!=NULL){

		char str_IP_temp[16];
        	sprintf( str_IP_temp,   "%d.%d.%d.%d",
                                PM_temp->PM_IP/256/256/256,
                                PM_temp->PM_IP/256/256%256,
                                PM_temp->PM_IP/256%256,
                                PM_temp->PM_IP%256);
	
		char *str_IP_ptr = (char *)&(PM_temp->PM_IP);
		printf( "%hhu.%hhu.%hhu.%hhu [%d][%u %u][%u %u][%u %u]\n",
			*(str_IP_ptr+3), *(str_IP_ptr+2), *(str_IP_ptr+1), *(str_IP_ptr+0),
//		printf( "%s [%d][%u %u][%u %u][%u %u]\n",
//			str_IP_temp,
			PM_temp->PM_STAT,
			PM_temp->IMG_LEFT, PM_temp->IMG_USED,
			PM_temp->CPU_LEFT, PM_temp->CPU_USED,
			PM_temp->MEM_LEFT, PM_temp->MEM_USED);
	} else {

		printf( "No found\n");
	}

	// VM test ============================================
	printf( "\nVM_test\n");

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

	// Get input
	scanf("%d.%d.%d.%d",ip_temp,ip_temp+1,ip_temp+2,ip_temp+3);
	ip_temp[0] = ip_temp[0]*256 + ip_temp[1];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[2];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[3];
	printf( "%u\n", ip_temp[0]);

	// Get data
	if(GET_VM_ENTRY( VM_temp, ip_temp )!=NULL){

		char    str_IP_temp_0[16];
        	sprintf( str_IP_temp_0,   "%d.%d.%d.%d",
                                VM_temp->PM_IP/256/256/256,
                                VM_temp->PM_IP/256/256%256,
                                VM_temp->PM_IP/256%256,
                                VM_temp->PM_IP%256);
	
		char    str_IP_temp_1[16];
        	sprintf( str_IP_temp_1,   "%d.%d.%d.%d",
                                VM_temp->VM_PUB_IP/256/256/256,
                                VM_temp->VM_PUB_IP/256/256%256,
                                VM_temp->VM_PUB_IP/256%256,
                                VM_temp->VM_PUB_IP%256);

		char    str_IP_temp_2[16];
        	sprintf( str_IP_temp_2,   "%d.%d.%d.%d",
                                VM_temp->VM_PVT_IP/256/256/256,
                                VM_temp->VM_PVT_IP/256/256%256,
                                VM_temp->VM_PVT_IP/256%256,
                                VM_temp->VM_PVT_IP%256);

		printf( "%s, %s, %s, %d, %d\n",
			str_IP_temp_0,
			str_IP_temp_1,
			str_IP_temp_2,
			VM_temp->VM_CPU, 
			VM_temp->VM_MEM);
	} else {

		printf( "No found\n");
	}

	// Get input
	scanf("%d.%d.%d.%d",ip_temp,ip_temp+1,ip_temp+2,ip_temp+3);
	ip_temp[0] = ip_temp[0]*256 + ip_temp[1];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[2];
	ip_temp[0] = ip_temp[0]*256 + ip_temp[3];
	printf( "%u\n", ip_temp[0]);
	
	// Get data
	if(GET_VM_ENTRY( VM_temp, ip_temp )!=NULL){

		char    str_IP_temp_0[16];
        	sprintf( str_IP_temp_0,   "%d.%d.%d.%d",
                                VM_temp->PM_IP/256/256/256,
                                VM_temp->PM_IP/256/256%256,
                                VM_temp->PM_IP/256%256,
                                VM_temp->PM_IP%256);
	
		char    str_IP_temp_1[16];
        	sprintf( str_IP_temp_1,   "%d.%d.%d.%d",
                                VM_temp->VM_PUB_IP/256/256/256,
                                VM_temp->VM_PUB_IP/256/256%256,
                                VM_temp->VM_PUB_IP/256%256,
                                VM_temp->VM_PUB_IP%256);

		char    str_IP_temp_2[16];
        	sprintf( str_IP_temp_2,   "%d.%d.%d.%d",
                                VM_temp->VM_PVT_IP/256/256/256,
                                VM_temp->VM_PVT_IP/256/256%256,
                                VM_temp->VM_PVT_IP/256%256,
                                VM_temp->VM_PVT_IP%256);

		printf( "%s, %s, %s, %d, %d\n",
			str_IP_temp_0,
			str_IP_temp_1,
			str_IP_temp_2,
			VM_temp->VM_CPU, 
			VM_temp->VM_MEM);
	} else {

		printf( "No found\n");
	}

	// Finalizaiotn
	return 0;
}
