#include "CMND_data_op.h"

int GET_VM_ID_AVAILABLE( const char *PM_NAME, const char *VM_NAME, unsigned int *VM_ROW ){

	// Load PM TABLE
	int PM_DATA[PM_COL];
	int temp = LOAD_PM_DATA( PM_NAME, PM_DATA );
	if( temp < 0 ) return -1;

	// Load VM TABLE
	int VM_DATA_size = sizeof(unsigned int) * PM_DATA[0] * VM_COL;
	unsigned int *VM_DATA = (unsigned int *) malloc( VM_DATA_size );
	temp = LOAD_VM_DATA( VM_NAME, VM_DATA, PM_DATA[0] );
	if( temp < 0 ){
		free( VM_DATA );
		return -1;
	}

	// Search and Copy
	int VM_ID = -1;;
	for( int idx = 0 ; idx < PM_DATA[0] ; idx++ ){

		if( VM_DATA[ idx*VM_COL+1 ] == VM_READY ){

			for( int jdx = 0 ; jdx < VM_COL ; jdx++ )
				VM_ROW[jdx] = VM_DATA[ idx*VM_COL + jdx ];

			VM_ID = idx;
			break;
		}
	}

        // End
        free( VM_DATA );
        return VM_ID;
}

int GET_VM_ID_BY_IP(	const char *PM_NAME, const char *VM_NAME, 
			const unsigned int VM_IP, unsigned int *VM_ROW ){

	// Load PM TABLE
	int PM_DATA[PM_COL];
	int temp = LOAD_PM_DATA( PM_NAME, PM_DATA );
	if( temp < 0 ) return -1;

	// Load VM TABLE
	int VM_DATA_size = sizeof(unsigned int) * PM_DATA[0] * VM_COL;
	unsigned int *VM_DATA = (unsigned int *) malloc( VM_DATA_size );
	temp = LOAD_VM_DATA( VM_NAME, VM_DATA, PM_DATA[0] );
	if( temp < 0 ){
		free( VM_DATA );
		return -1;
	}

	// Search and Copy
	int VM_ID = -1;
	for( int idx = 0 ; idx < PM_DATA[0] ; idx++ ){

		if( VM_DATA[ idx*VM_COL + 6 ] == VM_IP ){

			for( int jdx = 0 ; jdx < VM_COL ; jdx++ )
				VM_ROW[jdx] = VM_DATA[ idx*VM_COL + jdx ];

			VM_ID = idx;
			break;
		}
	}

        // End
        free( VM_DATA );
        return VM_ID;
}

// ============================================================

int GET_VM_ID( const char *PM_NAME, const char *VM_NAME, const int VM_ID, unsigned int *VM_ROW ){

	// Load PM TABLE
	int PM_DATA[PM_COL];
	int temp = LOAD_PM_DATA( PM_NAME, PM_DATA );
	if( temp == -1 ) return -1;

	// Load VM TABLE
	int VM_DATA_size = sizeof(unsigned int) * PM_DATA[0] * VM_COL;
	unsigned int *VM_DATA = (unsigned int *) malloc( VM_DATA_size );
	temp = LOAD_VM_DATA( VM_NAME, VM_DATA, PM_DATA[0] );
	if( temp == -1 ){
		
		free( VM_DATA );
		return -1;
	}

	// Copy Data
	for( int idx = 0 ; idx < VM_COL ; idx++ ) VM_ROW[idx] = VM_DATA[ VM_ID*VM_COL + idx ];

	// End
	free( VM_DATA );
	return VM_ID;	
}

