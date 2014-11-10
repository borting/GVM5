#ifndef CTRL_SERVICE
#define CTRL_SERVICE

#include "CTRL_works.h"

// Foundamental maintance
int HEARTBEAT( PM_ENTRY *PM );

// VM Power control
unsigned int CREATE_VM( unsigned int REQ_CPU, unsigned int REQ_MEM );
unsigned int SHUTDOWN_VM( unsigned int VM_PUB_IP );
unsigned int REBOOT_VM( unsigned int VM_PUB_IP );

// APP Control
int APP_START( unsigned int VM_PUB_IP, unsigned char *CMD );
int APP_CLOSE( unsigned short VM_WWW_PORT );
int APP_CLOSE_VAPPD( unsigned int VM_PUB_IP, unsigned short VM_PUB_PORT );

#endif
