#ifndef SPBPRIORITYQUEUE_H_
#define SPBPRIORITYQUEUE_H_
#include "SPListElement.h"
#include <stdbool.h>
/**
 * SP Bounded Priority Queue summary
 *
 * TODO Complete documentation
 */


/** type used to define Bounded priority queue **/
typedef struct sp_bp_queue_t* SPBPQueue;

/** type for error reporting **/
typedef enum sp_bp_queue_msg_t {
	SP_BPQUEUE_OUT_OF_MEMORY,
	SP_BPQUEUE_FULL,
	SP_BPQUEUE_EMPTY,
	SP_BPQUEUE_INVALID_ARGUMENT,
	SP_BPQUEUE_SUCCESS
} SP_BPQUEUE_MSG;

/**
 * Allocates a new BPQ.
 *
 * This function creates a new empty BPQ.
 * @param maxSize maximum number of elements in the BPQ
 * @return
 * 	NULL - If allocations failed.
 * 	A new BPQ in case of success.
 */
SPBPQueue spBPQueueCreate(int maxSize);

/**
 * Creates a copy of target BPQ.
 *
 * The new copy will contain all the elements from the source BPQ in the same
 * order.
 *
 * @param source The target BPQ to copy
 * @return
 * NULL if a NULL was sent or a memory allocation failed.
 * A BPQ containing the same elements with same order as source otherwise.
 */
SPBPQueue spBPQueueCopy(SPBPQueue source);

/**
 * Deallocates an existing BPQ. Clears all elements by using the
 * stored free function.
 *
 * @param source Target BPQ to be deallocated. If BPQ is NULL nothing will be
 * done
 */
void spBPQueueDestroy(SPBPQueue source);

/**
 * Removes all elements from target BPQ.
 *
 * @param source Target BPQ to remove all element from
 */
void spBPQueueClear(SPBPQueue source);

/**
 * Returns the number of elements in a BPQ
 *
 * @param source The target BPQ which size is requested.
 * @return
 * -1 if a NULL pointer was sent.
 * Otherwise the number of elements in the list.
 */
int spBPQueueSize(SPBPQueue source);

/**
 * TODO Complete documentation
 */
int spBPQueueGetMaxSize(SPBPQueue source);

/**
 * Adds a new copy of element to the BPQ, the new element will be the inserted before the first element that has a bigger value than the new element.
 * If the queue is full, the new element will be enqueued and afterwards the element with the highest value will be dequeued.
 *
 *
 * @param source The BPQ for which to add an element
 * @param element The element to enqueue. A copy of the element will be
 * inserted
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if a NULL was sent as BPQ
 * SP_BPQUEUE_FULL if source BPQ is full
 * SP_BPQUEUE_SUCCESS if the element has been enqueued successfully
 */
SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element);//TODO fix documentation

/**
 * Removes the element with the lowest value from the BPQ.
 *
 * @param source The BPQ for which the current element will be removed
 * @return
 * SP_BPQUEUE_INVALID_ARGUMENT if BPQ is NULL
 * SP_BPQUEUE_EMPTY if the BPQ is empty
 * SP_LIST_SUCCESS the current element was removed successfully
 */
SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source);//TODO fix documentation

/**
 * if size = 0 return null
 * TODO Complete documentation
 */
SPListElement spBPQueuePeek(SPBPQueue source);//TODO fix documentation

/**
 * TODO Complete documentation
 */
SPListElement spBPQueuePeekLast(SPBPQueue source);//TODO fix documentation

/**
 * TODO Complete documentation
 */
double spBPQueueMinValue(SPBPQueue source);//TODO fix documentation

/**
 * TODO Complete documentation
 */
double spBPQueueMaxValue(SPBPQueue source);//TODO fix documentation

/**
 * TODO Complete documentation
 */
bool spBPQueueIsEmpty(SPBPQueue source);//TODO fix documentation

/**
 * TODO Complete documentation
 */
bool spBPQueueIsFull(SPBPQueue source);//TODO fix documentation

#endif
