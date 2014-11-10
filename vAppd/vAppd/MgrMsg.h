#ifndef _MGRMSG_H_H
#define _MGRMSG_H_H

#include <unistd.h>

typedef struct {
    int type;
    union {
        pid_t pid;
	unsigned short pvt_port;
    } req;
} MgrMsg;

#endif	/* _MGRMSG_H_H */