int SET_VM_ID( const char *PM_NAME, const char *VM_NAME, const int VM_ID, unsigned int *VM_ROW ){

	// Load PM TABLE
        int PM_DATA[PM_COL];
        int temp = LOAD_PM_DATA( PM_NAME, PM_DATA );
        if( temp < 0 ) return -1;

        // Load VM TABLE
        int VM_DATA_size = sizeof(unsigned int) * PM_DATA[0] * VM_COL;
	unsigned int *VM_DATA = (unsigned int *) malloc( VM_DATA_size );
	temp = LOAD_VM_DATA( VM_NAME, VM_DATA, PM_DATA[0] );
        if( temp < 0 ){
	
		free( VM_DATA );
		return -1;
	}

	// Updata PM_DATA
	int PM_DATA_BAK[PM_COL];
	for( int idx = 0 ; idx < PM_COL ; idx ++ ) PM_DATA_BAK[idx] = PM_DATA[idx];

	// OP Failed
	if( VM_DATA[ VM_ID*VM_COL + 1 ] == VM_RUNNING|VM_DIRTY && VM_ROW[1] == VM_READY ){

		PM_DATA[1] = PM_DATA[1] - 1;
		PM_DATA[3] = PM_DATA[3] - VM_DATA[ VM_ID*VM_COL + 3 ];
		PM_DATA[5] = PM_DATA[5] - VM_DATA[ VM_ID*VM_COL + 4 ];

		VM_ROW[3] = 0;
		VM_ROW[4] = 0;
		VM_ROW[5] = 0;
		VM_ROW[6] = 0;
			
	// Ready to Running
	}else if( VM_DATA[ VM_ID*VM_COL + 1 ] == VM_READY && VM_ROW[1] == VM_RUNNING|VM_DIRTY ){

			PM_DATA[1] = PM_DATA[1] + 1;
			PM_DATA[3] = PM_DATA[3] + VM_ROW[3];
			PM_DATA[5] = PM_DATA[5] + VM_ROW[4];

	// Shutdown
	}else if( VM_DATA[ VM_ID*VM_COL + 1 ] - VM_RUNNING ==  VM_ROW[1] ){

			VM_ROW[3] = 0;
			VM_ROW[4] = 0;
			VM_ROW[5] = 0;
			VM_ROW[6] = 0;

			PM_DATA[3] = PM_DATA[3] - VM_DATA[ VM_ID*VM_COL + 3 ];
			PM_DATA[5] = PM_DATA[5] - VM_DATA[ VM_ID*VM_COL + 4 ];

	// Restore
	}else if( VM_DATA[ VM_ID*VM_COL + 1 ] - VM_DIRTY ==  VM_ROW[1] ){

			PM_DATA[1] = PM_DATA[1] - 1;
	}

	temp = SAVE_PM_DATA( PM_NAME, PM_DATA );
	if( temp < 0 ){

		free( VM_DATA );
		return -1;
	}

	// Update VM_DATA
        for( int idx = 0 ; idx < VM_COL ; idx++ ) VM_DATA[ VM_ID*VM_COL + idx ] = VM_ROW[idx];

	temp = SAVE_VM_DATA( VM_NAME, VM_DATA, PM_DATA[0] );
	if( temp == -1 ){

		free( VM_DATA );
		
		temp = SAVE_PM_DATA( PM_NAME, PM_DATA_BAK );
		if( temp < 0 ) return -2;
		
		return -1;
	}

	// End
	free( VM_DATA );
        return VM_ID;
}

// ============================================================

int SAVE_PM_DATA( const char *PM_NAME, const unsigned int *PM_DATA){
	
	// PM FILE
	FILE *PM_FILE = fopen( PM_NAME, "w");
	if( PM_FILE == NULL ) return -1;
	fprintf( PM_FILE, "%u,%u,%u,%u,%u,%u\n", 
			PM_DATA[0], PM_DATA[1], PM_DATA[2],
			PM_DATA[3], PM_DATA[4], PM_DATA[5]);

	// End
	fclose( PM_FILE );
	return PM_DATA[0];
}

int SAVE_VM_DATA( const char *VM_NAME, const unsigned int *VM_DATA, const int size){

	// VM FILE
	FILE *VM_FILE = fopen( VM_NAME, "w");
	if( VM_FILE == NULL ) return -1;
	
	for( int idx = 0 ; idx < size ; idx++ ){

	fprintf( VM_FILE, "%2u,%2u,%2u,%2u,%4u,", 
			VM_DATA[idx*VM_COL+0],
			VM_DATA[idx*VM_COL+1],
			VM_DATA[idx*VM_COL+2],
			VM_DATA[idx*VM_COL+3],
			VM_DATA[idx*VM_COL+4] );

		fprintf( VM_FILE, "%8X,", VM_DATA[idx*VM_COL+5] );
		fprintf( VM_FILE, "%8X\n", VM_DATA[idx*VM_COL+6] );
	}

	// End
	fclose( VM_FILE );
	return size;
}

