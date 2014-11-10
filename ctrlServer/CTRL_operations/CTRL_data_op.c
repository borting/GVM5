#include "CTRL_data_op.h"
/*
// Connection
const extern char             *MYSQL_host;
const extern char             *MYSQL_user;
const extern char             *MYSQL_pass;
const extern char             *MYSQL_dbase;
const extern unsigned int     MYSQL_port;
const extern char             *MYSQL_socket;
const extern unsigned long    MYSQL_client_flag;

const extern char *CTRL_PM_TABLE_NAME;
const extern char *CTRL_VM_TABLE_NAME;
*/
// General
unsigned int PRINT_ERR(MYSQL *con){

	fprintf(stderr, "%s\n", mysql_error(con));
	mysql_close(con);
	return 1;        
}

int DROP_TABLE( const char *name ){

	// connect 
	MYSQL *con = mysql_init(NULL);
	if (con == NULL) PRINT_ERR( con );
	if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass, 
				MYSQL_dbase, MYSQL_port, 
				MYSQL_socket, MYSQL_client_flag ) == NULL) {
	
		return PRINT_ERR( con );
	}

	// Setn query
	char str_query[64];
	sprintf( str_query, "DROP TABLE IF EXISTS %s", name);
	if (mysql_query(con, str_query)) return PRINT_ERR( con );

	// finalization
	mysql_close(con);
	return 0;
}

// PM TABLE ===================================================
int CREATE_PM_TABLE(  ){

	// connect 
	MYSQL *con = mysql_init(NULL);
	if (con == NULL) PRINT_ERR( con );
	if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass, 
				MYSQL_dbase, MYSQL_port, 
				MYSQL_socket, MYSQL_client_flag ) == NULL) {
	
		return PRINT_ERR( con );
	}

	// Setn query
	char str_query[512];
	sprintf( str_query, "CREATE TABLE %s(PM_IP TEXT, PM_STAT TINYINT, IMG_LEFT TINYINT UNSIGNED, IMG_USED TINYINT UNSIGNED, CPU_LEFT TINYINT UNSIGNED, CPU_USED TINYINT UNSIGNED, MEM_LEFT SMALLINT UNSIGNED, MEM_USED SMALLINT UNSIGNED)", CTRL_PM_TABLE_NAME);
	if (mysql_query(con, str_query)) return PRINT_ERR( con );

	// finalization
	mysql_close(con);
	return 0;
}

PM_ENTRY *DISP_PM_ENTRY( PM_ENTRY *data ){

	printf( "PM_IP\t%d.%d.%d.%d\n", 
			data->PM_IP>>24, 
			data->PM_IP<<8>>24, 
			data->PM_IP<<16>>24, 
			data->PM_IP<<24>>24);

        printf( "PM_STAT\t%d\n", data->PM_STAT);
    
	printf( "IMG_LEFT\t%d\n", data->IMG_LEFT);
        printf( "IMG_USED\t%d\n", data->IMG_USED);
        
	printf( "CPU_LEFT\t%d\n", data->CPU_LEFT);
        printf( "CPU_USED\t%d\n", data->CPU_USED);
        
	printf( "MEM_LEFT\t%d\n", data->MEM_LEFT);
        printf( "MEM_USED\t%d\n", data->MEM_USED);

	return data;
}

PM_ENTRY *GET_PM_ENTRY( PM_ENTRY *data, const unsigned int PM_IP ){

	// connect
        MYSQL *con = mysql_init(NULL);
        if (con == NULL) PRINT_ERR( con );
        if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass,
                                MYSQL_dbase, MYSQL_port,
                                MYSQL_socket, MYSQL_client_flag ) == NULL) {

                PRINT_ERR( con );
		return NULL;
        }
	
        // Set query
	char	str_IP_temp[16];
	sprintf( str_IP_temp, 	"%d.%d.%d.%d", 
				PM_IP/256/256/256,	PM_IP/256/256%256,
				PM_IP/256%256,	PM_IP%256);        	
	char str_query[512];
        sprintf( str_query, "select * from %s where PM_IP=\"%s\"",
				CTRL_PM_TABLE_NAME, str_IP_temp);

        if (mysql_query(con, str_query)){

		PRINT_ERR(con);
		return NULL;
	}

	// Save data
	MYSQL_RES *result = mysql_store_result(con);
	if (result == NULL){
	
		PRINT_ERR(con);
		return NULL;
  	}

	MYSQL_ROW row = mysql_fetch_row(result);
	if( row == NULL ) return NULL;

	data->PM_IP	= 0;		data->PM_STAT	= atoi(row[1]);
	data->IMG_LEFT	= atoi(row[2]);	data->IMG_USED	= atoi(row[3]);
	data->CPU_LEFT	= atoi(row[4]);	data->CPU_USED	= atoi(row[5]);
	data->MEM_LEFT	= atoi(row[6]);	data->MEM_USED	= atoi(row[7]);
	
	unsigned int ip_temp[4]={0};
	sscanf( row[0], "%d.%d.%d.%d", ip_temp, ip_temp+1, ip_temp+2, ip_temp+3 );
	data->PM_IP = (( ip_temp[0]*256 + ip_temp[1] )*256 + ip_temp[2])*256 + ip_temp[3];

        // finalization
	mysql_free_result(result);
        mysql_close(con);
	return data;
}

