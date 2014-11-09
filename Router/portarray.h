#ifndef _PORTARRAY_H_
#define _PORTARRAY_H_

/**
 * @brief structure for bit mapping
 *
 *  bit 1 is available port
 *  bit 0 is used port
 */
typedef struct
{
    int N;  // # of port / 32
    unsigned short portBase;	// port range lower bound
    unsigned short portUpper;	// port range upper bound
    unsigned int *array;
} PortArray;

/**
 * @brief create port array & initialization
 *
 * @param pArray pointer to new array
 * @param size  array size
 * @param portBase start port number of the array
 * @retval -1 locate array failed
 * @retval size number of available port (N * sizeof(int))
 */
int newPortArray(PortArray *pArray, int size, unsigned short portBase);

/**
 * @brief delete port array
 *
 * @param pArray array to be delete
 */
void delPortArray(PortArray *pArray);

/** 
 * @brief search available port & clear bit
 *
 * @param pArray
 * @param port
 * @retval 0 success
 * @retval -1 no available port
 * @retval -2 input port is null
 */
int searchPort(PortArray *pArray, unsigned short *port);

/**
 * @brief reclaim port & set bit
 *
 * @param pArray
 * @param port port number
 * @retval 0 success
 * @retval -1 port is already reclaimed
 * @retval -2 port is not in port range
 */
int reclaimPort(PortArray *pArray, unsigned short port);

#endif /* _PORTARRAY_H_ */
