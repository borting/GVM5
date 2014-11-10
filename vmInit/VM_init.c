#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ds_lib/ds_tcp_sock.h"

#define BUFF_SIZE 64

int main( int argc, char **argv ){

	printf("%s\n",argv[3]);
	printf("%s\n",argv[4]);	

        unsigned char    conn_buff[BUFF_SIZE];
        memset( conn_buff, 0, BUFF_SIZE );

	// PVT_IP
	int idx = 0;
	unsigned char *char_head = argv[3];
        unsigned char *char_tail = argv[3];
        while(1){

                if( *char_tail == '.' ){

			conn_buff[idx++] = atoi(char_head);
                        char_head = char_tail+1;
                }

                if( *char_tail == '\0' ){

                        conn_buff[idx] = atoi(char_head); 
                        break;
                }

                // iteration
              	char_tail++;
        }

	// PUB_IP
	idx = 4;
	char_head = argv[4];
        char_tail = argv[4];
        while(1){

                if( *char_tail == '.' ){

			conn_buff[idx++] = atoi(char_head);
                        char_head = char_tail+1;
                }

                if( *char_tail == '\0' ){

                        conn_buff[idx] = atoi(char_head); 
                        break;
                }

		// iteration
		char_tail++;
	}

        // Conn
        int conn_fd = TCP_client_sock( argv[1], atoi(argv[2]) );
        write( conn_fd, conn_buff, idx+1 );

        // Finalization
	close( conn_fd );
	return 0;
}
