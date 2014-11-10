#ifndef BUFF_SIZE
#define BUFF_SIZE 256
#endif

#ifndef CMND_SERVICE
#define CMND_SERVICE

#include "ds_tcp_sock.h"
#include "ds_sem.h"

#include "CMND_data_op.h"
#include "CMND_VM_op.h"

int CMND_REQ_HEARTBEAT(	const long int sem_key,
			const char *PM_NAME,
			const char *VM_NAME,
					
			char *str_cmd,
			char *str_res);

int CMND_REQ_START(	const long int sem_key,
			const char *PM_NAME,
			const char *VM_NAME,
					
			char *str_cmd,
			char *str_res);

int CMND_REQ_RESTORE( 	const long int sem_key,
			const char *PM_NAME,
			const char *VM_NAME,
						
			char *str_cmd,
			char *str_res);

int CMND_REQ_SHUTDOWN(	const long int sem_key,
			const char *PM_NAME,
			const char *vm_stat,
			
			char *str_cmd,
			char *str_res);

int CMND_REQ_REBOOT(	const long int sem_key,
			const char *PM_NAME,
			const char *vm_stat,
			
			char *str_cmd,
			char *str_res);
#endif
