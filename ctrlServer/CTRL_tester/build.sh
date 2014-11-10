#! /bin/bash
#gcc	CTRL_data_test.c  \
#	../CTRL_operations/CTRL_data_op.c	\
#	-I ../CTRL_operations/			\
#       -std=gnu99				\
 #      -o Tester_data_op			\
#	`mysql_config --cflags --libs`

gcc	CTRL_work_test.c  \
	../CTRL_operations/CTRL_data_op.c	\
	../CTRL_works/CTRL_works.c		\
	../DS_lib/ds_tcp_sock.c                 \
	-I ../DS_lib/                           \
	-I ../CTRL_works/			\
	-I ../CTRL_operations/			\
        -std=gnu99				\
        -o Tester_works				\
	`mysql_config --cflags --libs`
