gcc 	CTRL_daemon.c 				\
	../CTRL_operations/CTRL_data_op.c	\
	../DS_lib/ds_tcp_sock.c			\
	-I ../DS_lib				\
	-I ../CTRL_operations			\
	`mysql_config --cflags --libs`		\
	-std=gnu99				\
	-o CTRL_daemon

gcc	CTRL_servant.c				\
	../CTRL_operations/CTRL_data_op.c	\
	../CTRL_works/CTRL_works.c		\
        ../CTRL_services/CTRL_services.c	\
	../DS_lib/ds_tcp_sock.c			\
	-I ../CTRL_operations			\
	-I ../CTRL_works			\
	-I ../CTRL_services			\
	-I ../DS_lib				\
        `mysql_config --cflags --libs`		\
        -std=gnu99				\
        -o CTRL_servant

#gcc	CTRL_shadow.c			\
#	CTRL_headers/CTRL_data.c	\
#        CTRL_headers/CTRL_service.c	\
#	ds_lib/ds_tcp_sock.c		\
#        `mysql_config --cflags --libs`	\
#        -std=gnu99			\
#	-o CTRL_shadow

#gcc	CTRL_tracker.c			\
#	CTRL_headers/CTRL_data.c	\
#        CTRL_headers/CTRL_service.c	\
#	ds_lib/ds_tcp_sock.c		\
#        `mysql_config --cflags --libs`	\
#        -std=gnu99			\
#        -o CTRL_tracker
