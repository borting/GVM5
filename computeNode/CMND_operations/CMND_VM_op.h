#ifndef CMND_VM_OP
#define CMND_VM_OP

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

#include "CMND_op.h"

#define VM_PATH		"/home/rtes/GVM5/CMND_VM"
#define XM_CREATE_SH	"/home/rtes/GVM5/CMND_VM/xm_interface/xm_create.sh"
#define XM_REBOOT_SH	"/home/rtes/GVM5/CMND_VM/xm_interface/xm_reboot.sh"
#define XM_SHUTDOWN_SH	"/home/rtes/GVM5/CMND_VM/xm_interface/xm_shutdown.sh"

#define XM_RENAME_SH    "/home/rtes/GVM5/CMND_VM/xm_interface/xm_rename.sh"
#define XM_RESTORE_SH	"/home/rtes/GVM5/CMND_VM/xm_interface/xm_rename.sh"

int CMND_VM_CREATE( int VM_ID, int VM_CPU, int VM_MEM );
int CMND_VM_REBOOT( int VM_ID, int VM_CPU, int VM_MEM );
int CMND_VM_SHUTDOWN( int VM_ID );

int CMND_VM_RENAME( int VM_ID );
int CMND_VM_RESTORE( int VM_ID );

#endif