PM_ENTRY *DEL_PM_ENTRY( PM_ENTRY *data, const unsigned int PM_IP ){

	// Get data
	if(GET_PM_ENTRY( data, PM_IP )==NULL) return NULL;

	// connect
        MYSQL *con = mysql_init(NULL);
        if (con == NULL) PRINT_ERR( con );
        if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass,
                                MYSQL_dbase, MYSQL_port,
                                MYSQL_socket, MYSQL_client_flag ) == NULL) {

                PRINT_ERR( con );
		return NULL;
        }
	
        // Set query
	char	str_IP_temp[16];
	sprintf( str_IP_temp, 	"%d.%d.%d.%d", 
				PM_IP/256/256/256,	PM_IP/256/256%256,
				PM_IP/256%256,	PM_IP%256);        	
	char str_query[512];
        sprintf( str_query, "delete from %s where PM_IP=\"%s\"",
				CTRL_PM_TABLE_NAME, str_IP_temp);

        if (mysql_query(con, str_query)){

		PRINT_ERR(con);
		return NULL;
	}

        mysql_close(con);
	return data;
}

const PM_ENTRY *ADD_PM_ENTRY( const PM_ENTRY *data_in ){

	// connect
        MYSQL *con = mysql_init(NULL);
        if (con == NULL) PRINT_ERR( con );
        if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass,
                                MYSQL_dbase, MYSQL_port,
                                MYSQL_socket, MYSQL_client_flag ) == NULL) {

                PRINT_ERR( con );
		return NULL;
        }

        // Set query
	char	str_IP_temp[16];
	sprintf( str_IP_temp, 	"%d.%d.%d.%d", 
				data_in->PM_IP/256/256/256,
				data_in->PM_IP/256/256%256,
				data_in->PM_IP/256%256,
				data_in->PM_IP%256);        

	char str_query[512];
        sprintf( str_query, "insert into %s values('%s', %d, %d, %d, %d, %d, %d, %d)",
				CTRL_PM_TABLE_NAME, str_IP_temp, data_in->PM_STAT,
				data_in->IMG_LEFT, data_in->IMG_USED,
				data_in->CPU_LEFT, data_in->CPU_USED,
				data_in->MEM_LEFT, data_in->MEM_USED);

        if (mysql_query(con, str_query)){

		PRINT_ERR( con );
		return NULL;
	}

        // finalization
        mysql_close(con);
	return data_in;
}

PM_ENTRY *GET_PM_AVAILABLE( PM_ENTRY *data, unsigned int REQ_CPU, unsigned int REQ_MEM ){

	// connect
        MYSQL *con = mysql_init(NULL);
        if (con == NULL) PRINT_ERR( con );
        if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass,
                                MYSQL_dbase, MYSQL_port,
                                MYSQL_socket, MYSQL_client_flag ) == NULL) {

                PRINT_ERR( con );
		return NULL;
        }
	
        // Set query
	char str_query[512];
        sprintf( str_query, "select * from %s where IMG_LEFT>0 and CPU_LEFT>=%u and MEM_LEFT>=%u LIMIT 0,1", CTRL_PM_TABLE_NAME, REQ_CPU, REQ_MEM);

        if (mysql_query(con, str_query)){

		PRINT_ERR(con);
		return NULL;
	}

	// Save data
	MYSQL_RES *result = mysql_store_result(con);
	if (result == NULL){
	
		PRINT_ERR(con);
		return NULL;
  	}

	MYSQL_ROW row = mysql_fetch_row(result);
	if( row == NULL ) return NULL;

	data->PM_IP	= 0;		data->PM_STAT	= atoi(row[1]);
	data->IMG_LEFT	= atoi(row[2]);	data->IMG_USED	= atoi(row[3]);
	data->CPU_LEFT	= atoi(row[4]);	data->CPU_USED	= atoi(row[5]);
	data->MEM_LEFT	= atoi(row[6]);	data->MEM_USED	= atoi(row[7]);
	
	unsigned int ip_temp[4]={0};
	sscanf( row[0], "%d.%d.%d.%d", ip_temp, ip_temp+1, ip_temp+2, ip_temp+3 );
	data->PM_IP = (( ip_temp[0]*256 + ip_temp[1] )*256 + ip_temp[2])*256 + ip_temp[3];

        // finalization
	mysql_free_result(result);
        mysql_close(con);
	return data;
}

