#ifndef _OBJ_H_
#define _OBJ_H_

#define NAME_LEN    256

/*
 * Struct saving the name of an object (app/file/folder)
 */
typedef struct
{
    unsigned int len;	    // length of name (MAX is 255)
    char name[NAME_LEN];    // name of app/file/folder
} DATA_OBJ;

/*
 * Struct saving app's port
 */
typedef struct
{
    unsigned char ip[4];// local ip address
    unsigned short pvt_port;// new app's port number
} CONN_OBJ;

/*
 * Struct saving dir's information
 *
 * After sending DIR_OBJ,
 * file list are trasmitted in the following format (sepatated by '&')
 *    [d_type_1][DATA_OBJ_1]&[d_type_2][DATA_OBJ_2]&...&[d_type_N][DATA_OBJ_N]
 */
typedef struct
{
    unsigned int file_num;  // number of files and sub-folders in this folder
} DIR_OBJ;

/* read DATA_OBJ from fd */
int readDataObj(int fd, DATA_OBJ *dataObj);

/* read CONN_OBJ from fd */
int readConnObj(int fd, CONN_OBJ *connObj);

#endif	/* _OBJ_H_ */