int SAVE_DATA(  const char *PM_NAME, const unsigned int *PM_DATA,
                const char *VM_NAME, const unsigned int *VM_DATA){

	int temp = SAVE_PM_DATA( PM_NAME, PM_DATA );
	if( temp == -1 ) return -1;

	temp = SAVE_VM_DATA( VM_NAME, VM_DATA, temp );
	if( temp == -1 ) return -1;

	// End
	return temp;
}

// ============================================================

int LOAD_PM_DATA( const char *PM_NAME, unsigned int *PM_DATA ){

	// PM FILE
	FILE *PM_FILE = fopen( PM_NAME, "r");
	if( PM_FILE == NULL ) return -1;

	fscanf( PM_FILE, "%u,%u,%u,%u,%u,%u\n", 
		&PM_DATA[0], &PM_DATA[1], &PM_DATA[2],
		&PM_DATA[3], &PM_DATA[4], &PM_DATA[5]);

	// End
	fclose( PM_FILE );
	return PM_DATA[0];
}

int LOAD_VM_DATA( const char *VM_NAME, unsigned int *VM_DATA, const int size){

	// VM FILE
	FILE *VM_FILE = fopen( VM_NAME, "r");
	if( VM_FILE == NULL ) return -1;

	for( int idx = 0 ; idx < size ; idx++ ){

		fscanf( VM_FILE, "%u,%u,%u,%u,%u,%X,%X\n",
			&VM_DATA[idx*VM_COL+0], &VM_DATA[idx*VM_COL+1], 
			&VM_DATA[idx*VM_COL+2], &VM_DATA[idx*VM_COL+3], 
			&VM_DATA[idx*VM_COL+4], &VM_DATA[idx*VM_COL+5], 
			&VM_DATA[idx*VM_COL+6]);
	}

	// End
	fclose( VM_FILE );
	return size;
}

int LOAD_DATA(  const char *PM_NAME, unsigned int *PM_DATA,
                const char *VM_NAME, unsigned int *VM_DATA){

	int temp = LOAD_PM_DATA( PM_NAME, PM_DATA );
	if( temp == -1 ) return -1;
	
	temp = LOAD_VM_DATA( VM_NAME, VM_DATA, temp );
	if( temp == -1 ) return -1;

	return temp;
}

// ============================================================

int DISP_FILE( const char *PM_NAME, const char *VM_NAME ){

	unsigned int PM_STAT[PM_COL];
	int ret = LOAD_PM_DATA( PM_NAME, PM_STAT );

	unsigned int *VM_STAT = (unsigned int *)malloc(sizeof(unsigned int)*PM_STAT[0]*VM_COL);
	
	ret = LOAD_VM_DATA( VM_NAME, VM_STAT, ret );
	ret = DISP_DATA( PM_STAT, VM_STAT );
	
	free( VM_STAT );
	return ret;
}
		

int DISP_DATA( const unsigned int *PM_DATA, const unsigned int *VM_DATA ){

	printf("============================================================\n");

	// PM DATA
	printf( "[%d,%d][%d,%d][%d,%d]\n", 
			PM_DATA[0], PM_DATA[1], PM_DATA[2],
			PM_DATA[3], PM_DATA[4], PM_DATA[5]);

	printf("------------------------------------------------------------\n");

	// VM DATA
	int idx = 0;
	for( idx =0 ; idx < PM_DATA[0] ; idx++ ){

		printf( "[%2d,%2d,%6d] [%2d,%4d] ", 
			VM_DATA[idx*VM_COL+0],
			VM_DATA[idx*VM_COL+1],
			VM_DATA[idx*VM_COL+2],
			VM_DATA[idx*VM_COL+3],
			VM_DATA[idx*VM_COL+4] );

		printf( "[%03hhu.%03hhu.%03hhu.%03hhu] ",
			VM_DATA[idx*VM_COL+5] << 0 >> 24,
			VM_DATA[idx*VM_COL+5] << 8 >> 24,
			VM_DATA[idx*VM_COL+5] << 16 >> 24,
			VM_DATA[idx*VM_COL+5] << 24 >> 24 );

		printf( "[%03hhu.%03hhu.%03hhu.%03hhu]\n",
			VM_DATA[idx*VM_COL+6] << 0 >> 24,
			VM_DATA[idx*VM_COL+6] << 8 >> 24,
			VM_DATA[idx*VM_COL+6] << 16 >> 24,
			VM_DATA[idx*VM_COL+6] << 24 >> 24 );
	}
	
	printf("============================================================\n");
	return PM_DATA[0];
}
