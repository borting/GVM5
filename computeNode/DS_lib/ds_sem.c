#include "ds_sem.h"

// new semaphore
int ds_sem_new( long int sem_key, int sem_value ){

	int sem_id;
	sem_id = semget( sem_key, 1, IPC_CREAT|IPC_EXCL|0666 );
	if( sem_id < 0 ){

		perror("[DS_SEM] create err");
		return(-2);
	}

	if( semctl( sem_id, 0, SETVAL, sem_value ) < 0){

		perror("[DS_SEM] initial err");
		return(-3);
	}

	return semget( sem_key, 1, 0 );
}

// get
int ds_sem_get( long int sem_key ){

	int sem_id;
	sem_id = semget( sem_key, 1,0 );
	if( sem_id < 0 ){

		perror("[DS_SEM] get err");
		return(-1);
	}
	
	return sem_id;
}

// rmv semaphore
int ds_sem_rmv( int sem_id ){

//	int sem_id = ds_sem_get( sem_key );
	if( semctl( sem_id, 0, IPC_RMID, 0 ) < 0 ){

		perror("[DS_SEM] remove err");
		return(-4);
	}
	
	return 0;	
}

// lock
int ds_sem_lock( int sem_id ){

//	int sem_id = ds_sem_get( sem_key );
	struct sembuf sop;
	sop.sem_num 	=  0;
	sop.sem_op	= -1;
	sop.sem_flg	=  0;
	
	if( semop( sem_id, &sop, 1 ) < 0 ){
			
		perror("[DS_SEM] lock err");
		return(-5);
	}

	return 0;
}

// unlock
int ds_sem_unlock( int sem_id ){
	
//	int sem_id = ds_sem_get( sem_key );
	struct sembuf sop;
	sop.sem_num 	=  0;
	sop.sem_op	=  1;
	sop.sem_flg	=  0;
	
	if( semop( sem_id, &sop, 1 ) < 0 ){
			
		perror("[DS_SEM] unlock err");
		return(-6);
	}

	return 0;
}
