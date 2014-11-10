#include <stdio.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <errno.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/sem.h>
#include <sys/ipc.h>

// new semaphore
int ds_sem_new( long int sem_key, int sem_value );

// get semaphore
int ds_sem_get( long int sem_key );

// rmv semaphore
int ds_sem_rmv( int sem_id );

// lock
int ds_sem_lock( int sem_id );

// unlock
int ds_sem_unlock( int sem_id );
