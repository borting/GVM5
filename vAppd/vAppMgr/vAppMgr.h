#ifndef _VAPPMGR_H_
#define _VAPPMGR_H_

/* initialization */
void init();

/* fork vAppd */
void fork_vAppd();

/* process routine of request handler */
void *reqHandler(void *fd);

/* deconstruction */
void cleanup();

#endif	/* _VAPPMGR_H_ */
