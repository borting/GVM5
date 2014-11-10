#ifndef _APPLIST_H_
#define _APPLIST_H_

#define ushort unsigned short

/* create app list */
int newAppList(char *filename);

/* delete app list */
int delAppList(char *filename);

/* add new App to list */
int addApp(pid_t pid, char *filename);

/* delete App from list */
int delApp(pid_t pid, char *filename);

/* get the state of App */
int getAppState(pid_t pid, char *filename);

/* set the state of App */
int setAppState(pid_t pid, ushort state, char *filename);

/* get the pvt_port of App */
int getAppPort(pid_t pid, char *filename);

/* set the pvt_port of App */
int setAppPort(pid_t pid, ushort pvt_port, char *filename);

/* get the pid of App by pvt_port */
int getAppPid(ushort pvt_port, char *filename);

/* check App existed */
int searchApp(pid_t pid, char *filename);

#endif	/* _APPLIST_H_ */
