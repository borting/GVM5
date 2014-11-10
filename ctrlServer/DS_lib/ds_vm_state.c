#include "ds_vm_state.h"

int ds_vm_state_new( int vm_state[4][8] ){

	FILE	*temp_file;
	int 	temp_ret;

	temp_file = fopen("temp.txt","w");
	if( temp_file == NULL )	return -1;

	int idx;	
	for( idx = 0 ; idx < 4 ; idx ++ ){

		temp_ret = fprintf( temp_file, "%d %d %d %d %d %d %d %d\n",
					vm_state[idx][0],vm_state[idx][1],
					vm_state[idx][2],vm_state[idx][3],
					vm_state[idx][4],vm_state[idx][5],
					vm_state[idx][6],vm_state[idx][7]);

		if( temp_ret < 0 )	return temp_ret;		
	}
	fclose(temp_file);
	return 0;
}

int ds_vm_state_get( int vm_state[4][8] ){

	FILE	*temp_file;
	int 	temp_ret;

	temp_file = fopen("temp.txt","r");
	if( temp_file == NULL )	return -1;
	
	int idx = 0 ;
	for( idx = 0 ; idx < 4 ; idx ++ ){

		temp_ret = fscanf( temp_file, "%d %d %d %d %d %d %d %d",
					&vm_state[idx][0], &vm_state[idx][1],
					&vm_state[idx][2], &vm_state[idx][3],
					&vm_state[idx][4], &vm_state[idx][5],
					&vm_state[idx][6], &vm_state[idx][7]);

		if( temp_ret < 0 )	return temp_ret;		
	}
	fclose(temp_file);
	return 0;
}

int ds_vm_state_set( int vm_state[4][8] ){

	FILE	*temp_file;
	int 	temp_ret;

	temp_file = fopen("temp.txt","w");
	if( temp_file == NULL )	return -1;
	
	int idx;
	for( idx = 0 ; idx < 4 ; idx ++ ){

		temp_ret = fprintf( temp_file, "%d %d %d %d %d %d %d %d\n",
					vm_state[idx][0], vm_state[idx][1],
					vm_state[idx][2], vm_state[idx][3],
					vm_state[idx][4], vm_state[idx][5],
					vm_state[idx][6], vm_state[idx][7]);

		if( temp_ret < 0 )	return temp_ret;		
	}
	fclose(temp_file);
	return 0;
}

char *ds_vm_state_to_string( int vm_state[4][8], char input_buff[] ){

	int	temp_ret;
	char	temp_buff[4][1024];

	int idx;
	for( idx = 0 ; idx < 4 ; idx ++ ){

		temp_ret = sprintf( temp_buff[idx], "%d %d %d %d %d %d %d %d",
					vm_state[idx][0], vm_state[idx][1],
					vm_state[idx][2], vm_state[idx][3],
					vm_state[idx][4], vm_state[idx][5],
					vm_state[idx][6], vm_state[idx][7]);

		if( temp_ret < 0 ) return NULL;		
	}

	temp_ret = sprintf( input_buff, "%s\n%s\n%s\n%s\n", 
					temp_buff[0], temp_buff[1],
					temp_buff[2], temp_buff[3]);
	if( temp_ret < 0 ) return 0;

	return input_buff;
}