int DECREASE_PM_STAT(){
/*
	// connect
        MYSQL *con = mysql_init(NULL);
        if (con == NULL) PRINT_ERR( con );
        if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass,
                                MYSQL_dbase, MYSQL_port,
                                MYSQL_socket, MYSQL_client_flag ) == NULL) {

                PRINT_ERR( con );
		return 1; 
        }

        // Set query
	char str_query[512];
        sprintf( str_query, "update %s set PM_STAT=PM_STAT-1", );
        if (mysql_query(con, str_query)){

		PRINT_ERR(con);
		return 1;
	}
	
        // finalization
	mysql_free_result(result);
        mysql_close(con);
*/	return 0;
}

// VM TABLE ===================================================
int CREATE_VM_TABLE( ){

	// connect 
	MYSQL *con = mysql_init(NULL);
	if (con == NULL) PRINT_ERR( con );
	if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass, 
				MYSQL_dbase, MYSQL_port, 
				MYSQL_socket, MYSQL_client_flag ) == NULL) {
	
		return PRINT_ERR( con );
	}

	// Setn query
	char str_query[512];
	sprintf( str_query, "CREATE TABLE %s(PM_IP TEXT, VM_PUB_IP TEXT, VM_PVT_IP TEXT, VM_CPU TINYINT UNSIGNED, VM_MEM SMALLINT UNSIGNED)", CTRL_VM_TABLE_NAME);
	if (mysql_query(con, str_query)) return PRINT_ERR( con );

	// finalization
	mysql_close(con);
	return 0;
}

VM_ENTRY *DISP_VM_ENTRY( VM_ENTRY *data ){

	printf( "PM_IP\t%d.%d.%d.%d\n", 
			data->PM_IP>>24, 
			data->PM_IP<<8>>24, 
			data->PM_IP<<16>>24, 
			data->PM_IP<<24>>24);

	printf( "PVT_IP\t%d.%d.%d.%d\n", 
			data->VM_PVT_IP>>24, 
			data->VM_PVT_IP<<8>>24, 
			data->VM_PVT_IP<<16>>24, 
			data->VM_PVT_IP<<24>>24);

	printf( "PUB_IP\t%d.%d.%d.%d\n", 
			data->VM_PUB_IP>>24, 
			data->VM_PUB_IP<<8>>24, 
			data->VM_PUB_IP<<16>>24, 
			data->VM_PUB_IP<<24>>24);

	printf( "VM_CPU\t%d\n", data->VM_CPU);
        printf( "VM_MEM\t%d\n", data->VM_MEM);

	return data;
}

