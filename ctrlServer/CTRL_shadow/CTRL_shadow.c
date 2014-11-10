#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CTRL_headers/CTRL_data.h"
#include "CTRL_headers/CTRL_service.h"

#include "ds_lib/ds_sem.h"
#include "ds_lib/ds_tcp_sock.h"

#ifndef BUFF_SIZE
#define BUFF_SIZE       64
#endif

/*
	argv[1]	CTRL_TRACKER_IP
	argv[2] CTRL_TRACKER_PORT
*/

char             *MYSQL_host    = "localhost";
char             *MYSQL_user    = "root";
char             *MYSQL_pass    = "temp";
char             *MYSQL_dbase   = "CTRL_DATA";
unsigned int     MYSQL_port     = 0;
char             *MYSQL_socket  = NULL;
unsigned long    MYSQL_client_flag = 0;

char *CTRL_PM_TABLE_NAME = "PM_TABLE";
char *CTRL_VM_TABLE_NAME = "VM_TABLE";

int main( int argc, char **argv ){

	// Initialization =====================================
	printf( "CTRL_TRACKER\tInitailizing...\n" );

	printf( "%d\n", DROP_TABLE( CTRL_PM_TABLE_NAME ));
        printf( "%d\n", CREATE_PM_TABLE( ));

        printf( "%d\n", DROP_TABLE( CTRL_VM_TABLE_NAME ));
        printf( "%d\n", CREATE_VM_TABLE( ));
	
	// Service Initailizing -------------------------------
	printf( "CTRL_TRACKER\tService initializing...\n" );
	
	int     sock_ret;
        int     sock_fd;

        struct sockaddr_in      addr_cln;
        socklen_t               slen = sizeof(addr_cln);
        sock_fd = TCP_server_sock( atoi(argv[2]), 16 );

	// Start listening ====================================
        printf( "CTRL_TRACKER\tStart listening...\n" );

        int     conn_fd;
        int     conn_ret;
        char    conn_buff[BUFF_SIZE];

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
                        memset( str_res+8, 0, BUFF_SIZE-8 );
                        str_res[0] = '1';
			sprintf( str_res+8, "0.0.0.0" );
			write( conn_fd, str_res, strlen(str_res));

		// Servant ====================================
                }else if( pid_servant == 0 ){

                        char arg_conn_fd[16];
                        sprintf( arg_conn_fd, "%d", conn_fd );
                        execl(  "CTRL_tracker", "CTRL_tracker", arg_conn_fd, (char *)0 );
                        return 0;
                }

		close( conn_fd );
	}
	
	// Finalization =======================================
	return 0;
}

