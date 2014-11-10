#include "ds_tcp_sock.h"

int TCP_server_sock( const int port, const int qlen ){

	// Initialization
	struct sockaddr_in sin;
	memset( &sin, 0, sizeof(sin));
	sin.sin_family 		= AF_INET;
	sin.sin_port		= htons((unsigned short) port);
	sin.sin_addr.s_addr	= INADDR_ANY;	// Any client can do
	
	// Get a socket
	int s = socket( PF_INET, SOCK_STREAM, 0 );
	if( s < 0 ){
		exit( printf("[TCP]  Can't get a socket: %s\n",
			strerror(errno)));
	}
	
	if( bind( s, ( struct sockaddr *)&sin, sizeof(struct sockaddr_in)) <0 ){
		exit( printf("[TCP]  Can't bind to port %d: %s\n",
			port, strerror(errno)));
	}
	
	if( listen(s, qlen) < 0 ){
		exit( printf("[TCP]  Can't listen on port %d: %s\n",
			port, strerror(errno)));	
	}
	
	return s;
}

int TCP_client_sock( const char *host, const int port ){

	// Initialization
	struct sockaddr_in sin;
	memset( &sin, 0, sizeof(sin));
	sin.sin_family 		= AF_INET;
	sin.sin_port		= htons((unsigned short) port);
	sin.sin_addr.s_addr	= inet_addr( host );
	
	// Check Host name
	if( sin.sin_addr.s_addr == INADDR_NONE){
		exit( printf("[TCP]  Can't get host id %s: %s\n",
			host, strerror(errno)));	
	}
	
	// Get a socket
	int s = socket( PF_INET, SOCK_STREAM, 0 );
	if( s < 0 ){
		exit( printf("[TCP]  Can't get a socket: %s\n",
			strerror(errno)));
	}
	
	// Connect to server
	if( connect( s, ( struct sockaddr *)&sin, sizeof(sin))){
		exit( printf("[TCP]  Can't connect to %s: %s\n",
			host, strerror(errno)));
	}
	
	return s;
}

int TCP_client_sender( 	const char *host, const int port, 
			const int snd_buff_len, const char *snd_buff, 
			const int rcv_buff_len, char *rcv_buff){

	int conn_res;
	int conn_fd;

	// Create a connection
	conn_fd = TCP_client_sock( host, port );
	if( conn_fd < 0 ) return -1;

	// Send data
	conn_res = write( conn_fd, snd_buff, snd_buff_len );
	if( conn_res != snd_buff_len ){

		close( conn_fd );
		return -2;
	}

	// Read data
	conn_res = read( conn_fd, rcv_buff, rcv_buff_len );

	close( conn_fd );
	return conn_res;
}

/*
int TCP_client_sender( 	const unsigned int host, const int port, 
			const int snd_buff_len, const char *snd_buff, 
			const int rcv_buff_len, const char *rcv_buff ){

	unsigned char *IP_ptr = &host;
	unsigned char IP_temp[16]; memset( IP_temp, 0, 16 );
	sprintf(IP_temp, "%hhu.%hhu.%hhu.%hhu", *(IP_ptr+3), *(IP_ptr+2), *(IP_ptr+1), *IP_ptr);
	return TCP_client_sender(IP_temp, port, snd_buff_len, snd_buff, rcv_buff_len, rcv_buff);
}
*/
