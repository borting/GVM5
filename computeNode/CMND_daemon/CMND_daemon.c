#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CMND_data_op.h"

#include "ds_sem.h"
#include "ds_tcp_sock.h"

#define THREAD_NUM	16
#ifndef BUFF_SIZE
#define BUFF_SIZE 256
#endif

/*
	argv[1]	CMND_IP
	argv[2] CMND_SERVANT_PORT
	
	argv[3] CMND_NUM_VM
	argv[4] CMND_NUM_CPU
	argv[5] CMND_NUM_MEM

	argv[6] CTRL_IP
	argv[7] CTRL_TRACKER_PORT

	argv[8] CMND_SEM_CORE_DATA
	argv[9] CMND_PM_FILE
	argv[10]CMND_VM_FILE
*/

// System data ################################################

long int SEM_CORE_DATA;
int PM_STAT[PM_COL];
int *VM_STAT;

int pid_heartbeat;

// Sysyem handlers ############################################

void sigint_handler(int sig_no){

	// Remove semaphore
        int sem_id = ds_sem_get( SEM_CORE_DATA );
        ds_sem_rmv( sem_id );
		
	// Kill process
        kill( pid_heartbeat, SIGINT );
        exit(0);
}

void sigchld_reaper( int sig_no ){

	int status;
	while( waitpid( -1, &status, WNOHANG ) > 0 );
}

// MAIN #######################################################

int main( int argc, char **argv ){

	// Parsing ============================================
	


	// Initialization =====================================
        printf( "CMND_DAEMON\tInitailizing...\n" );

	SEM_CORE_DATA = atol( argv[8] ) ;
	
	// Handlers setting 
        printf( "CMND_DAEMON\tSetting system handlers.\n" );

        struct sigaction sigint_sa;
        memset( &sigint_sa, 0, sizeof( sigint_sa ) );
        sigint_sa.sa_handler = sigint_handler;
        sigaction( SIGINT, &sigint_sa, NULL );

	signal( SIGCHLD, sigchld_reaper );

	// PM, VM data
        printf( "CMND_DAEMON\tInitializing data.\n" );

        PM_STAT[0] = atoi(argv[3]); PM_STAT[1] = 0;
        PM_STAT[2] = atoi(argv[4]); PM_STAT[3] = 0;
        PM_STAT[4] = atoi(argv[5]); PM_STAT[5] = 0;

        VM_STAT = (int *)malloc(sizeof(int)*PM_STAT[0]*VM_COL);
	for( int idx = 0 ; idx < PM_STAT[0] ; idx++ ){

                VM_STAT[idx*VM_COL] = idx;
                VM_STAT[idx*VM_COL+2] = atoi(argv[2])+1+idx;
        }

	//DISP_DATA( PM_STAT, VM_STAT );
        SAVE_DATA( argv[9], PM_STAT, argv[10], VM_STAT );
	free( VM_STAT );
	DISP_FILE( argv[9], argv[10] );

	// Heartbeat ==========================================

        pid_heartbeat = fork();
        if( pid_heartbeat < 0 ){

                return -1;

        }else if( pid_heartbeat == 0 ){

                execl(	"CMND_heartbeat", "CMND_heartbeat",
                        argv[1], argv[2], argv[6], argv[7], NULL);
                
		return 0;
        }

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

	// Service initializing ===============================
        printf( "CMND_DAEMON\tService initializing...\n" );

        int     sock_ret;
        int     sock_fd;

        struct sockaddr_in      addr_cln;
        socklen_t               slen = sizeof(addr_cln);
        sock_fd = TCP_server_sock( atoi(argv[2]), 16 );

	// Start listening ====================================
        printf( "CMND_DAEMON\tStart listening...\n" );

	int	conn_fd;
	int	conn_ret;
	char	conn_buff[BUFF_SIZE];

	while(1){
	
		conn_fd = accept( sock_fd, (struct sockaddr *)&addr_cln, &slen );
		if( conn_fd == -1 ){

			printf("CMND\tConnection error.\n");
			continue;
		}
		
		int pid_servant = fork();
		if( pid_servant < 0 ){

			char str_res[BUFF_SIZE]; 
			memset( str_res, 0, BUFF_SIZE );
			read( conn_fd, str_res, BUFF_SIZE );
			memset( str_res+2, 0, BUFF_SIZE-2 );
			write( conn_fd, str_res, 10);
		
		}else if( pid_servant == 0 ){

			// Client release
			close(sock_fd);

			// printf("(%d)\n", getpid());
			char arg_conn_fd[16];
			sprintf( arg_conn_fd, "%d", conn_fd );
			execl( 	"CMND_servant", "CMND_servant", arg_conn_fd, 
				argv[8], argv[9], argv[10], argv[1], argv[2], (char *)0 );
			return 0;
		}

		// Host release
		close( conn_fd );
	}

	// Finalization =======================================
        printf( "CMND_DAEMON\tFinalizing...\n" );

	// Kill child process
	kill( pid_heartbeat, SIGINT );
        
	// Remove IPC
        sem_id = ds_sem_get( SEM_CORE_DATA );
        ds_sem_rmv( sem_id );

	// Free memory
	free( VM_STAT );

	// Close server
	close( sock_fd );

        return 0;
}
