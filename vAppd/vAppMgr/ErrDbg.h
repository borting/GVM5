#ifndef _ERRDBG_H_
#define _ERRDBG_H_

#include <stdio.h>

/* print error then exit */
#define stderrexit(errcode,format,arg...) \
    do { \
	fprintf(stderr, format "\n",##arg); \
	exit(errcode); \
    } while (0)

/* print to stderr */
//#define printErr(fmt, ...) fprintf(stderr, fmt "\n", __VA_ARGS__);
#define printErr(format,arg...) \
    do { \
	fprintf(stderr, format "\n",##arg); \
    } while (0)

/* print for debug */
#ifdef DEBUG
#define DEBUG_FLAG 1
#else
#define DEBUG_FLAG 0
#endif

#define printDebug(fmt, ...) \
    do { \
	if (DEBUG_FLAG) fprintf(stderr, "%s:%d:%s(): " \
		fmt, __FILE__, __LINE__, __func__, __VA_ARGS__); \
    } while(0)

#endif	/* _ERRDBG_H_ */