VM_ENTRY *GET_VM_ENTRY( VM_ENTRY *data, const unsigned int VM_PUB_IP ){

	// connect
        MYSQL *con = mysql_init(NULL);
        if (con == NULL) PRINT_ERR( con );
        if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass,
                                MYSQL_dbase, MYSQL_port,
                                MYSQL_socket, MYSQL_client_flag ) == NULL) {

                PRINT_ERR( con );
		return NULL;
        }
	
        // Set query
	char	str_IP_temp[16];
	sprintf( str_IP_temp, 	"%d.%d.%d.%d", 
				VM_PUB_IP/256/256/256,	VM_PUB_IP/256/256%256,
				VM_PUB_IP/256%256,		VM_PUB_IP%256);
	
	char str_query[512];
        sprintf( str_query, "select * from %s where VM_PUB_IP=\"%s\"",
				CTRL_VM_TABLE_NAME, str_IP_temp);

        if (mysql_query(con, str_query)){

		PRINT_ERR(con);
		return NULL;
	}

	// Save data
	MYSQL_RES *result = mysql_store_result(con);
	if (result == NULL){
	
		PRINT_ERR(con);
		return NULL;
  	}

	MYSQL_ROW row = mysql_fetch_row(result);
	if( row == NULL ) return NULL;

	data->PM_IP	= 0;		
	data->VM_PUB_IP	= 0;		
	data->VM_PVT_IP	= 0;		
	data->VM_CPU	= atoi( row[3] );		
	data->VM_MEM	= atoi( row[4] );		
	
	unsigned int ip_temp[4]={0};
	sscanf( row[0], "%d.%d.%d.%d", ip_temp, ip_temp+1, ip_temp+2, ip_temp+3 );
	data->PM_IP = (( ip_temp[0]*256 + ip_temp[1] )*256 + ip_temp[2])*256 + ip_temp[3];

	sscanf( row[1], "%d.%d.%d.%d", ip_temp, ip_temp+1, ip_temp+2, ip_temp+3 );
	data->VM_PUB_IP = ((ip_temp[0]*256 + ip_temp[1])*256 + ip_temp[2])*256 + ip_temp[3];

	sscanf( row[2], "%d.%d.%d.%d", ip_temp, ip_temp+1, ip_temp+2, ip_temp+3 );
	data->VM_PVT_IP = ((ip_temp[0]*256 + ip_temp[1])*256 + ip_temp[2])*256 + ip_temp[3];
        
	// finalization
	mysql_free_result(result);
        mysql_close(con);
	return data;
}

VM_ENTRY *DEL_VM_ENTRY( VM_ENTRY *data, const unsigned int VM_PUB_IP ){

	// Get data
	if(GET_VM_ENTRY( data, VM_PUB_IP )==NULL) return NULL;

	// connect
        MYSQL *con = mysql_init(NULL);
        if (con == NULL) PRINT_ERR( con );
        if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass,
                                MYSQL_dbase, MYSQL_port,
                                MYSQL_socket, MYSQL_client_flag ) == NULL) {

                PRINT_ERR( con );
		return NULL;
        }
	
        // Set query
	char	str_IP_temp[16];
	sprintf( str_IP_temp, 	"%d.%d.%d.%d", 
			VM_PUB_IP/256/256/256,	VM_PUB_IP/256/256%256,
			VM_PUB_IP/256%256,		VM_PUB_IP%256);        	

	char str_query[512];
        sprintf( str_query, "delete from %s where VM_PUB_IP=\"%s\"",
			CTRL_VM_TABLE_NAME, str_IP_temp);

        if (mysql_query(con, str_query)){

		PRINT_ERR(con);
		return NULL;
	}

        mysql_close(con);
	return data;
}

const VM_ENTRY *ADD_VM_ENTRY( const VM_ENTRY *data_in ){

	// connect
        MYSQL *con = mysql_init(NULL);
        if (con == NULL) PRINT_ERR( con );
        if ( mysql_real_connect( con, MYSQL_host, MYSQL_user, MYSQL_pass,
                                MYSQL_dbase, MYSQL_port,
                                MYSQL_socket, MYSQL_client_flag ) == NULL) {

                PRINT_ERR( con );
		return NULL;
        }

        // Set query
	char	str_IP_temp_0[16];
	sprintf( str_IP_temp_0, "%d.%d.%d.%d", 
				data_in->PM_IP/256/256/256,
				data_in->PM_IP/256/256%256,
				data_in->PM_IP/256%256,
				data_in->PM_IP%256);        

	char	str_IP_temp_1[16];
	sprintf( str_IP_temp_1,	"%d.%d.%d.%d", 
				data_in->VM_PUB_IP/256/256/256,
				data_in->VM_PUB_IP/256/256%256,
				data_in->VM_PUB_IP/256%256,
				data_in->VM_PUB_IP%256);        
	
	char	str_IP_temp_2[16];
	sprintf( str_IP_temp_2,	"%d.%d.%d.%d", 
				data_in->VM_PVT_IP/256/256/256,
				data_in->VM_PVT_IP/256/256%256,
				data_in->VM_PVT_IP/256%256,
				data_in->VM_PVT_IP%256);        
	char str_query[512];
        sprintf( str_query, "insert into %s values('%s', '%s', '%s', %d, %d)",
				CTRL_VM_TABLE_NAME, 
				str_IP_temp_0, str_IP_temp_1, str_IP_temp_2,
				data_in->VM_CPU, data_in->VM_MEM);

        if (mysql_query(con, str_query)){

		PRINT_ERR( con );
		return NULL;
	}

        // finalization
        mysql_close(con);
	return data_in;
}
