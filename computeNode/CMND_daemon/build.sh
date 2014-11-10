gcc	CMND_daemon.c				\
	../DS_lib/ds_tcp_sock.c			\
	../DS_lib/ds_sem.c			\
	../CMND_operations/CMND_data_op.c	\
	-I ../DS_lib/				\
	-I ../CMND_operations/			\
	-std=gnu99 				\
	-o CMND_daemon

gcc	CMND_servant.c				\
	../DS_lib/ds_tcp_sock.c			\
	../DS_lib/ds_sem.c			\
	../CMND_services/CMND_services.c	\
	../CMND_operations/CMND_data_op.c	\
	../CMND_operations/CMND_VM_op.c		\
	-I ../DS_lib/				\
	-I ../CMND_operations/			\
	-I ../CMND_services/			\
	-std=gnu99 				\
	-o CMND_servant

gcc	CMND_heartbeat.c			\
	../DS_lib/ds_tcp_sock.c                 \
	-I ../DS_lib/				\
	-std=gnu99                              \
	-o CMND_heartbeat	
