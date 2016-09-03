#include "KDTree.h"
#include "KDArray.h"
#include "Debug.h"
#include "SPPoint.h"
#include "SPConfig.h"
#include "SPBPriorityQueue.h"

#include <time.h>
#include <stdlib.h>
#include <stdio.h>

#define MALLOC_FAIL(x) if(!(x)){ return; }				//void
#define MALLOC_FAIL_NULL(x) if(!(x)){ return NULL; }	//returns null pointer
#define MALLOC_FAIL_INT(x,y) if(!(x)){ return (y); }	//returns integer y

#define INVALID -1

#define IF_ERROR_EXIT(x) if((x) != SP_CONFIG_SUCCESS){  PDEBUG("last called method did not success"); return NULL; }

struct kdtree_node_t{
	int dim;
	int val;
	struct kdtree_node_t* left;
	struct kdtree_node_t* right;
	SPPoint data;
};

struct kdtree_t{
	KDTreeNode head;
};

int spread(int* coor, int size){

	int max = -2;
	int min = 2*size;

	for(int i=0;i<size;i++){
		if(coor[i] > max)
			max = coor[i];
		if(coor[i] < min)
			min = coor[i];
	}

	return max - min;
}
int maxSpread(int** matrix, int size, int dim){
	int max = -1;
	int maxIndex = -1;
	int dimSpread;
	for(int i=0;i<dim;i++){
		dimSpread = spread(matrix[i],size);
		if(dimSpread > max){
			max = dimSpread;
			maxIndex = i;
		}
	}
	return maxIndex;
}



KDTreeNode BuildTreeFromKDArray(KDArray kda, splitMethod spKDTreeSplitMethod);

//TODO: while testing, check the case where size=0.
KDTree KDTInit(SPPoint* arr, int size, splitMethod spKDTreeSplitMethod){ //TODO: splitDimention is given by the system variables.
	PDEBUG("Initilizing KDTree")
	if(arr == 0 || size < 0){
		PDEBUG("null pointer was sent as parameter 'arr' or 'size' is negative.");
		return NULL;
	}
	KDArray kda = KDAInit(arr,size);
	KDTree kdt = (KDTree)malloc(sizeof(*kdt));
	kdt->head = BuildTreeFromKDArray(kda,spKDTreeSplitMethod);

	KDADestroySaveSPPoints(kda);
	return kdt;
}
KDTreeNode BuildTreeFromKDArray(KDArray kda, splitMethod spKDTreeSplitMethod){
	if(kda == NULL){
		PDEBUG("null pointer was sent as argument 'kda'");
		return NULL;
	}
	KDTreeNode kdtnode = (KDTreeNode)malloc(sizeof(*kdtnode));
	if(KDAGetSize(kda) == 1){
		kdtnode->dim = INVALID;
		kdtnode->val = INVALID;
		kdtnode->left = NULL;
		kdtnode->right = NULL;
		kdtnode->data = *(KDAGetPoints(kda));
		return kdtnode;
	}
	int splitDim;
	KDArray leftKda = NULL, rightKda = NULL;
	if(spKDTreeSplitMethod == MAX_SPREAD){
		splitDim = maxSpread(KDAGetMatrix(kda),KDAGetSize(kda),KDAGetDimension(kda));
	}else if(spKDTreeSplitMethod == RANDOM){
		srand((unsigned int)time(NULL)); //TODO: check random generator
		splitDim = rand() % KDAGetDimension(kda);

	}else if(spKDTreeSplitMethod == INCREMENTAL){
		static int lastSplitingDimension = -1; //TODO: check static decleration.
		lastSplitingDimension++;
		lastSplitingDimension %= KDAGetDimension(kda);
		splitDim = lastSplitingDimension;
	}else{
		PDEBUG("invalid spKDTreeSplitMethod.");
		return NULL;
	}

	int res = KDASplit(kda,splitDim,&leftKda,&rightKda);
	if(res < 1){
		PDEBUG("KDArray splitting failed.");
		return NULL;
	}
	kdtnode->dim = splitDim;
	kdtnode->val = KDAGetSize(kda)%2==0 ? KDAGetSize(kda)/2 : (KDAGetSize(kda)+1)/2;
	kdtnode->left = BuildTreeFromKDArray(leftKda,spKDTreeSplitMethod);
	kdtnode->right = BuildTreeFromKDArray(rightKda,spKDTreeSplitMethod);
	kdtnode->data = NULL;

	KDADestroySaveSPPoints(leftKda);
	KDADestroySaveSPPoints(rightKda);
	return kdtnode;
}

