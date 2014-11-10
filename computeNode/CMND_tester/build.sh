#! /bin/bash

gcc	Tester_VM_op.c	\
	../CMND_operations/CMND_VM_op.c	\
	-I ../CMND_operations/		\
	-std=gnu99			\
	-o Tester_VM_op 

gcc 	Tester_data_op.c 			\
	../CMND_operations/CMND_data_op.c	\
	-I ../CMND_operations/ 			\
	-std=gnu99				\
	-o Tester_data_op

gcc	Tester_service.c			\
	../DS_lib/ds_sem.c			\
	../DS_lib/ds_tcp_sock.c			\
	../CMND_operations/CMND_VM_op.c		\
	../CMND_operations/CMND_data_op.c	\
	../CMND_services/CMND_services.c	\
	-I ../CMND_services			\
	-I ../CMND_operations/			\
	-I ../DS_lib/				\
	-std=gnu99				\
	-o Tester_service
