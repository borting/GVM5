#ifndef _PROC_REQUEST_H_
#define _PROC_REQUEST_H_

/* init variables for procRoutine() */
void initProcRoutine();

/* the process routine of child process */
int procRoutine(int connfd, char *mgrPortStr);

#endif	/* _PROC_REQUEST_H_ */
