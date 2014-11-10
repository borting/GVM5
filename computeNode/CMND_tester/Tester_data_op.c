#include <stdio.h>
#include <stdlib.h>
#include "CMND_data_op.h"

int main(){

	int PM_STAT[PM_COL];
	PM_STAT[0] = 3;		PM_STAT[1] = 0;
	PM_STAT[2] = 16;	PM_STAT[3] = 0;
	PM_STAT[4] = 1024;	PM_STAT[5] = 0;

	// Initial ============================================
	int idx = 0;
	int *VM_STAT = (int *)malloc(sizeof(int)*PM_STAT[0]*VM_COL);
        for( idx = 0 ; idx < PM_STAT[0] ; idx++ ){

                VM_STAT[idx*VM_COL] = idx;
                VM_STAT[idx*VM_COL+2] = 30010 + idx;
        }

	VM_STAT[ 0*VM_COL + 5 ] = 3232235893;
	VM_STAT[ 1*VM_COL + 5 ] = 3232235894;
	VM_STAT[ 2*VM_COL + 5 ] = 3232235895;
	
	DISP_DATA( PM_STAT, VM_STAT );

/*	// Modify data
	for( idx = 0 ; idx < PM_STAT[0]*VM_COL ; idx++) VM_STAT[idx]++;
	DISP_DATA( PM_STAT, VM_STAT );
*/
	// Save data
	SAVE_DATA( "PM.temp", PM_STAT, "VM.temp", VM_STAT );
	
	// Free
	free( VM_STAT );

	// Reallocate =========================================
	VM_STAT = (int *)malloc(sizeof(int)*PM_STAT[0]*VM_COL);
	LOAD_DATA( "PM.temp", PM_STAT, "VM.temp", VM_STAT );
	
	DISP_DATA( PM_STAT, VM_STAT );
	
	unsigned int VM_temp[VM_COL];
	GET_VM_ID( "PM.temp", "VM.temp", 1, VM_temp );
	for( int idx = 0 ; idx < VM_COL ; idx ++ ) VM_temp[idx] = VM_temp[idx] + 1;

	SET_VM_ID( "PM.temp", "VM.temp", 2, VM_temp );
	LOAD_DATA( "PM.temp", PM_STAT, "VM.temp", VM_STAT );
	DISP_DATA( PM_STAT, VM_STAT );

	// Free
	free( VM_STAT );

	// GET SET ============================================

	int VM_DATA[VM_COL];
	for( int idx = 0 ; idx < PM_STAT[0] ; idx++ ){

		GET_VM_ID( "PM.temp", "VM.temp", idx, VM_DATA );
		VM_DATA[1] = 3;
		VM_DATA[6] = idx;
		SET_VM_ID( "PM.temp", "VM.temp", idx, VM_DATA );
	}

	VM_STAT = (int *)malloc(sizeof(int)*PM_STAT[0]*VM_COL);
        LOAD_DATA( "PM.temp", PM_STAT, "VM.temp", VM_STAT );
	DISP_DATA( PM_STAT, VM_STAT );
	free( VM_STAT );

	return 0;
}
