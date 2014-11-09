#ifndef _IPTABLES_CTRL_H_
#define _IPTABLES_CTRL_H_

//// init iptables
//int initIPtables(char *filepath);
//
//// clean iptables
//int cleanIPtables();
//
//// save iptables
//int saveIPtables(char *filepath);
//
//// restore iptables
//int restoreIPtables(char *filepath);

/**
 * @brief add new DNAT rule to iptables
 *
 * @param iface network interface
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval 2 bad IP/port be specified
 * @retval 0 add success
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int addDNATrule(char *iface, unsigned short pub_port, 
	char *pvt_ip, unsigned short pvt_port);

/**
 * @brief check whether DNAT rule in iptables
 *
 * @param iface network interface
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval 2 bad IP/port be specified
 * @retval 1 rule not existed
 * @retval 0 rule existed
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int checkDNATrule(char *iface, unsigned short pub_port, 
	char *pvt_ip, unsigned short pvt_port);

/**
 * @brief delete DNAT rule in iptables
 *
 * @param iface network interface
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval 2 bad IP/port be specified
 * @retval 1 no match rule be found
 * @retval 0 delete success
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 */
int delDNATrule(char *iface, unsigned short pub_port, 
	char *pvt_ip, unsigned short pvt_port);

/**
 * @brief find the local port number of a DNAT rule
 *
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval 0 local port found
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 * @retval -3 local port not found
 */
int findPubPort(unsigned short *pub_port, 
	char *pvt_ip, unsigned short pvt_port);
/**
 * @brief find the forward IP and port number of a DNAT rule
 *
 * @param pub_port local port number
 * @param pvt_ip forward destination IP address
 * @param pvt_port forward destination port number
 * @retval 0 forward IP & port found
 * @retval -1 popen failed
 * @retval -2 shell script not found or some error occured
 * @retval -3 forward IP & port not found
 */
int findPvtIPPort(unsigned short pub_port, 
	char *pvt_ip, unsigned short *pvt_port);

/**
 * @brief delete DNAT rules set for a particular IP address
 *
 * @param iface network interface
 * @param pvt_ip forward destination IP address
 * @retval count number of deleted rules
 * @retval -1 popen failed
 */
int delIPinDNAT(char *iface, char *pvt_ip);

#endif /* _IPTABLES_CTRL_H_ */
