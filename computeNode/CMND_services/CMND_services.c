#include "CMND_services.h"

int CMND_REQ_HEARTBEAT(	const long int sem_key,
			const char *PM_NAME,
			const char *VM_NAME,

			char *str_cmd,
			char *str_res){

	// Initializing =======================================
	int sem_id = ds_sem_get( sem_key );

	// Get data ===========================================
	unsigned int pm_stat[PM_COL];		
	
	// ####################################################
	ds_sem_lock( sem_id );

	LOAD_PM_DATA( PM_NAME, pm_stat );

	ds_sem_unlock( sem_id );
	// ####################################################
	
	// Generate output ====================================

	// RET + NOUNCE
	str_res[0] = str_cmd[0];
	str_res[1] = str_cmd[1];

	// IMG
	str_res[6] = pm_stat[0] - pm_stat[1];	// IMG LEFT
	str_res[7] = pm_stat[2] - pm_stat[3];	// CPU LEFT

	unsigned int mem_temp;
	mem_temp = pm_stat[4] - pm_stat[5];	// MEM LEFT
	str_res[8] = mem_temp / 256 ;		
	str_res[9] = mem_temp % 256 ;	
	
	// DISPLAY and RETURN =================================
	DISP_FILE( PM_NAME, VM_NAME );
	return 0;
}	

int CMND_REQ_START(	const long int sem_key,
			const char *PM_NAME,
			const char *VM_NAME,

			char *str_cmd,
			char *str_res){
			
	// Initializing =======================================
	int sem_id = ds_sem_get( sem_key );

	int temp_cpu = (int)str_cmd[2]&0XFF;
	int temp_mem = (int)(str_cmd[3]&0XFF)<<8|(int)str_cmd[4]&0XFF;

	// GET VM ID ==========================================
	unsigned int VM_ROW[VM_COL]; 
	
	// ####################################################
	ds_sem_lock( sem_id );
	
	int VM_RET = GET_VM_ID_AVAILABLE( PM_NAME, VM_NAME, VM_ROW );
	if( VM_RET > -1 ){

		VM_ROW[1] = VM_RUNNING|VM_DIRTY;
		VM_ROW[3] = temp_cpu;
		VM_ROW[4] = temp_mem;	

		// Save FILE
		VM_RET = SET_VM_ID( PM_NAME, VM_NAME, VM_RET, VM_ROW );
	}	
	
	ds_sem_unlock( sem_id );
	// ####################################################

	// OPERATION CHECK
	if( VM_RET < 0 ) return VM_RET;

	// START VM
	VM_RET = CMND_VM_CREATE( VM_RET, temp_cpu, temp_mem );
	if( VM_RET < 0 ){

		// ############################################
		ds_sem_lock( sem_id );

		VM_ROW[1] = VM_READY;
		VM_RET = SET_VM_ID( PM_NAME, VM_NAME, VM_RET, VM_ROW );

		ds_sem_unlock( sem_id );
		// ############################################	

		return VM_RET;
	}

	// Get VM IP ==========================================
	struct          sockaddr_in addr_cln;
	socklen_t       sock_len = sizeof(addr_cln);
	int             sock_fd = TCP_server_sock( VM_ROW[2], 1 );

	char conn_buff[BUFF_SIZE];
	int conn_fd = accept( sock_fd, (struct sockaddr *)&addr_cln, &sock_len);

	memset( conn_buff, 0, BUFF_SIZE );
	int conn_ret = read( conn_fd, conn_buff, 8 );

	close( conn_fd );
	shutdown( sock_fd, SHUT_RDWR );
	close( sock_fd );

	unsigned int PVT_IP_temp = conn_buff[0]&0XFF;
	PVT_IP_temp = (PVT_IP_temp << 8) + (conn_buff[1]&0XFF);
	PVT_IP_temp = (PVT_IP_temp << 8) + (conn_buff[2]&0XFF);
	PVT_IP_temp = (PVT_IP_temp << 8) + (conn_buff[3]&0XFF);
	
	unsigned int PUB_IP_temp = conn_buff[4]&0XFF;
	PUB_IP_temp = (PUB_IP_temp << 8) + (conn_buff[5]&0XFF);
	PUB_IP_temp = (PUB_IP_temp << 8) + (conn_buff[6]&0XFF);
	PUB_IP_temp = (PUB_IP_temp << 8) + (conn_buff[7]&0XFF);

	// ####################################################
	ds_sem_lock( sem_id );

	// Update Data
	VM_RET = GET_VM_ID( PM_NAME, VM_NAME, VM_RET, VM_ROW );
	if( VM_RET > -1 ){

		VM_ROW[5] = PVT_IP_temp;
		VM_ROW[6] = PUB_IP_temp;

		// Save FILE
		VM_RET = SET_VM_ID( PM_NAME, VM_NAME, VM_RET, VM_ROW );
	}	
	
	ds_sem_unlock( sem_id );
	// ####################################################

	// OPERATION CHECK
	if( VM_RET < 0 ) return VM_RET;

	// Generate OUTPUT
	str_res[0] = 0;
	str_res[1] = str_cmd[1];
	
	str_res[2] = conn_buff[0]; str_res[3] = conn_buff[1]; 
	str_res[4] = conn_buff[2]; str_res[5] = conn_buff[3];
	
	str_res[6] = conn_buff[4]; str_res[7] = conn_buff[5];
	str_res[8] = conn_buff[6]; str_res[9] = conn_buff[7];

	// DISPLAY and RETURN =================================
	DISP_FILE( PM_NAME, VM_NAME );

	if( VM_RET < 0 )	return VM_RET;
	else			return VM_RET;	
}

