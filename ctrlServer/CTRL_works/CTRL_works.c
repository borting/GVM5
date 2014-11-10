#include "CTRL_works.h"

// WEB =========================================================
// =============================================================

int WEB_APP_CLOSE( unsigned short VM_WWW_PORT ){

	int conn_fd = TCP_client_sock( WEB_IP, WEB_PORT );
	int conn_ret;

	const char *req_template = "GET /main/closeApp?port=%hu HTTP/1.1\r\nHost: 140.113.151.70\r\nUser-Agent: Mozilla/5.0 (X11; U; Linux i686; en-US; rv:1.9.2.8) Gecko/20100723 Ubuntu/10.04 (lucid) Firefox/3.6.8\r\nAccept: text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8\r\n Accept-Language: en-us,en;q=0.5\r\nAccept-Encoding: gzip,deflate\r\nAccept-Charset: ISO-8859-1,utf-8;q=0.7,*;q=0.7\r\nKeep-Alive: 115\r\n\r\n";

	char conn_buff[1024]; memset( conn_buff, 0, 1024 );
	sprintf( conn_buff, req_template, VM_WWW_PORT );
	if(( conn_ret = write( conn_fd, conn_buff, strlen( conn_buff ))) == -1 ) return -1;

	close( conn_fd );
	return VM_WWW_PORT;
}

// VAPPD =======================================================
// =============================================================

int VAPPD_APP_START( unsigned int VM_PVT_IP, char *CMD ){

        int     conn_ret;
        int     conn_fd;

	unsigned char conn_ip[16];
	unsigned char conn_buff[BUFF_SIZE];

	memset( conn_ip, 0, 16 );
	sprintf( conn_ip, "%d.%d.%d.%d",
			VM_PVT_IP>>24,		VM_PVT_IP<<8>>24, 
			VM_PVT_IP<<16>>24,	VM_PVT_IP<<24>>24 );
	
printf( "CTRL[%d] Starting APP from %s\n", getpid(), conn_ip );

	conn_fd = TCP_client_sock( conn_ip, VAPPD_PORT );
        if( conn_fd < 0 ) return -1;

	VAPPD_CMD CMD_temp;
	CMD_temp.CMD_TYPE = 0;
	CMD_temp.CMD_LEN = strlen( CMD );
	strncpy( CMD_temp.CMD, CMD, strlen( CMD ));

	int size_temp = sizeof(unsigned int)*2 + sizeof(char)*CMD_temp.CMD_LEN;
	memset( conn_buff, 0, sizeof(conn_buff) );
	conn_ret = write( conn_fd, &CMD_temp, size_temp );

printf( "VAPPD < " );
printf( "%u %u %s", CMD_temp.CMD_TYPE, CMD_temp.CMD_LEN, CMD_temp.CMD );
printf("\n");

	VAPPD_RES RES_temp;
	memset( &RES_temp, 0, sizeof( VAPPD_RES ) );
	conn_ret = read( conn_fd, &RES_temp, sizeof( VAPPD_RES ) );

printf("VAPPD > ");
printf( "%u %d ", RES_temp.CMD_TYPE, RES_temp.CMD_RES );
printf( "%hhu.%hhu.%hhu.%hhu ", RES_temp.VM_PUB_IP[0], RES_temp.VM_PUB_IP[1],
				RES_temp.VM_PUB_IP[2], RES_temp.VM_PUB_IP[3]);

printf( "%hu\n", RES_temp.VM_PUB_PORT );

	if( RES_temp.CMD_RES < 0 ) 	return RES_temp.CMD_RES; // FAILED
	else				return RES_temp.VM_PUB_PORT;
}

