#include "../KDArray.h"
#include "../SPPoint.h"
#include "unit_test_util.h"
#include <stdlib.h>
#include <stdbool.h>
#include "../Debug.h"

//Checks if copy Works
#define NO_OF_POINTS 5
#define DIM 2





void print_matrix_u(int rows, int cols,int** mat){
	for(int i=0;i<rows;i++){
			printf("\t[");
			for(int j=0;j<cols;j++)
				printf("%3d ",mat[i][j]);
			printf("]\n");
		}
}

bool equalMat(int rows, int cols,int** mat1, int mat2[rows][cols]){
	for(int i=0;i<rows;i++)
		for(int j=0;j<cols;j++){
			if(mat1[i][j] != mat2[i][j])
				return false;
		}
	return true;
}
bool equalArr(SPPoint* arr1, SPPoint* arr2, int size){
		for(int j=0;j<size;j++){
			if(arr1[j] != arr2[j]){
				_D printf("comparison failed on %dth element, the following:\n",j+1);
				//_D printPoint(arr1[j]);
				//_D printPoint(arr2[j]);
				return false;
			}
		}
	return true;
}

bool initTest() {
	SPPoint* pts = (SPPoint*)malloc(sizeof(*pts)*NO_OF_POINTS);
	double vals[NO_OF_POINTS][DIM] = {{1,2},{123,70},{2,7},{9,11},{3,4}};
	for(int i=0;i<NO_OF_POINTS;i++)
		pts[i] = spPointCreate(vals[i],2,i);

	KDArray kda = KDAInit(pts, NO_OF_POINTS);
	if(kda == NULL)
		return false;
	ASSERT_TRUE(KDAGetSize(kda) == NO_OF_POINTS);
	ASSERT_TRUE(KDAGetDimension(kda) == DIM);
	ASSERT_TRUE(equalArr(pts,KDAGetPoints(kda),NO_OF_POINTS) == true);

	int expectedMatrix[DIM][NO_OF_POINTS] = {{0,2,4,3,1},{0,4,2,3,1}};

	ASSERT_TRUE(equalMat(DIM,NO_OF_POINTS,KDAGetMatrix(kda),expectedMatrix) == true);
	PDEBUG("fine by here :)")
	KDADestroy(kda);
	PDEBUG("fine by here too :)")

	//for(int i=0;i<NO_OF_POINTS;i++)
	//	free(vals[i]);
	for(int i=0;i<DIM;i++)
		free(expectedMatrix[i]);
	//free(vals);
	//free(expectedMatrix);
	return true;
}


bool smallInitTest() {
	int no_of_points = 1;
	//int dim = 3;
	SPPoint* pts = (SPPoint*)malloc(sizeof(*pts)*no_of_points);
	double vals[1][3] = {{1,2,3}};
	for(int i=0;i<1;i++)
		pts[i] = spPointCreate(vals[i],3,i);

	KDArray kda = KDAInit(pts, 1);
	if(kda == NULL)
		return false;
	ASSERT_TRUE(KDAGetSize(kda) == 1);

	ASSERT_TRUE(KDAGetDimension(kda) == 3);
	ASSERT_TRUE(equalArr(pts,KDAGetPoints(kda),1) == true);

	int expectedMatrix[3][1] = {{1},{2},{3}};

	ASSERT_TRUE(equalMat(3,1,KDAGetMatrix(kda),expectedMatrix) == true);

	KDADestroy(kda);
	for(int i=0;i<1;i++)
		free(vals[i]);
	for(int i=0;i<3;i++)
		free(expectedMatrix[i]);
	//free(vals);
	//free(expectedMatrix);
	return true;
}

