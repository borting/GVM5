#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ErrDbg.h"
#include "Obj.h"

int readDataObj(int fd, DATA_OBJ *dataObj)
{
    int retval;
    int nameLen, readLen = 0;

    /* read length of name */
    retval = read(fd, &(dataObj->len), sizeof(unsigned int));
    if (retval != sizeof(unsigned int)) {
	printErr("read Request Data from client failed");
	return -1;
    }

    /* length check */
    nameLen = dataObj->len;
    if (nameLen > (NAME_LEN - 1)) {
	printErr("Request Data Length larger than %d", NAME_LEN);
	return -2;
    }

    /* read name */
    do {
        retval = read(fd, (dataObj->name + readLen), (nameLen - readLen));
	if (retval < 1) {
	    printErr("read Request Data from client failed");
	    return -1;
	}
	else {
	    readLen += retval;
	}
    } while (readLen != nameLen);

    /* manually add null('\0') to the 'valid' end of dataObj->name */
    dataObj->name[readLen] = '\0';

    return 0;
}

int readConnObj(int fd, CONN_OBJ *connObj)
{
    int retval;
    int readLen = 0;

    do {
	retval = read(fd, connObj + readLen, sizeof(CONN_OBJ) - readLen);
	if (retval < 1) {
	    printErr("read Connection Info from client failed");
	    return -1;
	}
	else {
	    readLen += retval;
	}
    } while (readLen != sizeof(CONN_OBJ));

    return 0;
}
