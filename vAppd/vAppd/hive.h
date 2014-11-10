#ifndef _HIVE_H_
#define _HIVE_H_

#include <unistd.h>

/* parse raw command */
int searchGene(char *rawCMD, char **cmd, char **args, char **envs);

/* exec App */
int spawnApp(char *cmd, char **args, char **envs, unsigned short pvt_port);

/* check execution, report vAppMgr, reply ctrl server */
int babySitter(pid_t ppid, char *cmd, unsigned short pvt_port);

#endif	/* _HIVE_H_ */
