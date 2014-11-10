#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <errno.h>
#include <fcntl.h>

int ds_vm_state_new( int vm_state[4][8] );
int ds_vm_state_get( int vm_state[4][8] );
int ds_vm_state_set( int vm_state[4][8] );

char *ds_vm_state_to_string( int vm_state[4][8], char temp_buff[] );