int VAPPD_APP_CLOSE( unsigned int VM_PVT_IP, unsigned short VM_PVT_PORT ){

        int     conn_ret;
        int     conn_fd;

	unsigned char conn_ip[16];
	unsigned char conn_buff[BUFF_SIZE];

	memset( conn_ip, 0, 16 );
	sprintf( conn_ip, "%d.%d.%d.%d",
			VM_PVT_IP>>24,		VM_PVT_IP<<8>>24, 
			VM_PVT_IP<<16>>24,	VM_PVT_IP<<24>>24 );
	
printf( "CTRL[%d] Closing APP at %s[%u]\n", getpid(), conn_ip, VM_PVT_PORT );

	conn_fd = TCP_client_sock( conn_ip, VAPPD_PORT );
        if( conn_fd < 0 ) return -1;

	VAPPD_CMD_CLOSE CMD_temp;
	CMD_temp.CMD_TYPE = 4;
	CMD_temp.VM_PVT_IP[0] = VM_PVT_IP >> 24;
	CMD_temp.VM_PVT_IP[1] = VM_PVT_IP << 8  >> 24;
	CMD_temp.VM_PVT_IP[2] = VM_PVT_IP << 16 >> 24;
	CMD_temp.VM_PVT_IP[3] = VM_PVT_IP << 24 >> 24;
	CMD_temp.VM_PVT_PORT = VM_PVT_PORT;
	conn_ret = write( conn_fd, &CMD_temp, sizeof(CMD_temp) );

	VAPPD_RES RES_temp;
	memset( &RES_temp, 0, sizeof( VAPPD_RES ) );
	conn_ret = read( conn_fd, &RES_temp, sizeof( VAPPD_RES ) );

printf("VAPPD > %s\n", conn_buff);

	if( RES_temp.CMD_RES < 0 ) 	return RES_temp.CMD_RES; // FAILED
	else				return RES_temp.CMD_RES;
}

// MYSQL =======================================================
// =============================================================

PM_ENTRY *MYSQL_heartbeat_PM( PM_ENTRY *PM, unsigned int PM_IP ){

	PM_ENTRY PM_temp;
	if( DEL_PM_ENTRY( &PM_temp, PM_IP ) != NULL ){

		if( PM_temp.PM_STAT == 255 )	PM->PM_STAT = 255;
		else				PM->PM_STAT = PM_temp.PM_STAT + 1;

		PM->PM_IP = PM_IP;
		PM->IMG_USED = PM_temp.IMG_LEFT + PM_temp.IMG_USED - PM->IMG_LEFT;
                PM->CPU_USED = PM_temp.CPU_LEFT + PM_temp.CPU_USED - PM->CPU_LEFT;
                PM->MEM_USED = PM_temp.MEM_LEFT + PM_temp.MEM_USED - PM->MEM_LEFT;
		ADD_PM_ENTRY( PM );
		return PM;
	
	}else{

		PM->PM_IP = PM_IP;
		PM->PM_STAT = 60;
                ADD_PM_ENTRY( PM );
                return PM;
	}
}

PM_ENTRY *MYSQL_get_PM( PM_ENTRY *PM, int NUM_CPU, int NUM_MEM ){

	if( GET_PM_AVAILABLE( PM, NUM_CPU, NUM_MEM ) == NULL ) return NULL;
	if( DEL_PM_ENTRY( PM, PM->PM_IP ) == NULL ) return NULL ;

	PM->IMG_LEFT = PM->IMG_LEFT - 1;
	PM->IMG_USED = PM->IMG_USED + 1;

	PM->CPU_LEFT = PM->CPU_LEFT - NUM_CPU;
	PM->CPU_USED = PM->CPU_USED + NUM_CPU;
	
	PM->MEM_LEFT = PM->MEM_LEFT - NUM_MEM;
	PM->MEM_USED = PM->MEM_USED + NUM_MEM;

	if( ADD_PM_ENTRY( PM ) == NULL ) return NULL;
	return PM;
}

VM_ENTRY *MYSQL_get_VM( VM_ENTRY *VM, unsigned int PUB_IP ){

	return GET_VM_ENTRY( VM, PUB_IP );	
}


VM_ENTRY *MYSQL_del_VM( VM_ENTRY *VM, unsigned int PUB_IP ){

	return DEL_VM_ENTRY( VM, PUB_IP );
}

const VM_ENTRY *MYSQL_add_VM( VM_ENTRY *VM, int NUM_CPU, int NUM_MEM, 
			unsigned int PHY_IP, unsigned int PVT_IP, unsigned int PUB_IP ){

	VM->PM_IP 	= PHY_IP;
	VM->VM_PUB_IP	= PUB_IP;
	VM->VM_PVT_IP	= PVT_IP;
	VM->VM_CPU 	= NUM_CPU;
	VM->VM_MEM	= NUM_MEM;

	return ADD_VM_ENTRY( VM );
}

// DNAT ========================================================
// =============================================================

