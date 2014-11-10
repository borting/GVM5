#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

#include "CTRL_data_op.h"
#include "CTRL_services.h"

#ifndef BUFF_SIZE 
#define BUFF_SIZE 256
#endif

// EXTERN =====================================================
// ============================================================

// WEB
const char *WEB_IP = "192.168.30.1";
const unsigned short	WEB_PORT = 80;

// VAPPD
const unsigned short	VAPPD_PORT = 10000;

// MYSQL
const char             *MYSQL_host    = "localhost";
const char             *MYSQL_user    = "root";
const char             *MYSQL_pass    = "rtes";
const char             *MYSQL_dbase   = "CTRL_DATA";
const unsigned int     MYSQL_port     = 0;
const char             *MYSQL_socket  = NULL;
const unsigned long    MYSQL_client_flag = 0;

const char *CTRL_PM_TABLE_NAME = "PM_TABLE";
const char *CTRL_VM_TABLE_NAME = "VM_TABLE";

// DNAT
const char              *DNAT_IP = "192.168.30.1";
const unsigned short    DNAT_PORT = 10000;

// CMND
const unsigned int CMND_PORT = 30010;

// MAIN =======================================================
// ============================================================

int main( int argc, char **argv ){

	// Initializaion
	int conn_fd = atoi(argv[1]);

	// Get command
        unsigned char conn_buff[BUFF_SIZE]; memset( conn_buff, 0, sizeof(conn_buff));
        int conn_ret = read( conn_fd, conn_buff, BUFF_SIZE );
        if( conn_ret <= 0 ){

                printf("\nCMND[%d] Data reading error.\n", getpid());
                close(conn_fd);
                return -1;
        }

	// REQ_heartbeat ======================================
	// ====================================================
	if( conn_buff[1] == '0' ){
		
		PM_ENTRY PM_temp;
		PM_temp.PM_IP = conn_buff[2];
		PM_temp.PM_IP = PM_temp.PM_IP*256 + conn_buff[3];
		PM_temp.PM_IP = PM_temp.PM_IP*256 + conn_buff[4];
		PM_temp.PM_IP = PM_temp.PM_IP*256 + conn_buff[5];

		PM_temp.IMG_LEFT = conn_buff[6];
		PM_temp.CPU_LEFT = conn_buff[7];
		PM_temp.MEM_LEFT = conn_buff[8]*256 + conn_buff[9];

		PM_temp.IMG_USED = 0;
		PM_temp.CPU_USED = 0;
		PM_temp.MEM_USED = 0;

		MYSQL_heartbeat_PM( &PM_temp, PM_temp.PM_IP );
	}

	// REQ_Start ==========================================
	// ====================================================
	else if( conn_buff[2] == '1' ){

		// Parsing
		unsigned int num_temp, cpu_temp, mem_temp;
		sscanf(conn_buff ,"0 1 %u %u %u", &num_temp, &cpu_temp, &mem_temp);

		// Display ------------------------------------
		// --------------------------------------------
		printf("\nCTRL[%u]\tREQ_start. [%u,%u]\n", num_temp, cpu_temp, mem_temp);

                unsigned char conn_res[BUFF_SIZE]; memset( conn_res, 0, BUFF_SIZE );
		unsigned int IP_temp = CREATE_VM( cpu_temp, mem_temp );

		// Updata IP to Router
		unsigned char *IP_ptr = (unsigned char *)&IP_temp;

		conn_buff[0] = 2; // Register
		conn_buff[1] = *(IP_ptr+3);	conn_buff[2] = *(IP_ptr+2);
		conn_buff[3] = *(IP_ptr+1);	conn_buff[4] = *(IP_ptr);
		conn_buff[5] = 0; 		conn_buff[6] = 0;
		conn_buff[7] = 0; 		conn_buff[8] = 0;
				
		// Response to WEB
		if( IP_temp ){

			unsigned char *IP_ptr = (char *)&IP_temp;
			sprintf(conn_res, "1 1 %04u %u.%u.%u.%u", 
				num_temp, *(IP_ptr+3), *(IP_ptr+2), *(IP_ptr+1), *IP_ptr);

printf( "%s\n", conn_res );

			conn_ret = write( conn_fd, conn_res, strlen(conn_res) );
		}else{
			sprintf( conn_res, "1 1 %04u 0.0.0.0", num_temp );

printf( "%s\n", conn_res );

			conn_ret = write( conn_fd, conn_res, strlen(conn_res) );
		}	
	}	

	// REQ_Shutdown =======================================
	// ====================================================
	else if( conn_buff[2] == '2' ){

                // Parsing
                unsigned int num_temp, IP, IP_temp[4];
                sscanf(conn_buff ,"0 2 %u %u.%u.%u.%u", 
				&num_temp, IP_temp, IP_temp+1, IP_temp+2, IP_temp+3);
		
		IP = ((IP_temp[0]*256 + IP_temp[1])*256 + IP_temp[2])* 256 + IP_temp[3];

		// Display ------------------------------------
		// --------------------------------------------
                printf("\nCTRL[%u]\tREQ_shutdown.\n", num_temp);

                char conn_res[BUFF_SIZE]; memset( conn_res, 0, BUFF_SIZE );
                IP = SHUTDOWN_VM( IP );

		// Notify Router
		unsigned char *IP_ptr = (unsigned char *)&IP;

		conn_buff[0] = 3; // Log off
		conn_buff[1] = *(IP_ptr+3);	conn_buff[2] = *(IP_ptr+2);
		conn_buff[3] = *(IP_ptr+1);	conn_buff[4] = *(IP_ptr);
		conn_buff[5] = 0; 		conn_buff[6] = 0;
		conn_buff[7] = 0; 		conn_buff[8] = 0;
				
		// Response to WEB
                if( IP ){

			IP_ptr = (char *)&IP;
			sprintf(conn_res, "1 2 %04u %u.%u.%u.%u", 
				num_temp, *(IP_ptr+3), *(IP_ptr+2), *(IP_ptr+1), *IP_ptr);


printf( "%s\n", conn_res );

			conn_ret = write( conn_fd, conn_res, strlen(conn_res) );
                }else{
                        sprintf( conn_res, "1 2 %04u 0.0.0.0", num_temp );

printf( "%s\n", conn_res );

                        conn_ret = write( conn_fd, conn_res, strlen(conn_res) );
                }
        }

	// REQ_Restart ========================================
	// ====================================================
	else if( conn_buff[2] == '3' ){

                // Parsing
                unsigned int num_temp, IP, IP_temp[4];
                sscanf(conn_buff ,"0 3 %u %u.%u.%u.%u",
                                &num_temp, IP_temp, IP_temp+1, IP_temp+2, IP_temp+3);

                IP = ((IP_temp[0]*256 + IP_temp[1])*256 + IP_temp[2])* 256 + IP_temp[3];

		// Notify Router
		unsigned char *IP_ptr = (unsigned char *)&IP;

		conn_buff[0] = 3; // Log off
		conn_buff[1] = *(IP_ptr+3);	conn_buff[2] = *(IP_ptr+2);
		conn_buff[3] = *(IP_ptr+1);	conn_buff[4] = *(IP_ptr);
		conn_buff[5] = 0; 		conn_buff[6] = 0;
		conn_buff[7] = 0; 		conn_buff[8] = 0;
			
		// Display ------------------------------------
		// --------------------------------------------
                printf("\nCTRL[%u]\tREQ_restart.\n", num_temp);

                char conn_res[BUFF_SIZE]; memset( conn_res, 0, BUFF_SIZE );
                IP = REBOOT_VM( IP );

		// Notify Router
		IP_ptr = (unsigned char *)&IP;

		conn_buff[0] = 2; // Register
		conn_buff[1] = *(IP_ptr+3);	conn_buff[2] = *(IP_ptr+2);
		conn_buff[3] = *(IP_ptr+1);	conn_buff[4] = *(IP_ptr);
		conn_buff[5] = 0; 		conn_buff[6] = 0;
		conn_buff[7] = 0; 		conn_buff[8] = 0;
				
		// Response to WEB
                if( IP ){

                        IP_ptr = (char *)&IP;
                        sprintf(conn_res, "1 3 %04u %u.%u.%u.%u",
                                num_temp, *(IP_ptr+3), *(IP_ptr+2), *(IP_ptr+1), *IP_ptr);


printf( "%s\n", conn_res );

                        conn_ret = write( conn_fd, conn_res, strlen(conn_res) );
                }else{
                        sprintf( conn_res, "1 3 %04u 0.0.0.0", num_temp );

printf( "%s\n", conn_res );

                        conn_ret = write( conn_fd, conn_res, strlen(conn_res) );
                }
        }

	// Start an APP =======================================
	// ====================================================
	else if( conn_buff[2]=='4' ){

                // Parsing
                unsigned int	num_temp, IP_temp[4];
		unsigned char 	cmd_temp[BUFF_SIZE];
                sscanf(	conn_buff ,"0 4 %u %u.%u.%u.%u %s", &num_temp, 
			IP_temp, IP_temp+1, IP_temp+2, IP_temp+3, cmd_temp );
		
		// Display ------------------------------------
		// --------------------------------------------
                printf("\nCTRL[%u]\tREQ_start_APP.\n", num_temp);

		int IP = ((IP_temp[0]*256 + IP_temp[1])*256 + IP_temp[2])* 256 + IP_temp[3];
		int VM_WWW_PORT = APP_START( IP, cmd_temp );

		memset( conn_buff, 0, sizeof( conn_buff ) );
		if( VM_WWW_PORT > 0 ){

			sprintf( conn_buff, "1 4 %u %hhu.%hhu.%hhu.%hhu %s %d", 
				num_temp, IP_temp[0], IP_temp[1], IP_temp[2], IP_temp[3],
				cmd_temp, VM_WWW_PORT );


printf( "%s\n", conn_buff );

			conn_ret = write( conn_fd, conn_buff, strlen(conn_buff) );
		}else{
			sprintf( conn_buff, "1 4 %u 0.0.0.0 %s 0", num_temp, cmd_temp );

printf( "%s\n", conn_buff );

			conn_ret = write( conn_fd, conn_buff, strlen(conn_buff) );
		}
	}

	// Close an APP =======================================
	// ====================================================
	else if( conn_buff[2]=='5' ){

		// Parsing
                unsigned int	num_temp, IP, IP_temp[4];
		unsigned short 	vm_pub_port_temp;
                sscanf(	conn_buff ,"0 5 %u %u.%u.%u.%u %hu", &num_temp, 
			IP_temp, IP_temp+1, IP_temp+2, IP_temp+3, &vm_pub_port_temp );

		// Display ------------------------------------
		// --------------------------------------------
                printf("\nCTRL[%u]\tREQ_close_APP(VAPPD).\n", num_temp);

		IP = ((IP_temp[0]<<8 | IP_temp[1])<<8 | IP_temp[2])<<8 | IP_temp[3];
		APP_CLOSE_VAPPD( IP, vm_pub_port_temp );
	}

	// Close an APP =======================================
	// ====================================================
	else if( conn_buff[2] == '6' ){
	
		// Parsing
                unsigned int	num_temp, IP, IP_temp[4];
		unsigned short 	vm_www_port_temp;
                sscanf(	conn_buff ,"0 6 %u %u.%u.%u.%u %hu", &num_temp, 
			IP_temp, IP_temp+1, IP_temp+2, IP_temp+3, &vm_www_port_temp );

		// Display ------------------------------------
		// --------------------------------------------
                printf("\nCTRL[%u]\tREQ_close_APP(%hu).\n", num_temp, vm_www_port_temp);
		APP_CLOSE( vm_www_port_temp );
	}

	// Something wrong ====================================
	// ====================================================
	else printf("CTRL\tSomething wrong.\n");

	// Finalization
	close( conn_fd );
	return 0;
}