int CMND_REQ_RESTORE( 	const long int sem_key,
			const char *PM_NAME,
			const char *VM_NAME,

			char *str_cmd,
			char *str_res){
		
	// Initializing =======================================
	int sem_id = ds_sem_get( sem_key );
	int vm_id = str_cmd[2]&0XFF;

	// RESTORE ============================================	
	int VM_RET = CMND_VM_RESTORE( vm_id );
	if( VM_RET < 0 ) return VM_RET;
	
	// Update Data
	unsigned int VM_ROW[VM_COL];

	// ####################################################
	ds_sem_lock( sem_id );	
	
	VM_RET = GET_VM_ID( PM_NAME, VM_NAME, vm_id, VM_ROW );
	
	if( VM_RET > -1 ){

		VM_ROW[1] = VM_ROW[1]^0X02;
		VM_RET = SET_VM_ID( PM_NAME, VM_NAME, VM_RET, VM_ROW );
	}	
	
	ds_sem_unlock( sem_id );
	// ####################################################

	// DISPLAY and RETURN =================================
	DISP_FILE( PM_NAME, VM_NAME );

	if( VM_RET < 0 )	return VM_RET;
	else			return VM_RET;	
}

int CMND_REQ_SHUTDOWN(	const long int sem_key,
			const char *PM_NAME,
			const char *VM_NAME,
			
			char *str_cmd,
			char *str_res){

	// Initializing =======================================
	int sem_id = ds_sem_get( sem_key );
	
	int vm_ip = str_cmd[2]&0XFF;
	vm_ip = ( vm_ip << 8 ) | str_cmd[3]&0XFF;
	vm_ip = ( vm_ip << 8 ) | str_cmd[4]&0XFF;
	vm_ip = ( vm_ip << 8 ) | str_cmd[5]&0XFF;


	// Get VM_ID ==========================================
	unsigned int VM_ROW[VM_COL];
	
	// Lock core data #####################################
	ds_sem_lock( sem_id );
	
	int VM_RET = GET_VM_ID_BY_IP( PM_NAME, VM_NAME, vm_ip, VM_ROW );
	
	ds_sem_unlock( sem_id );
	// ####################################################

	// Shutdown ===========================================
	if( VM_RET < 0 ) return VM_RET;
	VM_RET = CMND_VM_SHUTDOWN( VM_RET );
	if( VM_RET < 0 ) return VM_RET;


	// Update Data ========================================
	
	// Lock core data #####################################
	ds_sem_lock( sem_id );

	VM_RET = GET_VM_ID_BY_IP( PM_NAME, VM_NAME, vm_ip, VM_ROW );

	ds_sem_unlock( sem_id );
	// ####################################################

	if( VM_RET > -1 ){

		VM_ROW[1] = VM_ROW[1]^0X01;

		// Save FILE
		VM_RET = SET_VM_ID( PM_NAME, VM_NAME, VM_RET, VM_ROW );
	}		
	
	// Generate OUTPUT ====================================
	str_res[0] = 0;
	for( int idx = 1 ; idx < 10 ; idx++ ) str_res[idx] = str_cmd[idx];

	// DISPLAY and RETURN =================================
	DISP_FILE( PM_NAME, VM_NAME );

	if( VM_RET < 0 )	return VM_RET;
	else			return VM_RET;	
}