void KDTNDestroy(KDTreeNode kdtn){
	if(kdtn == NULL){
		PDEBUG("a null pointer was sent as argument 'kdtn'");
		return;
	}
	spPointDestroy(kdtn->data);
	if(kdtn->left != NULL)
		KDTNDestroy(kdtn->left);
	if(kdtn->right != NULL)
		KDTNDestroy(kdtn->right);
	free(kdtn);
}

void KDTDestroy(KDTree kdt){
	if(kdt == NULL){
		PDEBUG("a null pointer was sent as argument 'kdt'");
		return;
	}
	KDTNDestroy(kdt->head);
	//TODO: Did it free everything in KDTree?
	free(kdt);
}

bool isLeaf(KDTreeNode kdtn){
	if(KDTNGetLeftChild(kdtn) == NULL && KDTNGetRightChild(kdtn) == NULL)
		return true;
	else
		return false;
}

KDTreeNode KDTGetHead(KDTree kdt){
	if(!kdt) return NULL;
	return kdt->head;
}

int KDTNGetDimention(KDTreeNode kdtn){
	if(!kdtn) return -1;
	return kdtn->dim;
}
int KDTNGetValue(KDTreeNode kdtn){
	if(!kdtn) return -1;
		return kdtn->val;
}
SPPoint KDTNGetData(KDTreeNode kdtn){
	if(!kdtn) return NULL;
		return kdtn->data;
}
KDTreeNode KDTNGetLeftChild(KDTreeNode kdtn){
	if(!kdtn) return NULL;
		return kdtn->left;
}
KDTreeNode KDTNGetRightChild(KDTreeNode kdtn){
	if(!kdtn) return NULL;
		return kdtn->right;
}

double square(double a){
	return a*a;
}
void nearestNeighborsAux(KDTreeNode curr, SPBPQueue bpq, SPPoint p, int k);

//TODO: returns NULL in case of error
int* getNearestNeighbors(KDTree kdt, SPPoint p, SPConfig config){
	SP_CONFIG_MSG msg;
	int spKNN = spConfigGetSPKNN(config,&msg);
	 IF_ERROR_EXIT(msg);
	int k = spConfigGetNumOfSimilarImages(config,&msg);
	 IF_ERROR_EXIT(msg);
	int* nearest;
	SPBPQueue bpq = spBPQueueCreate(spKNN);
	nearestNeighborsAux(kdt->head,bpq,p,k);

	nearest = (int*)malloc(sizeof(int)*k);
	SPListElement temp;
	for(int i=0;i<k;i++){
		temp = spBPQueuePeek(bpq);
		nearest[i] = spListElementGetIndex(temp);
		spBPQueueDequeue(bpq);
	}
	spBPQueueDestroy(bpq);
	return nearest;
}

//TODO: returns 0 or -1 for error.
void nearestNeighborsAux(KDTreeNode curr, SPBPQueue bpq, SPPoint p, int k){
	if(curr == NULL)
		return;

	/* Add the current point to the BPQ. Note that this is a no-op if the
	* point is not as good as the points we've seen so far.*/
	if(isLeaf(curr)){
		double dist = spPointL2SquaredDistance(p,curr->data);
		int index = spPointGetIndex(curr->data);
		SPListElement element = spListElementCreate(index, dist);
		spBPQueueEnqueue(bpq,element);
		return;
	}

	/* Recursively search the half of the tree that contains the test point. */
	bool searchedOnLeft;
	if(spPointGetAxisCoor(p,curr->dim)<= curr->val){
		nearestNeighborsAux(curr->left,bpq,p,k);
		searchedOnLeft = true;
	}else{
		nearestNeighborsAux(curr->right,bpq,p,k);
		searchedOnLeft = false;
	}

	/* If the candidate hypersphere crosses this splitting plane, look on the
	* other side of the plane by examining the other subtree*/
	if(!spBPQueueIsFull(bpq) || square(spPointGetAxisCoor(p,curr->dim)-curr->val) < spBPQueueMaxValue(bpq)){
		if(searchedOnLeft)
			nearestNeighborsAux(curr->right,bpq,p,k);
		else
			nearestNeighborsAux(curr->left,bpq,p,k);
	}
}



