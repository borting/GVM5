#ifndef _COM_PROTOCOL_H_
#define _COM_PROTOCOL_H_

#include "Obj.h"

#define DIR_LEN	    (2048 - sizeof(int))

/****************************************************************************/
/* Open a new application						    */
/****************************************************************************/

typedef struct
{
    DATA_OBJ app;	    // app to be opened
} REQ_OPEN_APP;		    // Request type: 0

typedef CONN_OBJ REP_OPEN_APP;

/****************************************************************************/
/* Open an existing file						    */
/****************************************************************************/

typedef struct
{
    DATA_OBJ app;	    // app to be opened
    DATA_OBJ file;	    // file to be opened
} REQ_OPEN_FILE;	    // Request type: 1

typedef CONN_OBJ REP_OPEN_FILE;

/****************************************************************************/
/* Close an application							    */
/****************************************************************************/

typedef CONN_OBJ REQ_CLOSE_APP;	// Request type: 4

// reply status code only

/****************************************************************************/
/* Create a new file							    */
/****************************************************************************/

typedef struct
{
    DATA_OBJ file;	    // file to be created
} REQ_NEW_FILE;		    // Request type: 8

// reply status code only

/****************************************************************************/
/* Delete a file							    */
/****************************************************************************/

typedef struct
{
    DATA_OBJ file;	    // file to be deleted
} REQ_DEL_FILE;		    // Request type: 9

// reply status code only

/****************************************************************************/
/* Create a new folder							    */
/****************************************************************************/

typedef struct
{
    DATA_OBJ dir;	    // folder to be created
} REQ_NEW_DIR;		    // Request type: 10

// reply status code only

/****************************************************************************/
/* Delete a folder							    */
/****************************************************************************/

typedef struct
{
    DATA_OBJ dir;	    // folder to be deleted
} REQ_DEL_DIR;		    // Request type: 11

// reply status code only

/****************************************************************************/
/* Change/list directory						    */
/****************************************************************************/

typedef struct
{
    char dir[DIR_LEN];	    // dir name, specify "." means list current dir
} REQ_CH_DIR;		    // Request type: 16

typedef DIR_OBJ REP_CH_DIR;

/****************************************************************************/
/* Union of REQUEST command						    */
/****************************************************************************/

typedef struct
{
    unsigned int type;	    // Request type
    union {
	REQ_OPEN_APP	open_app;   // type: 0
	REQ_OPEN_FILE	open_file;  // type: 1
	REQ_CLOSE_APP	close_app;  // type: 4
	REQ_NEW_FILE	new_file;   // type: 8
	REQ_DEL_FILE	del_file;   // type: 9
	REQ_NEW_DIR	new_dir;    // type: 10
	REQ_DEL_DIR	del_dir;    // type: 11
	REQ_CH_DIR	ch_dir;	    // type: 16
    } cmd;
} VAPP_REQ;

/****************************************************************************/
/* Union of REPLY message						    */
/****************************************************************************/

typedef struct
{
    unsigned int type;	    // Request type
    int retval;		    // Return value
    union {
	REP_OPEN_APP	open_app;   // type: 0
	REP_OPEN_FILE	open_file;  // type: 1
	//REP_CH_DIR	ch_dir;	    // type: 16
    } msg;
} VAPP_REP;

#endif	/* _COM_PROTOCOL_H_ */
