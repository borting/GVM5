#include "CTRL_services.h"

// Foundamental maintance
int HEARTBEAT( PM_ENTRY *PM ){
	
	if( MYSQL_heartbeat_PM( PM, PM->PM_IP ) != NULL)	return 0;
	else							return -1;	
}

// VM Power control
unsigned int CREATE_VM( unsigned int REQ_CPU, unsigned int REQ_MEM ){

	// Get availibale PM + Update DATABASE
	PM_ENTRY PM_temp;
	if( MYSQL_get_PM( &PM_temp, REQ_CPU, REQ_MEM ) == NULL){

		printf("MYSQL !!!\n");
		return 0;
	}
	
	// Start VM from PM
	VM_ENTRY VM_temp;
	if( CMND_VM_START( &VM_temp, PM_temp.PM_IP, REQ_CPU, REQ_MEM ) == NULL ){
	
		printf("CMND !!!\n");
		return 0;
	}

	// Update DATABASE
	VM_temp.VM_CPU = REQ_CPU;
	VM_temp.VM_MEM = REQ_MEM;
	const VM_ENTRY *res = MYSQL_add_VM(	&VM_temp, REQ_CPU, REQ_MEM, VM_temp.PM_IP, 
						VM_temp.VM_PVT_IP, VM_temp.VM_PUB_IP );
	// If failed
	if( res == NULL ){

		printf( "MYSQL 2\n" );
		return 0; // FAILED
	}
	
	// Notify DNAT
	if( DNAT_VM_Start( VM_temp.VM_PUB_IP ) < 0 ){

		printf( "DNAT\n" );
		return 0;
	}
	else						return VM_temp.VM_PUB_IP;
}

unsigned int SHUTDOWN_VM( unsigned int VM_PUB_IP ){

	// Get VM's PM_IP
	VM_ENTRY VM_temp;
	if( MYSQL_get_VM( &VM_temp, VM_PUB_IP ) == NULL ) return 0;
	
	// Shutdown VM from PM
	if( CMND_VM_SHUTDOWN( &VM_temp, VM_temp.PM_IP, VM_temp.VM_PUB_IP ) == NULL ) return 0;
	
	// Update DATABASE
	VM_ENTRY *res = MYSQL_del_VM( &VM_temp, VM_temp.VM_PUB_IP );
	if( res == NULL )	return 0; // FAILED
	
	// Notify DNAT
	if( DNAT_VM_Close( VM_temp.VM_PUB_IP ) < 0 ) return 0;
	else			return VM_temp.VM_PUB_IP;	
}

unsigned int REBOOT_VM( unsigned int VM_PUB_IP ){

	// Get VM's PHY_IP
	VM_ENTRY VM_temp;
	if( MYSQL_get_VM( &VM_temp, VM_PUB_IP ) == NULL ) return 0;
	
	// Notify DNAT
	if( DNAT_VM_Close( VM_temp.VM_PUB_IP ) < 0 ) return 0;

	// Update DATABASE
	const VM_ENTRY *res = MYSQL_del_VM( &VM_temp, VM_temp.VM_PUB_IP );
	if( res == NULL ) return 0;
	
	// Shutdown VM from PM
	if( CMND_VM_REBOOT( &VM_temp, VM_temp.PM_IP, VM_temp.VM_PUB_IP ) == NULL ) return 0;
	
	// Notify DNAT
	if( DNAT_VM_Start( VM_temp.VM_PUB_IP ) < 0 ) return 0;

	// Update DATABASE
	res = MYSQL_add_VM( 	&VM_temp, VM_temp.VM_CPU, VM_temp.VM_MEM, 
				VM_temp.PM_IP, VM_temp.VM_PVT_IP, VM_temp.VM_PUB_IP );
	
	if( res == NULL )	return 0; // FAILED
	else			return VM_temp.VM_PUB_IP;
}

// APP Control
int APP_START( unsigned int VM_PUB_IP, unsigned char *CMD ){

	// Get VM's PVT_IP
	VM_ENTRY VM_temp;
	if( MYSQL_get_VM( &VM_temp, VM_PUB_IP ) == NULL ) return -1;

	// Start VAPP
	int VM_PUB_PORT =  VAPPD_APP_START( VM_temp.VM_PVT_IP, CMD );
	if( VM_PUB_PORT < 0 ) return -1;

	// ASK 4 ROUTING
	return DNAT_APP_Start( VM_temp.VM_PUB_IP, VM_PUB_PORT );
}

int APP_CLOSE( unsigned short VM_WWW_PORT ){

	// Get APP's PUB_IP + PORT
	unsigned int	VM_PUB_IP;
	unsigned short	VM_PUB_PORT;

	int res = DNAT_APP_GET_PVT_PORT( VM_WWW_PORT, &VM_PUB_IP, &VM_PUB_PORT );
	if( res < 0 ){
	
		printf( "DNAT ERR\n");
		return -1;	
	}
	
	// Get VM's PVT_IP
	VM_ENTRY VM_temp;
	if( MYSQL_get_VM( &VM_temp, VM_PUB_IP ) == NULL ){

		printf( "VM not found\n");
		return -1;
	}

	// Close APP
	res = VAPPD_APP_CLOSE( VM_temp.VM_PVT_IP, VM_PUB_PORT );
	if( res < 0 ){

		printf( "VAPPD ERR\n" );
		return -1;
	}else{
		return 0;
	}
}

int APP_CLOSE_VAPPD( unsigned int VM_PUB_IP, unsigned short VM_PUB_PORT ){

	// ASK 4 DEROUTING
        int VM_WWW_PORT = DNAT_APP_Close( VM_PUB_IP, VM_PUB_PORT );
	if( VM_WWW_PORT < 0 ) return -1;

	WEB_APP_CLOSE( (unsigned short)VM_WWW_PORT );
	return 0;
}
