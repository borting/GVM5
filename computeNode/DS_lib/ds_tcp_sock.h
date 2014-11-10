#ifndef _DS_TCP_SOCKET_
#define _DS_TCP_SOCKET_

#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

// TCP server socket
int TCP_server_sock( const int port, int qlen );

// TCP client socket
int TCP_client_sock( const char *host, const int port );
/*
int TCP_client_sender(  const unsigned int host, const int port,
                        const int snd_buff_len, const char *snd_buff,
                        const int rcv_buff_len, const char *rcv_buff );
*/
int TCP_client_sender(  const char *host, const int port,
                        const int snd_buff_len, const char *snd_buff,
                        const int rcv_buff_len, char *rcv_buff);

#endif // _DS_TCP_SOCKET_
