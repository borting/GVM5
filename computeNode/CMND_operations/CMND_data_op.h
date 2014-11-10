#ifndef CMND_DATA_OP
#define CMND_DATA_OP

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <CMND_op.h>

#define VM_READY 	0
#define VM_RUNNING 	1
#define VM_DIRTY	2

#define VM_COL 7
#define PM_COL 6

/*	PM_COL
	IMG_LEFT
	IMG_USED
	CPU_LFET
	CPU_USED
	MEM_LEFT
	MEM_USED
*/

/*	VM_COL
	IMG_ID
	IMG_STAT
	IMG_PORT
	VM_CPU
	VM_MEM
	VM_IP_PVT
	VM_IP_PUB
*/

int GET_VM_ID_AVAILABLE( const char *PM_NAME, const char *VM_NAME, unsigned int *VM_ROW );
int GET_VM_ID_BY_IP( 	const char *PM_NAME, const char *VM_NAME, 
			const unsigned int VM_IP, unsigned int *VM_ROW );

// ============================================================

int GET_VM_ID( const char *PM_NAME, const char *VM_NAME, const int VM_ID, unsigned int *VM_ROW );
int SET_VM_ID( const char *PM_NAME, const char *VM_NAME, const int VM_ID, unsigned int *VM_ROW );

// ============================================================

int SAVE_PM_DATA( const char *PM_NAME, const unsigned int *PM_DATA ); 
int SAVE_VM_DATA( const char *VM_NAME, const unsigned int *VM_DATA, const int size);

int SAVE_DATA(	const char *PM_NAME, const unsigned int *PM_DATA, 
		const char *VM_NAME, const unsigned int *VM_DATA);

// ============================================================

int LOAD_PM_DATA( const char *PM_NAME, unsigned int *PM_DATA);
int LOAD_VM_DATA( const char *VM_NAME, unsigned int *VM_DATA, const int size);

int LOAD_DATA( 	const char *PM_NAME, unsigned int *PM_DATA,
		const char *VM_NAME, unsigned int *VM_DATA);

// ============================================================

int DISP_DATA( const unsigned int *PM_DATA, const unsigned int *VM_DATA );	
int DISP_FILE( const char *PM_NAME, const char *VM_NAME );	

#endif
