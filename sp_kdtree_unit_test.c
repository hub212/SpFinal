#include "../KDTree.h"
#include "../SPPoint.h"
#include "unit_test_util.h"
#include <stdlib.h>
#include <stdbool.h>
#include "../Debug.h"

//Checks if copy Works
#define NO_OF_POINTS 5
#define DIM 2

void printPoint_unit(SPPoint point) {
	if(point == NULL)
		return;
    printf("DEBUG : -------------- Index %d ---------------\n", spPointGetIndex(point));
    printf("DEBUG : \t\tdim : %d\n", spPointGetDimension(point));
    printf("DEBUG : \t\tcoordinates :\n");
    for (int i=0; i<spPointGetDimension(point); i++) {
        printf("DEBUG : \t\t\tcoor[%d] - %f\n",i,spPointGetAxisCoor(point,i));
    }
}


bool initMaxSpreadTest() {
	SPPoint* pts = (SPPoint*)malloc(sizeof(*pts)*NO_OF_POINTS);
	double vals[5][2] = {{1,2},{123,70},{2,7},{9,11},{3,4}};
	for(int i=0;i<NO_OF_POINTS;i++){
		pts[i] = spPointCreate(vals[i],2,i);
	}
	_D printPoint_unit(pts[0]);


	KDTree KDTMaxSpread = KDTInit(pts, NO_OF_POINTS, MAX_SPREAD);

	PDEBUG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>here is good");
	//return true;

	_D printPoint_unit(pts[0]);
	KDTreeNode tnode =  KDTGetHead(KDTMaxSpread);
	KDTreeNode left,right;
	KDTreeNode leftgrand1,leftgrand2,rightgrand1,rightgrand2;



	left = KDTNGetLeftChild(tnode);
	right = KDTNGetRightChild(tnode);



	ASSERT_TRUE(left != NULL);
	ASSERT_TRUE(right != NULL);


	leftgrand1 = KDTNGetLeftChild(left);
	leftgrand2 = KDTNGetRightChild(left);
	rightgrand1 = KDTNGetLeftChild(right);
	rightgrand2 = KDTNGetRightChild(right);




	ASSERT_TRUE(leftgrand1 != NULL);
	ASSERT_TRUE(leftgrand2 != NULL);
	ASSERT_TRUE(rightgrand1 != NULL);
	ASSERT_TRUE(rightgrand2 != NULL);

	ASSERT_TRUE(isLeaf(KDTNGetLeftChild(leftgrand1)) == true);
	ASSERT_TRUE(isLeaf(KDTNGetRightChild(leftgrand1)) == true);
	ASSERT_TRUE(isLeaf(leftgrand2) == true);
	ASSERT_TRUE(isLeaf(rightgrand1) == true);
	ASSERT_TRUE(isLeaf(rightgrand2) == true);


	PDEBUG("pss[0]:")
	_D printPoint_unit(pts[0]);
	PDEBUG("on tree:")
	_D printPoint_unit(KDTNGetData(KDTNGetLeftChild(leftgrand1)));
	ASSERT_TRUE(KDTNGetData(KDTNGetLeftChild(leftgrand1)) == pts[0]);
	ASSERT_TRUE(KDTNGetData(KDTNGetRightChild(leftgrand1)) == pts[2]);
	ASSERT_TRUE(KDTNGetData(leftgrand2) == pts[4]);
	ASSERT_TRUE(KDTNGetData(rightgrand1) == pts[3]);
	ASSERT_TRUE(KDTNGetData(rightgrand2) == pts[1]);

	//KDTDestroy(KDTMaxSpread); //TODO: free cause error.
	//for(int i=0;i<NO_OF_POINTS;i++)
	//		free(vals[i]);
	//free(vals);
	return true;
}

bool initRandomTest() {
	SPPoint* pts = (SPPoint*)malloc(sizeof(*pts)*NO_OF_POINTS);
	double vals[5][2] = {{1,2},{123,70},{2,7},{9,11},{3,4}};
	for(int i=0;i<NO_OF_POINTS;i++)
		pts[i] = spPointCreate(vals[i],2,i);

	KDTree KDTRandom = KDTInit(pts, NO_OF_POINTS, 1);


	//KDTDestroy(KDTRandom);//TODO: free cause error.
	//for (int i = 0; i < NO_OF_POINTS; i++)
	//	free(vals[i]);
	//free(vals);

	return true;
}
bool initIncrementalTest() {
	SPPoint* pts = (SPPoint*)malloc(sizeof(*pts)*NO_OF_POINTS);
	double vals[5][2] = {{1,2},{123,70},{2,7},{9,11},{3,4}};
	for(int i=0;i<NO_OF_POINTS;i++)
		pts[i] = spPointCreate(vals[i],2,i);

	KDTree KDTIncremental = KDTInit(pts, NO_OF_POINTS,2);


	//KDTDestroy(KDTIncremental);//TODO: free cause error.
	//for (int i = 0; i < NO_OF_POINTS; i++)
	//	free(vals[i]);
	//free(vals);

	return true;
}


int main() {
	//TODO: The tests are based on examples given in the assignment. Consider making more tests.
	//TODO: INCREMENTAL and RANDOM modes haven't been tested.
	_D setvbuf(stdout, NULL, _IONBF, 0); //TODO: delete at the end.
	PDEBUG("Testing KDTree")
	RUN_TEST(initMaxSpreadTest);
	RUN_TEST(initRandomTest);
	RUN_TEST(initIncrementalTest);
	return 0;
}

