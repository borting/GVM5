#include "CMND_services.h"

// System data ################################################

long int SEM_CORE_DATA;
int pid_heartbeat;

// Sysyem handlers ############################################

void sigint_handler(int sig_no){

	// Remove semaphore
	printf("KILLED\n");
	int sem_id = ds_sem_get( SEM_CORE_DATA );
	ds_sem_rmv( sem_id );

	// Kill process
	kill( pid_heartbeat, SIGINT );
	exit(0);
}

// ############################################################

int main( int argc, char **argv ){

	// Initial ============================================
	SEM_CORE_DATA = 19900108;

	int PM_STAT[PM_COL];
	PM_STAT[0] = 3;		PM_STAT[1] = 0;
	PM_STAT[2] = 16;	PM_STAT[3] = 0;
	PM_STAT[4] = 1024;	PM_STAT[5] = 0;

	int *VM_STAT = (int *)malloc(sizeof(int)*PM_STAT[0]*VM_COL);
	for( int idx = 0 ; idx < PM_STAT[0] ; idx++ ){
		
		VM_STAT[idx*VM_COL] = idx;
		VM_STAT[idx*VM_COL+2] = 30010 + idx;
	}
	SAVE_DATA( "PM.temp", PM_STAT, "VM.temp", VM_STAT );
	DISP_FILE( "PM.temp", "VM.temp" );
	free( VM_STAT );

	// Handlers setting
	printf( "CMND_DAEMON\tSetting system handlers.\n" );

	struct sigaction sigint_sa;
	memset( &sigint_sa, 0, sizeof( sigint_sa ) );
	sigint_sa.sa_handler = sigint_handler;
	sigaction( SIGINT, &sigint_sa, NULL );

	// Semaphore setting
	printf("CMND_DAEMON\tSetting semaphores.\n");
	int sem_id = ds_sem_new( SEM_CORE_DATA ,1 );

	if( sem_id < 0){

		// If sem existed.
		ds_sem_rmv( ds_sem_get(SEM_CORE_DATA) );
		sem_id = ds_sem_new( SEM_CORE_DATA ,1 );
		if(sem_id < 0){

			printf("CMND_DAEMON\tSemaphore creating error.\n");
			return -1;
		}
	}

	// Test ===============================================

	unsigned char snd_buff[BUFF_SIZE];
	unsigned char rcv_buff[BUFF_SIZE];

	// START ==============================================
	puts("Test CREATE");

	for( int idx = 0 ; idx < 10 ; idx++ ){
	
		printf("> %d ", idx);
		scanf("%d", (int *)(rcv_buff+idx));
	}

	memset( snd_buff, 0, BUFF_SIZE );
	CMND_REQ_START( SEM_CORE_DATA, "PM.temp", "VM.temp", rcv_buff, snd_buff );
	for( int idx = 0 ; idx < 10 ; idx++ ) printf( "< %d [%hhu]\n", idx, snd_buff[idx]);

	// RESTART ============================================
	puts("Test RESTART");

	for( int idx = 0 ; idx < 10 ; idx++ ){
	
		printf("> %d ", idx);
		scanf("%d", (int *)(rcv_buff+idx));
	}

	memset( snd_buff, 0, BUFF_SIZE );
	CMND_REQ_REBOOT( SEM_CORE_DATA, "PM.temp", "VM.temp", rcv_buff, snd_buff );
	for( int idx = 0 ; idx < 10 ; idx++ ) printf("< %d [%hhu]\n", idx, snd_buff[idx]);
	
	// SHUTDOWN ===========================================
	puts("Test SHUTDOWN");

	for( int idx = 0 ; idx < 10 ; idx++ ){
	
		printf("> %d ", idx);
		scanf("%d", (int *)(rcv_buff+idx));
	}
	
	memset( snd_buff, 0, BUFF_SIZE );
	CMND_REQ_SHUTDOWN( SEM_CORE_DATA, "PM.temp", "VM.temp", rcv_buff, snd_buff );
	for( int idx = 0 ; idx < 10 ; idx++ ) printf("< %d [%hhu]\n", idx, snd_buff[idx]);

	// RESTORE ============================================
	puts("Test RESTORE");

	for( int idx = 0 ; idx < 10 ; idx++ ){
	
		printf("> %d ", idx);
		scanf("%d", (int *)(rcv_buff+idx));
	}

	memset( snd_buff, 0, BUFF_SIZE );
	CMND_REQ_RESTORE( SEM_CORE_DATA, "PM.temp", "VM.temp", rcv_buff, snd_buff );
	for( int idx = 0 ; idx < 10 ; idx++ ) printf("< %d [%hhu]\n", idx, snd_buff[idx]);

	// Finalization =======================================	

	// Remove IPC
        sem_id = ds_sem_get( SEM_CORE_DATA );

	return 0;
}
