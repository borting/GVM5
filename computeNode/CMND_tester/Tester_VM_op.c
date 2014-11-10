#include <stdio.h>
#include "CMND_VM_op.h"

int main(){
	
	int temp_img;
	int temp_cpu;
	int temp_mem;
	int temp_res;

	// Start
	scanf( "%d %d %d", &temp_img, &temp_cpu, &temp_mem );
	temp_res = CMND_VM_CREATE( temp_img, temp_cpu, temp_mem );
	printf( "[%d] Creating VM... %d\n" ,getpid(), temp_res );

	// Reboot
	scanf( "%d %d %d", &temp_img, &temp_cpu, &temp_mem );
	temp_res = CMND_VM_REBOOT( temp_img, temp_cpu, temp_mem );
	printf( "[%d] Rebooting VM... %d\n" ,getpid(), temp_res );

	// Shudown
	scanf( "%d", &temp_img );
	temp_res = CMND_VM_SHUTDOWN( temp_img );
	printf( "[%d] Shutting down VM... %d\n" , getpid(), temp_res );

	// Restore
	scanf( "%d", &temp_img );
	temp_res = CMND_VM_RESTORE( temp_img );
	printf( "[%d] Restore VM... %d\n", getpid(), temp_res );

	return 0;
}