int DNAT_VM_Start( unsigned int PUB_IP ){

	DNAT_REQ dnat_buff;
	dnat_buff.TYPE = REQ_VMStart;
	dnat_buff.PUB_IP[0] = PUB_IP >> 24;
	dnat_buff.PUB_IP[1] = PUB_IP << 8 >>24;
	dnat_buff.PUB_IP[2] = PUB_IP << 16 >>24;
	dnat_buff.PUB_IP[3] = PUB_IP << 24 >>24;
	dnat_buff.PUB_PORT = 0;
	dnat_buff.WWW_PORT = 0;
	
	int     conn_ret;
        int     conn_fd;	

	conn_fd = TCP_client_sock( DNAT_IP, DNAT_PORT );
        if( conn_fd < 0 ) return -1;

	// Communication
	conn_ret = write( conn_fd, &dnat_buff, sizeof( DNAT_REQ ) );
	memset( &dnat_buff, 0, sizeof( DNAT_REQ ));
	conn_ret = read( conn_fd, &dnat_buff, sizeof( DNAT_REQ ) );
	close( conn_fd );

	// IF FAILED
	if( dnat_buff.TYPE < 0 )	return dnat_buff.TYPE;
	else				return 0;
}

int DNAT_VM_Close( unsigned int PUB_IP ){

	DNAT_REQ dnat_buff;
	dnat_buff.TYPE = REQ_VMClose;
	dnat_buff.PUB_IP[0] = PUB_IP >> 24;
	dnat_buff.PUB_IP[1] = PUB_IP << 8 >>24;
	dnat_buff.PUB_IP[2] = PUB_IP << 16 >>24;
	dnat_buff.PUB_IP[3] = PUB_IP << 24 >>24;
	dnat_buff.PUB_PORT = 0;
	dnat_buff.WWW_PORT = 0;
	
	int     conn_ret;
        int     conn_fd;	

	conn_fd = TCP_client_sock( DNAT_IP, DNAT_PORT );
        if( conn_fd < 0 ) return -1;

	// Communication
	conn_ret = write( conn_fd, &dnat_buff, sizeof( DNAT_REQ ) );
	memset( &dnat_buff, 0, sizeof( DNAT_REQ ));
	conn_ret = read( conn_fd, &dnat_buff, sizeof( DNAT_REQ ) );
	close( conn_fd );

	// IF FAILED
	if( dnat_buff.TYPE < 0 )	return dnat_buff.TYPE;
	else				return dnat_buff.WWW_PORT;
}

int DNAT_APP_Start( unsigned int PUB_IP, unsigned short PUB_PORT ){

	DNAT_REQ dnat_buff;
	dnat_buff.TYPE = REQ_AppStart;
	dnat_buff.PUB_IP[0] = PUB_IP >> 24;
	dnat_buff.PUB_IP[1] = PUB_IP << 8 >>24;
	dnat_buff.PUB_IP[2] = PUB_IP << 16 >>24;
	dnat_buff.PUB_IP[3] = PUB_IP << 24 >>24;
	dnat_buff.PUB_PORT = PUB_PORT;
	dnat_buff.WWW_PORT = 0;
	
	int     conn_ret;
        int     conn_fd;	

	conn_fd = TCP_client_sock( DNAT_IP, DNAT_PORT );
        if( conn_fd < 0 ) return -1;

	// Communication
	conn_ret = write( conn_fd, &dnat_buff, sizeof( DNAT_REQ ) );
	memset( &dnat_buff, 0, sizeof( DNAT_REQ ));
	conn_ret = read( conn_fd, &dnat_buff, sizeof( DNAT_REQ ) );
	close( conn_fd );

	// IF FAILED
	if( dnat_buff.TYPE < 0 )	return dnat_buff.TYPE;
	else				return dnat_buff.WWW_PORT;
}

int DNAT_APP_Close( unsigned int PUB_IP, unsigned short PUB_PORT ){

	DNAT_REQ dnat_buff;
	dnat_buff.TYPE = REQ_AppClose;
	dnat_buff.PUB_IP[0] = PUB_IP >> 24;
	dnat_buff.PUB_IP[1] = PUB_IP << 8 >>24;
	dnat_buff.PUB_IP[2] = PUB_IP << 16 >>24;
	dnat_buff.PUB_IP[3] = PUB_IP << 24 >>24;
	dnat_buff.PUB_PORT = PUB_PORT;
	dnat_buff.WWW_PORT = 0;
	
	int     conn_ret;
        int     conn_fd;	

	conn_fd = TCP_client_sock( DNAT_IP, DNAT_PORT );
        if( conn_fd < 0 ){

		printf( "Connet\n" );
		return -1;
	}

	// Communication
	conn_ret = write( conn_fd, &dnat_buff, sizeof( DNAT_REQ ) );
	memset( &dnat_buff, 0, sizeof( DNAT_REQ ));
	conn_ret = read( conn_fd, &dnat_buff, sizeof( DNAT_REQ ) );
	close( conn_fd );

	// IF FAILED
	if( dnat_buff.TYPE < 0 ){

		printf( " Failed\n" );
		return dnat_buff.TYPE;
	
	}else{

		return dnat_buff.WWW_PORT;
	}
}

