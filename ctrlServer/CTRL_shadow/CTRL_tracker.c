#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "CTRL_headers/CTRL_data.h"
#include "CTRL_headers/CTRL_service.h"

#ifndef BUFF_SIZE
#define BUFF_SIZE 64
#endif

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

        // Initializaion
        int conn_fd = atoi(argv[1]);

        // Get command ----------------------------------------
        unsigned char    conn_buff[BUFF_SIZE]; memset( conn_buff, 0, sizeof(conn_buff));
        int     conn_ret = read( conn_fd, conn_buff, BUFF_SIZE );
        if( conn_ret <= 0 ){

                printf("\nCMND[%d] Data reading error.\n", getpid());
                close(conn_fd);
                return -1;
        }

        // Decision section -----------------------------------
        
	// CMND_HEARTBEAT
        if( conn_buff[0] == 0 ){

		// 
		printf("CTRL[%d] TRACKER received a CMND heartbeat.\n", getpid());
		for( int idx = 0 ; idx < 10 ; idx++ ) printf("[%hhu]", conn_buff[idx]);
		printf("\n");

		PM_ENTRY PM_temp;
		PM_temp.PM_IP = 0;
		PM_temp.PM_IP = conn_buff[2]*256 + conn_buff[3];
		PM_temp.PM_IP = PM_temp.PM_IP*256 + conn_buff[4];
		PM_temp.PM_IP = PM_temp.PM_IP*256 + conn_buff[5];
		PM_temp.IMG_LEFT = conn_buff[6];
		PM_temp.IMG_USED = 0;
		PM_temp.CPU_LEFT = conn_buff[7];
		PM_temp.CPU_USED = 0;
		PM_temp.MEM_LEFT = conn_buff[8]*256+conn_buff[9];
		PM_temp.MEM_USED = 0;

		printf("%u %u %u %u\n",PM_temp.PM_IP,PM_temp.IMG_LEFT,PM_temp.CPU_LEFT,PM_temp.MEM_LEFT);

		HEARTBEAT_CMND( &PM_temp );
	}	
	// Something wrong
        else{

                printf("CTRL\tSomething wrong.\n");
        }

        // Finalization
        return 0;
}
