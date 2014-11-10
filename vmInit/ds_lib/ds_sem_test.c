#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>

#include <sys/ipc.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>

#include <errno.h>
#include "ds_sem.h"

int sem;

void increase(){

	int	temp_ret;
	int	temp_fd;
	int	temp_cnt;
	char	temp_buff[1024];
	
		// Test
		printf( "[%d] \n", getpid() );
	int idx = 0;
	for( idx = 0 ; idx < 10000 ; idx++ ){


#ifdef WITH_SEM

		int sem_id = ds_sem_get( 19891006 );
		
		// Sem lock
		if( ds_sem_lock( sem_id ) < 0 ){

			printf("[INCR] lock error.\n");
			exit(-1);
		}
#endif
		// Open file
		temp_fd = open( "./temp.txt" , O_RDWR );
		if( temp_fd < 0 ){
			printf("[INCR] Open error\n");
			exit(-1);
		}

		temp_ret = read( temp_fd, temp_buff, 1024 );
		if( temp_ret < 0 ){
			printf("[INCR] Open error\n");
			exit(-1);
		}

		temp_cnt = atoi( temp_buff );
		temp_cnt++;

		lseek( temp_fd, 0, SEEK_SET );
		memset( temp_buff, 0, 1024 );
		sprintf( temp_buff, "%d", temp_cnt );

		temp_ret = write( temp_fd, temp_buff, strlen(temp_buff ));
		if( temp_buff < 0 ){
			printf("[INCR] Open error\n");
		}

		// close file
		close(temp_fd);
#ifdef WITH_SEM
		// Sem unlock
		if( ds_sem_unlock( sem_id ) < 0 ){

			printf("[INCR] unlock error.\n");
			exit(-1);
		}
#endif
	}
}

int main(){

	int child_pid;
	int status;
		
	int sem_id;

#ifdef WITH_SEM
	sem_id = ds_sem_new( 19891006, 1 );
	if( sem_id  < 0 )	return -1;
	else			printf("[Test] sem created\n");
#endif
	printf("%d\n", ds_sem_get(19891006));
	printf("%d\n", sem_id);

	if( (child_pid = fork()) < 0 )	return -1;
	
	// Child process
	else if( child_pid == 0 ){

		increase();
		return 0;
	}

	// Parent process
	else{

		increase();
		waitpid( child_pid, &status, 0 );
	}

#ifdef WITH_SEM
	if( ds_sem_rmv( sem_id ) < 0 )	return -1;
	else				printf("[Test] sem removed\n");
#endif

	return 0;
}
