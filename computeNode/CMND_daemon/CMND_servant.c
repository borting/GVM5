#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "CMND_data_op.h"
#include "CMND_services.h"

#include "ds_sem.h"
#include "ds_tcp_sock.h"

#ifndef BUFF_SIZE
#define BUFF_SIZE	256
#endif

/*	argv[1] conn_fd;

	argv[2] CMND_SEM_CORE_DATA
	argv[3] PM_FILE;
	argv[4] VM_FILE;

	argv[5] CMND_IP
	argv[6] CMND_port
*/

void sigchld_reaper( int sig_no){
	
	int status;
	while( waitpid( -1, &status, WNOHANG ) > 0 );
}

// Servant ####################################################

int main( int argc, char **argv ){
/*
	printf("(%s)\n",argv[1]);
	printf("(%s)\n",argv[2]);
	printf("(%s)\n",argv[3]);
	printf("(%s)\n",argv[4]);
*/	

	// Initializing =======================================
	signal( SIGCHLD, sigchld_reaper );

	int conn_fd = atoi(argv[1]);

	// Get command ----------------------------------------
        unsigned char conn_snd_buff[BUFF_SIZE]; 
	memset( conn_snd_buff, 0, sizeof(conn_snd_buff));
        
	unsigned char conn_rcv_buff[BUFF_SIZE]; 
	memset( conn_rcv_buff, 0, sizeof(conn_rcv_buff));

        int conn_ret = read( conn_fd, conn_rcv_buff, BUFF_SIZE );
        if( conn_ret <= 0 ){

                printf("\nCMND[%d] Data reading error.\n", conn_fd);
                close(conn_fd);
		return -1;
        }
	
	// Decision section -----------------------------------

	// REQ_Heartbeat
	if( conn_rcv_buff[0] == 0 ){

		printf("\nCMND[%d]\tREQ_heartbeat.\n", conn_fd);
		CMND_REQ_HEARTBEAT( 	atol(argv[2]), argv[3], argv[4], 
					conn_rcv_buff, conn_snd_buff );

		unsigned char *IP_head = argv[5];
		unsigned char *IP_tail = argv[5];
		int idx = 2;
		while(1){

			if( *IP_tail++ == '.' ){
				conn_snd_buff[idx++] = atoi(IP_head);
				IP_head = IP_tail;
			
			}else if( *IP_tail == '\0' ){
				conn_snd_buff[idx] = atoi(IP_head);
				break;
			}
		}		

		// Response
		conn_ret = write( conn_fd, conn_snd_buff, 10 );
	
	// REQ_Start
	}else if( conn_rcv_buff[0] == 1 ){

		printf("\nCMND[%hhd]\tREQ_start.\n", conn_fd);
		int vm_id = CMND_REQ_START(	atol(argv[2]), argv[3], argv[4], 
						conn_rcv_buff, conn_snd_buff);
	
		if( vm_id < 0 ){

			printf( "CMND[%hhd]\tREQ_start failed.\n", conn_fd );
			memset( conn_snd_buff+2, 0, BUFF_SIZE-2 );
			conn_snd_buff[0] = 1; conn_snd_buff[1] = conn_rcv_buff[1];
		}

		// Response
		conn_ret = write( conn_fd, conn_snd_buff, 10  );

	// REQ_Shutdown
	}else if( conn_rcv_buff[0] == 2 ){

		printf("\nCMND[%hhd]\tREQ_shutdown.\n", conn_fd);
                int vm_id = CMND_REQ_SHUTDOWN(	atol(argv[2]), argv[3], argv[4], 
						conn_rcv_buff, conn_snd_buff);
	
		if( vm_id < 0 ){

			printf( "CMND[%hhd]\tREQ_start failed.\n", conn_fd );
			memset( conn_snd_buff+2, 0, BUFF_SIZE-2 );
			conn_snd_buff[0] = 1; conn_snd_buff[1] = conn_rcv_buff[1];
		}

		// Response
		conn_ret = write( conn_fd, conn_snd_buff, 10 );

		// Restore		
		if( vm_id > -1){

			conn_rcv_buff[2] = vm_id;
			printf("\nCMND[%hhd]\tREQ_RESTORE.\n", conn_fd);
                	CMND_REQ_RESTORE( 	atol(argv[2]), argv[3], argv[4], 
						conn_rcv_buff, conn_snd_buff);
		}
	
	// REQ_Reboot	
	}else if( conn_rcv_buff[0] == 3 ){
	
		printf("\nCMND[%hhd]\tREQ_restart.\n", conn_fd);
                int vm_id = CMND_REQ_REBOOT(	atol(argv[2]), argv[3], argv[4], 
						conn_rcv_buff, conn_snd_buff);
	
		if( vm_id < 0 ){

			printf( "CMND[%hhd]\tREQ_start failed.\n", conn_fd );
			memset( conn_snd_buff+2, 0, BUFF_SIZE-2 );
			conn_snd_buff[0] = 1; conn_snd_buff[1] = conn_rcv_buff[1];
		}

		// Response
		conn_ret = write( conn_fd, conn_snd_buff, 10 );
	
	// Something wrong
	}else{
		// Display
		printf("\nCMND\tSomething wrong.\n");
	}

	// Finalizing 
	close(conn_fd);
	return 0;
}
