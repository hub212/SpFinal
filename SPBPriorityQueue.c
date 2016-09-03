/*
 * SPBPriorityQueue.c
 *
 *  Created on: 22 במאי 2016
 *      Author: user
 */

#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include "SPListElement.h"
#include "SPList.h"
#include "SPBPriorityQueue.h"


struct sp_bp_queue_t{
	int maxSize;
	SPList elements;
};


void moveIteratorToElement(SPList list, SPListElement element){
	SPListElement head = spListGetFirst(list);
	while(head != element)
			head = spListGetNext(list);
}
void dequeueHighestValue(SPBPQueue source){
	spListGetLast(source->elements);
	spListRemoveCurrent(source->elements);
}




//header implementation (public functions)

SPBPQueue spBPQueueCreate(int maxSize){
	SPBPQueue queue = (SPBPQueue)malloc(sizeof(*queue));
	if(queue == NULL)
		return NULL;
	queue->maxSize = maxSize>0?maxSize:0;
	queue->elements = spListCreate();
	return queue;
}

SPBPQueue spBPQueueCopy(SPBPQueue source){
	if(source == NULL)
		return NULL;
	SPBPQueue copy = (SPBPQueue)malloc(sizeof(*copy));
	if(copy == NULL)
		return NULL;
	copy->maxSize = source->maxSize;
	copy->elements = spListCopy(source->elements);
	return copy;
}

void spBPQueueDestroy(SPBPQueue source){
	if(source == NULL)
		return;

	spListDestroy(source->elements);
	free(source);
}

void spBPQueueClear(SPBPQueue source){
	if(source == NULL)
		return;
	spListClear(source->elements);
}

int spBPQueueSize(SPBPQueue source){
	if(source == NULL)
		return -1;
	return spListGetSize(source->elements);
}

int spBPQueueGetMaxSize(SPBPQueue source){
	if(source == NULL)
		return -1;
	return source->maxSize;
}

SP_BPQUEUE_MSG spBPQueueEnqueue(SPBPQueue source, SPListElement element){
	SP_LIST_MSG msg;

	if(source == NULL || element == NULL)
		return SP_BPQUEUE_INVALID_ARGUMENT;

	SPListElement elementCopy = spListElementCopy(element);


	if(spBPQueueSize(source) != 0){
		SPListElement iterator = spListGetFirst(source->elements);
		int elemVal = spListElementGetValue(element);
		while(iterator != NULL && spListElementGetValue(iterator) < elemVal){
			iterator = spListGetNext(source->elements);
		}

		if(iterator != NULL)
			msg = spListInsertBeforeCurrent(source->elements,elementCopy);
		else
			msg = spListInsertLast(source->elements,elementCopy);

		spListElementDestroy(elementCopy);

		if(msg == SP_LIST_OUT_OF_MEMORY)
			return SP_BPQUEUE_OUT_OF_MEMORY;

		//if queue was full before enqueue, removes the element with highest value
		if(spBPQueueSize(source) <= source->maxSize){
			return SP_BPQUEUE_SUCCESS;
		}else{
			dequeueHighestValue(source);
			return SP_BPQUEUE_FULL;
		}
		return SP_BPQUEUE_SUCCESS;
	}else{//if queue is empty
		msg = spListInsertLast(source->elements,elementCopy);
		spListElementDestroy(elementCopy);
		if(msg == SP_LIST_OUT_OF_MEMORY)
			return SP_BPQUEUE_OUT_OF_MEMORY;
		else
			return SP_BPQUEUE_SUCCESS;
	}


	return SP_BPQUEUE_EMPTY;
}

SP_BPQUEUE_MSG spBPQueueDequeue(SPBPQueue source){
	if(source == NULL)
		return  SP_BPQUEUE_INVALID_ARGUMENT;
	if(spBPQueueSize(source) == 0)
		return SP_BPQUEUE_EMPTY;

	spListGetFirst(source->elements);
	spListRemoveCurrent(source->elements);
	return SP_BPQUEUE_SUCCESS;
}


SPListElement spBPQueuePeek(SPBPQueue source){
	if(source == NULL)
		return NULL;
	if(spBPQueueSize(source) == 0)
		return NULL;

	SPListElement iteratorLocation = spListGetCurrent(source->elements);
	SPListElement head = spListGetFirst(source->elements);
	SPListElement copy = spListElementCopy(head);

	//sets the iterator back to where it was
	moveIteratorToElement(source->elements, iteratorLocation);

	return copy;
}



SPListElement spBPQueuePeekLast(SPBPQueue source){
	if(source == NULL)
			return NULL;
	if(spBPQueueSize(source) == 0)
		return NULL;

	SPListElement iteratorLocation = spListGetCurrent(source->elements);
	SPListElement head = spListGetLast(source->elements);
	SPListElement copy = spListElementCopy(head);


	//sets the iterator back to where it was
	moveIteratorToElement(source->elements, iteratorLocation);

	return copy;
}

double spBPQueueMinValue(SPBPQueue source){
	if(source == NULL)
		return -1;
	SPListElement minElement = spBPQueuePeek(source);
	if(minElement == NULL)
		return -1;

	float v = spListElementGetValue(minElement);
	spListElementDestroy(minElement);
	return v;
}

double spBPQueueMaxValue(SPBPQueue source){
	if(source == NULL)
			return -1;
	SPListElement maxElement = spBPQueuePeekLast(source);
	if(maxElement == NULL)
		return -1;
	float v = spListElementGetValue(maxElement);
	spListElementDestroy(maxElement);
	return v;
}

bool spBPQueueIsEmpty(SPBPQueue source){
	if(source == NULL)
		return true;
	return spBPQueueSize(source)==0;
}

bool spBPQueueIsFull(SPBPQueue source){
	if(source == NULL)
		return false;
	return spBPQueueSize(source)==source->maxSize;
}
