#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "portarray.h"

/**
 * @brief create port array & initialization
 *
 * @param pArray pointer to new array
 * @param size  array size
 * @param portBase start port number of the array
 * @retval -1 locate array failed
 * @retval size number of available port (N * sizeof(int))
 */
int newPortArray(PortArray *pArray, int size, unsigned short portBase)
{
    int N;

    /* check size */
    if (size < 0)
	return -1;

    /* normalize size to N * sizeof(int) */
    N = (size / 32) + 1;

    /* initialize arrray */
    pArray->array = malloc(sizeof(unsigned int) * N);
    if (!pArray->array)
	return -1;

    memset(pArray->array, 0xffffffff, sizeof(unsigned int) * N);

    /* assign port range */
    pArray->N = N;
    pArray->portBase = portBase;
    pArray->portUpper = (unsigned short)(32 * N + portBase);

    return 32 * N;
}

/**
 * @brief delete port array
 *
 * @param pArray array to be delete
 */
void delPortArray(PortArray *pArray)
{
    free(pArray->array);
}

/** 
 * @brief search available port & clear bit
 *
 * @param pArray
 * @param port
 * @retval o success
 * @retval -1 no available port
 * @retval -2 input port is null
 */
int searchPort(PortArray *pArray, unsigned short *port)
{
    unsigned short row, col = 31;
    unsigned int *array = pArray->array;
    unsigned int target, mask;
    int N = pArray->N;

    if (port == NULL)
	return -2;

    /* Search available port */
    for (row = 0; row < N; row++)
    {
	target = array[row];
	if (target == 0) // no available port
	    continue;

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

	break;	// end search
    }

    if (row == N)	// no available port
	return -1;

    /* Clear bit in array to indicate port used  */
    mask = 1 << col;
    array[row] = array[row] ^ mask; // exclusive or

    /* return port number */
    *port = (unsigned short)(row * 32 + col + pArray->portBase);
    return 0;
}

/**
 * @brief reclaim port & set bit
 *
 * @param pArray
 * @param port port number
 * @retval 0 success
 * @retval -1 port is already reclaimed
 * @retval -2 port is not in port range
 */
int reclaimPort(PortArray *pArray, unsigned short port)
{
    int row, col;
    unsigned int mask;
    unsigned int *array = pArray->array;

    /* check port in range */
    if ((port < pArray->portBase) || (port >= pArray->portUpper))
	return -2;

    /* locate posistion to be set */
    port -= pArray->portBase;
    row = port / 32;
    col = port % 32;

    /* generate mask */
    mask = 1 << col;

    /* test and set */
    if (array[row] & mask)	// bit is already set
	return -1;
    else 
	array[row] |= mask;

    return 0;
}