int DNAT_APP_GET_PVT_PORT(	unsigned short WWW_PORT,
				unsigned int *PUB_IP, unsigned short *PUB_PORT){

	DNAT_REQ dnat_buff;
	dnat_buff.TYPE = REQ_AppGetPVTPort;
	dnat_buff.PUB_IP[0] = 0;
	dnat_buff.PUB_IP[1] = 0;
	dnat_buff.PUB_IP[2] = 0;
	dnat_buff.PUB_IP[3] = 0;
	dnat_buff.PUB_PORT = 0;
	dnat_buff.WWW_PORT = WWW_PORT;

	// Communication
	int     conn_ret;
        int     conn_fd;	

	conn_fd = TCP_client_sock( DNAT_IP, DNAT_PORT );
        if( conn_fd < 0 ) return -1;	
	conn_ret = write( conn_fd, &dnat_buff, sizeof( DNAT_REQ ) );

	memset( &dnat_buff, 0, sizeof( DNAT_REQ ));
	conn_ret = read( conn_fd, &dnat_buff, sizeof( DNAT_REQ ) );
	close( conn_fd );

	// IF FAILED
	if( dnat_buff.TYPE < 0 ){

		printf("%d\n", dnat_buff.TYPE );
		return dnat_buff.TYPE;
	}
	else{
		unsigned int IP_temp;
		IP_temp = dnat_buff.PUB_IP[0];
		IP_temp = (IP_temp << 8) + dnat_buff.PUB_IP[1];
		IP_temp = (IP_temp << 8) + dnat_buff.PUB_IP[2];
		IP_temp = (IP_temp << 8) + dnat_buff.PUB_IP[3];
		
		*PUB_IP = IP_temp;
		*PUB_PORT = dnat_buff.PUB_PORT;
		return dnat_buff.PUB_PORT;
	}
}

// CMND ========================================================
// =============================================================

VM_ENTRY *CMND_VM_START( VM_ENTRY *data, unsigned int CMND_IP, int NUM_CPU, int NUM_MEM ){

        int     conn_ret;
        int     conn_fd;

	unsigned char conn_ip[16];
	unsigned char conn_buff[BUFF_SIZE];

	memset( conn_ip, 0, 16 );
	sprintf( conn_ip, "%d.%d.%d.%d",
			CMND_IP>>24,	CMND_IP<<8>>24, 
			CMND_IP<<16>>24,CMND_IP<<24>>24 );

printf( "CTRL[%d] Starting VM from %s\n", getpid(), conn_ip );

	conn_fd = TCP_client_sock( conn_ip, CMND_PORT );
        if( conn_fd < 0 ) return NULL;

	// Payload
	memset( conn_buff, 0, BUFF_SIZE );
        conn_buff[0] = 1;
        conn_buff[2] = NUM_CPU;
        conn_buff[3] = NUM_MEM/256;
        conn_buff[4] = NUM_MEM%256;

printf( "CMND < " );
for( int idx = 0 ;idx < 10 ; idx++ ) printf("[%hhu]", conn_buff[idx]);
printf("\n");

	// Communication
	conn_ret = write( conn_fd, conn_buff, 5 );
	memset( conn_buff, 0, BUFF_SIZE );
	conn_ret = read( conn_fd, conn_buff, 10 );
	close( conn_fd );

printf( "CMND > " );
for( int idx = 0 ;idx < 10 ;idx++ ) printf("[%hhu]", conn_buff[idx]);
printf("\n");
	
	// Operation failed
        if( conn_buff[0] == 255 ) return NULL;

	// Parsing response
        data->PM_IP	= CMND_IP;
        data->VM_PVT_IP	= (conn_buff[2]<<24)+(conn_buff[3]<<16)+(conn_buff[4]<<8)+conn_buff[5];
        data->VM_PUB_IP	= (conn_buff[6]<<24)+(conn_buff[7]<<16)+(conn_buff[8]<<8)+conn_buff[9];
        data->VM_CPU	= NUM_CPU;
        data->VM_MEM	= NUM_MEM;

	return data;
}