int CMND_REQ_REBOOT(	const long int sem_key,
			const char *PM_NAME,
			const char *VM_NAME,

			char *str_cmd,
			char *str_res){

	// Parsing ============================================
	int sem_id = ds_sem_get( sem_key );

	int vm_ip = str_cmd[2]&0XFF;
	vm_ip = ( vm_ip << 8 ) | str_cmd[3]&0XFF;
	vm_ip = ( vm_ip << 8 ) | str_cmd[4]&0XFF;
	vm_ip = ( vm_ip << 8 ) | str_cmd[5]&0XFF;

	// Get data ===========================================
	
	unsigned int VM_ROW[VM_COL];
	int CPU_temp;
	int MEM_temp;
	
	// Lock core data #####################################
	ds_sem_lock( sem_id );
	
	int VM_RET = GET_VM_ID_BY_IP( PM_NAME, VM_NAME, vm_ip, VM_ROW );
	if( VM_RET < 0 ) return VM_RET;

	ds_sem_unlock( sem_id );
	// ####################################################

	if( VM_RET > -1 ){

		CPU_temp = VM_ROW[3];
		MEM_temp = VM_ROW[4];	

	}else return VM_RET; // Failed

	// Restart ============================================
	VM_RET = CMND_VM_REBOOT( VM_RET, CPU_temp, MEM_temp );
	if( VM_RET < 0 ) return VM_RET; 	
	
	// Get VM IP ==========================================
	struct          sockaddr_in addr_cln;
	socklen_t       sock_len = sizeof(addr_cln);
	int             sock_fd = TCP_server_sock( VM_ROW[2], 1 );

	char conn_buff[BUFF_SIZE];
	int conn_fd = accept( sock_fd, (struct sockaddr *)&addr_cln, &sock_len);

	memset( conn_buff, 0, BUFF_SIZE );
	int conn_ret = read( conn_fd, conn_buff, 8 );

	close( conn_fd );
	shutdown( sock_fd, SHUT_RDWR );
	close( sock_fd );

	unsigned int PVT_IP_temp = conn_buff[0]&0XFF;
	PVT_IP_temp = (PVT_IP_temp << 8) + conn_buff[1]&0XFF;
	PVT_IP_temp = (PVT_IP_temp << 8) + conn_buff[2]&0XFF;
	PVT_IP_temp = (PVT_IP_temp << 8) + conn_buff[3]&0XFF;
	
	unsigned int PUB_IP_temp = conn_buff[4]&0XFF;
	PUB_IP_temp = (PUB_IP_temp << 8) + conn_buff[5]&0XFF;
	PUB_IP_temp = (PUB_IP_temp << 8) + conn_buff[6]&0XFF;
	PUB_IP_temp = (PUB_IP_temp << 8) + conn_buff[7]&0XFF;
	
	// Generate OUTPUT ====================================
	str_res[0] = 0;
	str_res[1] = str_cmd[1];
	
	str_res[2] = conn_buff[0]; str_res[3] = conn_buff[1]; 
	str_res[4] = conn_buff[2]; str_res[5] = conn_buff[3];
	
	str_res[6] = conn_buff[4]; str_res[7] = conn_buff[5];
	str_res[8] = conn_buff[6]; str_res[9] = conn_buff[7];

	// DISPLAY and RETURN =================================
	DISP_FILE( PM_NAME, VM_NAME );

	if( VM_RET < 0 )	return VM_RET;
	else			return VM_RET;		
}