bool splitTest() {
	SPPoint* pts = (SPPoint*)malloc(sizeof(*pts)*NO_OF_POINTS);
	double vals[5][2] = {{1,2},{123,70},{2,7},{9,11},{3,4}};
	for(int i=0;i<NO_OF_POINTS;i++)
		pts[i] = spPointCreate(vals[i],2,i);

	KDArray kda = KDAInit(pts, NO_OF_POINTS);
	if(kda == NULL)
		return false;

	KDArray left = NULL, right= NULL;
	KDASplit(kda,0,&left,&right);

	ASSERT_TRUE(left != NULL);
	ASSERT_TRUE(right != NULL);

	//check left
	ASSERT_TRUE(KDAGetSize(left) == 3);
	ASSERT_TRUE(KDAGetDimension(left) == DIM);

	SPPoint* lexpectedPts = (SPPoint*)malloc(sizeofSPPoint()*3);
	lexpectedPts[0] = pts[0];
	lexpectedPts[1] = pts[2];
	lexpectedPts[2] = pts[4];
	int lexpectedMatrix[2][3] = {{0,1,2},{0,2,1}};
	ASSERT_TRUE(equalMat(DIM,3,KDAGetMatrix(left),lexpectedMatrix) == true);
	ASSERT_TRUE(equalArr(lexpectedPts,KDAGetPoints(left),3) == true);



	//free(lexpectedMatrix[0]);
	//free(lexpectedMatrix[1]);
	//free(lexpectedMatrix);

	for(int i=0;i<3;i++)
		spPointDestroy(lexpectedPts[i]);
	free(lexpectedPts);

	//check right
	ASSERT_TRUE(KDAGetSize(right) == 2);
	ASSERT_TRUE(KDAGetDimension(right) == DIM);

	SPPoint* rexpectedPts = (SPPoint*)malloc(sizeofSPPoint()*2);
	rexpectedPts[0] = pts[1];
	rexpectedPts[1] = pts[3];
	int rexpectedMatrix[2][2] = {{1,0},{1,0}};
	ASSERT_TRUE(equalMat(DIM,2,KDAGetMatrix(right),rexpectedMatrix) == true);
	ASSERT_TRUE(equalArr(rexpectedPts,KDAGetPoints(right),2) == true);


	//free(rexpectedMatrix[0]);
	//free(rexpectedMatrix[1]);
	//free(rexpectedMatrix);

	for (int i = 0; i < 2; i++)
		spPointDestroy(rexpectedPts[i]);
	free(rexpectedPts);

	for(int i=0;i<NO_OF_POINTS;i++)
			free(vals[i]);
	//free(vals);

	return true;
}
bool smallSplitTest() {
	SPPoint* pts = (SPPoint*)malloc(sizeof(*pts)*1);
	double vals[1][4] = {{1,2,3,4}};
	for(int i=0;i<1;i++)
		pts[i] = spPointCreate(vals[i],4,i);

	KDArray kda = KDAInit(pts, 1);
	if(kda == NULL)
		return false;

	KDArray left = NULL, right= NULL;
	KDASplit(kda,0,&left,&right);

	ASSERT_TRUE(left != NULL);
	ASSERT_TRUE(right == NULL);

	//check left
	ASSERT_TRUE(KDAGetSize(left) == 1);
	ASSERT_TRUE(KDAGetDimension(left) == 4);

	SPPoint* lexpectedPts = (SPPoint*)malloc(sizeofSPPoint());
	lexpectedPts[0] = pts[0];
	int lexpectedMatrix[4][1] = {{1},{2},{3},{4}};
	ASSERT_TRUE(equalMat(4,1,KDAGetMatrix(left),lexpectedMatrix) == true);
	ASSERT_TRUE(equalArr(lexpectedPts,KDAGetPoints(left),1) == true);

	//free(lexpectedMatrix[0]);
	//free(lexpectedMatrix[1]);
	//free(lexpectedMatrix);

	for(int i=0;i<1;i++)
		spPointDestroy(lexpectedPts[i]);
	free(lexpectedPts);

	for(int i=0;i<1;i++)
			free(vals[i]);
	//free(vals);

	return true;
}



int main() {
	//TODO: The tests are based on examples given in the assignment. Consider making more tests.
	_D setvbuf(stdout, NULL, _IONBF, 0); //TODO: delete at the end.
	RUN_TEST(initTest);
	RUN_TEST(splitTest);
	RUN_TEST(smallInitTest);
	RUN_TEST(smallSplitTest);
	printf("\ndone!\n");
	return 0;
}

