#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "portPool.h"

/**
 * @brief create port pool & initialization
 *
 * @param pPool pointer to new pool
 * @param size  pool size
 * @param portBase start port number of the pool
 * @retval -1 locate pool failed
 * @retval size number of available port (N * sizeof(int))
 */
int newPortPool(PortPool *pPool, int size, unsigned short portBase)
{
    int N;

    /* check size */
    if (size < 0)
	return -1;

    /* normalize size to N * sizeof(int) */
    N = (size / 32) + 1;

    /* initialize arrray */
    pPool->pool = malloc(sizeof(unsigned int) * N);
    if (!pPool->pool)
	return -1;

    memset(pPool->pool, 0xffffffff, sizeof(unsigned int) * N);

    /* assign port range */
    pPool->next = 0;
    pPool->N = N;
    pPool->portBase = portBase;
    pPool->portUpper = (unsigned short)(32 * N + portBase);

    return 32 * N;
}

/**
 * @brief delete port pool
 *
 * @param pPool pool to be delete
 */
void delPortPool(PortPool *pPool)
{
    free(pPool->pool);
}

/** 
 * @brief search available port & clear bit
 *
 * @param pPool
 * @param port
 * @retval o success
 * @retval -1 no available port
 * @retval -2 input port is null
 */
int searchPort(PortPool *pPool, unsigned short *port)
{
    unsigned short row, col = 31;
    unsigned int *pool = pPool->pool;
    unsigned int target, mask;
    int N = pPool->N;
    int next = pPool->next;

    if (port == NULL)
	return -2;

    /* Check row first, find available row */
    /* 
     * Sometimes, the port of a closed socket will not be reclaimed immediately.
     * Here, we prevent to resue the port which is just reclaimed.
     */

    /* start from next search point */
    for (row = next; row < N; row ++) {
	if ((target = pool[row]) != 0)	// found
	    break;
    }

    /* if not found, search from row(0) to row(next-1) */
    if (row == N) {
	for (row = 0; row < next; row ++) {
	    if ((target = pool[row]) != 0)	// found
		break;
	}

	if (row == next)    // no available port
	    return -1;
    }

    /* locate available column (port) */
    /* Test lower 16 bits */
    if (target & 0x0000ffff)
	col -= 16;
    else
	target >>= 16;

    /* Test lower 8 bits */
    if (target & 0x000000ff)
	col -= 8;
    else
	target >>= 8;

    /* Test lower 4 bits */
    if (target & 0x0000000f)
	col -= 4;
    else
	target >>= 4;

    /* Test lower 2 bits */
    if (target & 0x00000003)
	col -= 2;
    else
	target >>= 2;

    /* Test lowest bit */
    if (target & 0x00000001)
	col -= 1;

    /* column located */

    /* Clear bit in pool to indicate port used  */
    mask = 1 << col;
    pool[row] = pool[row] ^ mask; // exclusive or

    /* index next search */
    pPool->next = row + 1;  // next search would start from next row
    if (pPool->next == N)
	pPool->next = 0;

    /* return port number */
    *port = (unsigned short)(row * 32 + col + pPool->portBase);

    return 0;
}

/**
 * @brief reclaim port & set bit
 *
 * @param pPool
 * @param port port number
 * @retval 0 success
 * @retval -1 port is already reclaimed
 * @retval -2 port is not in port range
 */
int reclaimPort(PortPool *pPool, unsigned short port)
{
    int row, col;
    unsigned int mask;
    unsigned int *pool = pPool->pool;

    /* check port in range */
    if ((port < pPool->portBase) || (port >= pPool->portUpper))
	return -2;

    /* locate posistion to be set */
    port -= pPool->portBase;
    row = port / 32;
    col = port % 32;

    /* generate mask */
    mask = 1 << col;

    /* test and set */
    if (pool[row] & mask)	// bit is already set
	return -1;
    else 
	pool[row] |= mask;

    return 0;
}
