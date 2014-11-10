#ifndef _ERRDBG_H_
#define _ERRDBG_H_

#include <stdio.h>

/***************************************************************************/
/* print error then exit */
/***************************************************************************/
#define stderrexit(errcode,format,arg...) \
    do { \
	fprintf(stderr, format "\n",##arg); \
	exit(errcode); \
    } while (0)

/***************************************************************************/
/* print to stderr */
/***************************************************************************/
#define printErr(format,arg...) \
    do { \
	fprintf(stderr, format "\n",##arg); \
    } while (0)

/***************************************************************************/
/* print for debug */
/***************************************************************************/
#ifdef DEBUG
#define DEBUG_FLAG 1
#else
#define DEBUG_FLAG 0
#endif

#define printDebug(fmt,arg...) \
    do { \
	if (DEBUG_FLAG) fprintf(stderr, "(DBG)%s:%d:%s(): " \
		fmt, __FILE__, __LINE__, __func__,##arg); \
    } while(0)

#endif	/* _ERRDBG_H_ */
