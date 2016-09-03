/*
 * KDTreeNode.h
 *
 *  Created on: 8 αιεμι 2016
 *      Author: user
 */
#include <stdbool.h>
#include "SPPoint.h"
#include "SPConfig.h"

#ifndef KDTREE_H_
#define KDTREE_H_

typedef struct kdtree_node_t* KDTreeNode;
typedef struct kdtree_t* KDTree;

KDTree KDTInit(SPPoint* arr, int size, splitMethod spKDTreeSplitMethod);

void KDTDestroy(KDTree kdt);

KDTreeNode KDTGetHead(KDTree kdt);

int KDTNGetDimention(KDTreeNode kdtn);

int KDTNGetValue(KDTreeNode kdtn);

SPPoint KDTNGetData(KDTreeNode kdtn);

KDTreeNode KDTNGetLeftChild(KDTreeNode kdtn);

KDTreeNode KDTNGetRightChild(KDTreeNode kdtn);

bool isLeaf(KDTreeNode kdtn);

int* getNearestNeighbors(KDTree kdt, SPPoint p, SPConfig config);

#endif /* KDTREE_H_ */
