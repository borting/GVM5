#include "CMND_VM_op.h"

int CMND_VM_CREATE( int VM_ID, int VM_CPU, int VM_MEM ){

	// Pipe initialization 
	int pipe_fd[2];
	if( pipe( pipe_fd ) < 0 ){

		return ERR_PIPE; // Pipe error
	}

	// Fork
	int pid_worker = fork();	
	if(  pid_worker < 0){
		
		return ERR_FORK; // Fork error

	// Parent process
	}else if( pid_worker > 0 ){

		// Pipe setting
		close( pipe_fd[1] );
		
		// Wait for worker finishing his job
		int stat_worker;
		waitpid( pid_worker, &stat_worker, 0 );

		// Parsing string from pipe
		char pipe_buff[256];

		int idx = 0;
		int res = -1;
		memset( pipe_buff, 0, 256 );
		while( read( pipe_fd[0], pipe_buff+idx, 1 )){

			if( *(pipe_buff+idx++) == '\n' ){
				
				res = strncmp( pipe_buff, "Started domain", 13 );
				if( res == 0 )	break;
				else idx = 0;					
			}
		}
		
		if( res == 0 )	return VM_ID;
		else{
			
			if( CMND_VM_RENAME( VM_ID ) < 0 ) return ERR_FATAL;
			return ERR_VM;
		}

	// Worker process
	}else{
		// Pipe setting
		close( pipe_fd[0] );	// close unused reading pipe
		close( 1 );		// close stdout
		dup( pipe_fd[1] );	// replace stdin with writing pipe
		close( pipe_fd[1] );	// close old writing pipe

		char ARG_IMG_TEMP[16]; sprintf( ARG_IMG_TEMP, "%d", VM_ID );
		char ARG_CPU_TEMP[16]; sprintf( ARG_CPU_TEMP, "%d", VM_CPU );
		char ARG_MEM_TEMP[16]; sprintf( ARG_MEM_TEMP, "%d", VM_MEM );
		execl(  XM_CREATE_SH, "xm_create.sh", VM_PATH,
			ARG_IMG_TEMP, ARG_CPU_TEMP, ARG_MEM_TEMP, (char *)0);

		exit(0);
	}
}

int CMND_VM_RENAME( int VM_ID ){

	// Fork
	int pid_worker = fork();	
	if(  pid_worker < 0){
		
		return ERR_FORK; // Fork error
	}

	// Parent process
	else if( pid_worker > 0 ){

		// Wait for worker finishing his job
		int stat_worker;
		waitpid( pid_worker, &stat_worker, 0 );

		return VM_ID;
	}

	// Worker process
	else{
		// FD setting
		close(1);
		
		char ARG_IMG_TEMP[16]; sprintf( ARG_IMG_TEMP, "%d", VM_ID );
		execl(	XM_RENAME_SH, "xm_rename.sh", VM_PATH,
			ARG_IMG_TEMP, (char *)0);

		exit(0);
	}
}

int CMND_VM_REBOOT( int VM_ID, int VM_CPU, int VM_MEM ){

	// Pipe initialization 
	int pipe_fd[2];
	if( pipe( pipe_fd ) < 0 ){

		return ERR_PIPE; // Pipe error
	}

	// Fork
	int pid_worker = fork();	
	if(  pid_worker < 0){
		
		return ERR_FORK; // Fork error
	
	// Parent process
	}else if( pid_worker > 0 ){

		// Pipe setting
		close( pipe_fd[1] );
		
		// Wait for worker finishing his job
		int stat_worker;
		waitpid( pid_worker, &stat_worker, 0 );

		// Parsing string from pipe
		char pipe_buff[256];
	
		int idx = 0;
		int res = -1;
		memset( pipe_buff, 0, 256 );
		while( read( pipe_fd[0], pipe_buff+idx, 1 )){

			if( *(pipe_buff+idx++) == '\n' ){
				
				res = strncmp( pipe_buff, "Started domain", 13 );
				if( res == 0 )	break;
				else idx = 0;					
			}
		}
		
		if( res == 0 )	return VM_ID;
		else		return ERR_VM;

	// Worker process
	}else{
		// Pipe setting
		close( pipe_fd[0] );	// close unused reading pipe
		close( 1 );		// close stdout
		dup( pipe_fd[1] );	// replace stdin with writing pipe
		close( pipe_fd[1] );	// close old writing pipe

		char ARG_IMG_TEMP[16]; sprintf( ARG_IMG_TEMP, "%d", VM_ID );
		char ARG_CPU_TEMP[16]; sprintf( ARG_CPU_TEMP, "%d", VM_CPU );
		char ARG_MEM_TEMP[16]; sprintf( ARG_MEM_TEMP, "%d", VM_MEM );

		execl(	XM_REBOOT_SH, "xm_reboot.sh", VM_PATH,
			ARG_IMG_TEMP, ARG_CPU_TEMP, ARG_MEM_TEMP, (char *)0);

		exit(0);
	}
}

int CMND_VM_RESTORE( int VM_ID ){

	// Fork
	int pid_worker = fork();	
	if(  pid_worker < 0){
		
		return ERR_FORK; // Fork error
	}

	// Parent process
	else if( pid_worker > 0 ){

		// Wait for worker finishing his job
		int stat_worker;
		waitpid( pid_worker, &stat_worker, 0 );

		return VM_ID;
	}

	// Worker process
	else{
		// FD setting
		close(1);
		
		char ARG_IMG_TEMP[16]; sprintf( ARG_IMG_TEMP, "%d", VM_ID );
		execl(	XM_RESTORE_SH, "xm_restore.sh", VM_PATH,
			ARG_IMG_TEMP, (char *)0);
		
		exit(0);
	}
}

int CMND_VM_SHUTDOWN( int VM_ID ){
	
	// Fork
	int pid_worker = fork();	
	if(  pid_worker < 0){
		
		return ERR_FORK; // Fork error
	}

	// Parent process
	else if( pid_worker > 0 ){

		// Wait for worker finishing his job
		int stat_worker;
		waitpid( pid_worker, &stat_worker, 0 );

		return VM_ID;
	}

	// Worker process
	else{
		// FD setting
		close(1);
		
		char ARG_IMG_TEMP[16]; sprintf( ARG_IMG_TEMP, "%d", VM_ID );
		execl(	XM_SHUTDOWN_SH, "xm_shutdown.sh", VM_PATH,
			ARG_IMG_TEMP, (char *)0);

		exit(0);
	}
}