VM_ENTRY *CMND_VM_SHUTDOWN( VM_ENTRY *data, unsigned int CMND_IP, unsigned int VM_PUB_IP ){

	int     conn_ret;
	int     conn_fd;

        unsigned char   conn_ip[16]; 
        unsigned char   conn_buff[BUFF_SIZE];

	memset( conn_ip, 0, 16 );
	sprintf( conn_ip, "%d.%d.%d.%d",
		CMND_IP/256/256/256, (CMND_IP/256/256)%256,
		(CMND_IP/256)%256, CMND_IP%256 );

printf( "CTRL[%d] Shutting down VM from %s\n", getpid(), conn_ip );

	conn_fd = TCP_client_sock( conn_ip, CMND_PORT );
        if( conn_fd < 0 ) return NULL;

	memset( conn_buff, 0, BUFF_SIZE );
	conn_buff[0] = 2;
	conn_buff[2] = VM_PUB_IP>>24;
	conn_buff[3] = ((VM_PUB_IP<<8)>>24);
	conn_buff[4] = ((VM_PUB_IP<<16)>>24);
	conn_buff[5] = ((VM_PUB_IP<<24)>>24);

printf("CMND < ");
for( int idx = 0 ;idx < 10 ;idx++ ) printf("[%hhu]", conn_buff[idx]);
printf("\n");

	// Communication
	conn_ret = write( conn_fd, conn_buff, 6 );
	memset( conn_buff, 0, BUFF_SIZE );
	conn_ret = read( conn_fd, conn_buff, 10 );
	close( conn_fd );

printf("CMND > ");
for( int idx = 0 ;idx < 10 ;idx++ ) printf("[%hhu]", conn_buff[idx]);
printf("\n");

	// Operation failed
        if( conn_buff[0]==255 ) return NULL;

	// Parsing response
        data->PM_IP	= CMND_IP;
        data->VM_PUB_IP	= (conn_buff[2]<<24)+(conn_buff[3]<<16)+(conn_buff[4]<<8)+conn_buff[5];
        data->VM_PVT_IP	= (conn_buff[6]<<24)+(conn_buff[7]<<16)+(conn_buff[8]<<8)+conn_buff[9];
        data->VM_CPU	= 0;
        data->VM_MEM	= 0;

	return data;
}

VM_ENTRY *CMND_VM_REBOOT( VM_ENTRY *data, unsigned int CMND_IP, unsigned int VM_PUB_IP ){
	
	int     conn_ret;
	int     conn_fd;

        unsigned char   conn_ip[16]; 
        unsigned char   conn_buff[BUFF_SIZE];

	memset( conn_ip, 0, 16 );
	sprintf( conn_ip, "%d.%d.%d.%d",
		CMND_IP/256/256/256, (CMND_IP/256/256)%256,
		(CMND_IP/256)%256, CMND_IP%256 );

printf( "CTRL[%d] Rebooting VM from %s\n", getpid(), conn_ip );

	conn_fd = TCP_client_sock( conn_ip, CMND_PORT );
        if( conn_fd < 0 ) return NULL;

	memset( conn_buff, 0, BUFF_SIZE );
	conn_buff[0] = 3;
	conn_buff[2] = VM_PUB_IP>>24;
	conn_buff[3] = ((VM_PUB_IP<<8)>>24);
	conn_buff[4] = ((VM_PUB_IP<<16)>>24);
	conn_buff[5] = ((VM_PUB_IP<<24)>>24);

printf("CMND < ");
for( int idx = 0 ;idx < 10 ;idx++ ) printf("[%hhu]", conn_buff[idx]);
printf("\n");

	// Communication
	conn_ret = write( conn_fd, conn_buff, 6 );
	memset( conn_buff, 0, BUFF_SIZE );
	conn_ret = read( conn_fd, conn_buff, 10 );
	close( conn_fd );

printf("CMND > ");
for( int idx = 0 ;idx < 10 ;idx++ ) printf("[%hhu]", conn_buff[idx]);
printf("\n");

	// Operation failed
        if( conn_buff[0]==255 ) return NULL;

	// Parsing response
        data->PM_IP	= CMND_IP;
        data->VM_PVT_IP	= (conn_buff[2]<<24)+(conn_buff[3]<<16)+(conn_buff[4]<<8)+conn_buff[5];
        data->VM_PUB_IP	= (conn_buff[6]<<24)+(conn_buff[7]<<16)+(conn_buff[8]<<8)+conn_buff[9];
        data->VM_CPU	= 0;
        data->VM_MEM	= 0;

	return data;
}

