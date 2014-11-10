#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "CTRL_data_op.h"
#include "ds_tcp_sock.h"

#ifndef BUFF_SIZE
#define BUFF_SIZE 256
#endif

/*
	argv[1]	CTRL_IP
	argv[2] CTRL_PORT

	argv[3]	WEB_IP
	argv[4]	WEB_PORT
*/

// EXTERN =====================================================
// ============================================================

const char             *MYSQL_host    = "localhost";
const char             *MYSQL_user    = "root";
const char             *MYSQL_pass    = "rtes";
const char             *MYSQL_dbase   = "CTRL_DATA";
const unsigned int     MYSQL_port     = 0;
const char             *MYSQL_socket  = NULL;
const unsigned long    MYSQL_client_flag = 0;

const char *CTRL_PM_TABLE_NAME = "PM_TABLE";
const char *CTRL_VM_TABLE_NAME = "VM_TABLE";

// Handlers ===================================================
// ============================================================

void sigchld_reaper( int sig_no ){

	int status;
	while( waitpid( -1, &status, WNOHANG ) > 0 );
}

// MAIN =======================================================
// ============================================================

int main( int argc, char **argv ){

	// Initialization =====================================
	printf( "CTRL_DAEMON\tInitailizing...\n" );

	printf( "%d\n", DROP_TABLE( CTRL_PM_TABLE_NAME ));
        printf( "%d\n", CREATE_PM_TABLE( ));

        printf( "%d\n", DROP_TABLE( CTRL_VM_TABLE_NAME ));
        printf( "%d\n", CREATE_VM_TABLE( ));

	// Handlers setting
	printf( "CMND_DAEMON\tSetting system handlers.\n" );
	signal( SIGCHLD, sigchld_reaper );

	// Service Initailizing -------------------------------
	printf( "CTRL_DAEMON\tService initializing...\n" );
	int     sock_ret;
        int     sock_fd;

        struct sockaddr_in      addr_cln;
        socklen_t               slen = sizeof(addr_cln);
        sock_fd = TCP_server_sock( atoi(argv[2]), 16 );

	// Start listening ====================================
        printf( "CTRL_DAEMON\tStart listening...\n" );

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
			write( conn_fd, str_res, 10);

		// Servant ====================================
                }else if( pid_servant == 0 ){

			// FD control
			close( sock_fd );

                        char arg_conn_fd[16];
                        sprintf( arg_conn_fd, "%d", conn_fd );
                        execl(  "CTRL_servant", "CTRL_servant", arg_conn_fd, 
				argv[1], argv[2], argv[3], argv[4],
				argv[5], argv[6], (char *)0 );
                        return 0;
                }

		// FD_control
		close( conn_fd );
	}
	
	// Finalization =======================================
	return 0;
}

