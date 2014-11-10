#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <sys/time.h>

#include "ds_tcp_sock.h"

#ifndef BUFF_SIZE
#define BUFF_SIZE 256
#endif

#define CMND_HEARTBEAT_PERIOD 30

void heartbeat_handler( int sig_num ){}

/*      argv[0]
       
	argv[1] CMND_IP
	argv[2] CMND_PORT
 
        argv[3] CTRL_IP
        argv[4] CTRL_PORT
*/

// MAIN #######################################################
int main( int argc, char **argv ){

	// Argument initialization ----------------------------

	// Timer setting --------------------------------------
	printf("Heartbeat\tSetting heartbeat.\n");
	
	// Working loop ---------------------------------------
	printf("Heartbeat\tStart working.\n");

	unsigned char conn_buff[BUFF_SIZE]; 
	memset( conn_buff, 0, BUFF_SIZE );

	while(1){

		// Get heartbeat
		int conn_fd = TCP_client_sock( argv[1], atoi(argv[2]) );
		int conn_ret = write( conn_fd, conn_buff, 10 );
			
		conn_ret = read( conn_fd, conn_buff, BUFF_SIZE );
		close( conn_fd );
	
		// Translation
		conn_buff[1] = 48;

		// Send heartbeat
		conn_fd = TCP_client_sock( argv[3], atoi(argv[4]) );
		conn_ret = write( conn_fd, conn_buff, 10 );

		close( conn_fd );

		// Sleep
		sleep( CMND_HEARTBEAT_PERIOD );
	}

	// Finalization
	return 0;
}	
