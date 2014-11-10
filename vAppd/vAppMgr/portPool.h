#ifndef _PORTPOOL_H_
#define _PORTPOOL_H_

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
    unsigned int *pool;
    int next;	// index of start point for next search
} PortPool;

/**
 * @brief create port pool & initialization
 *
 * @param pPool pointer to new pool
 * @param size  pool size
 * @param portBase start port number of the pool
 * @retval -1 locate pool failed
 * @retval size number of available port (N * sizeof(int))
 */
int newPortPool(PortPool *pPool, int size, unsigned short portBase);

/**
 * @brief delete port pool
 *
 * @param pPool pool to be delete
 */
void delPortPool(PortPool *pPool);

/** 
 * @brief search available port & clear bit
 *
 * @param pPool
 * @param port
 * @retval 0 success
 * @retval -1 no available port
 * @retval -2 input port is null
 */
int searchPort(PortPool *pPool, unsigned short *port);

/**
 * @brief reclaim port & set bit
 *
 * @param pPool
 * @param port port number
 * @retval 0 success
 * @retval -1 port is already reclaimed
 * @retval -2 port is not in port range
 */
int reclaimPort(PortPool *pPool, unsigned short port);

#endif /* _PORTPOOL_H_ */
