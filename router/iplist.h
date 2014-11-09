#ifndef _DNAT_H_
#define _DNAT_H_

/**
 * @brief validate whether input IP is in the same subnet
 *
 * @param ip input ip address
 * @param subnet subnet
 * @param mask network mask
 * @retval 0 success
 * @retval -1 failure
 */
int validateIP(unsigned char *ip, unsigned char *subnet, unsigned char *mask);

/**
 * @brief create a file for saving IP list 
 *
 * @param filename file name
 * @retval 1 file already existed but truncated as new
 * @retval 0 new file create
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int createIPList(char *filename);

/** 
 * @brief delete file saving IP list
 *
 * @param filename file name
 * @retval 1 file not existed
 * @retval 0 file deleted
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int deleteIPList(char *filename);

/** 
 * @brief add a new IP to IPList
 *
 * @param ip ip address
 * @param filename file name
 * @retval 1 ip already in list
 * @retval 0 success
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int addIPtoList(char *ip, char *filename);

/** 
 * @brief check whether an IP in IPList
 *
 * @param ip ip address
 * @param filename file name
 * @retval 1 ip not in list
 * @retval 0 ip in list
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int checkIPinList(char *ip, char *filename);

/** 
 * @brief delete an IP in IPList
 *
 * @param ip ip address
 * @param filename file name
 * @retval 1 ip not in list
 * @retval 0 ip deleted
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int delIPinList(char *ip, char *filename);

#endif /* _DNAT_H_ */
