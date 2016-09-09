#include <stdlib.h>
#include <stdio.h>
#include "KDArray.h"
#include "Debug.h"


#define MALLOC_FAIL(x) if(!(x)){ if(DEBUG) {PDEBUG("DEBUG: malloc failed");} return; }				//void
#define MALLOC_FAIL_NULL(x) if(!(x)){ if(DEBUG) {PDEBUG("DEBUG: malloc failed");} return NULL; }	//returns null pointer
#define MALLOC_FAIL_INT(x,y) if(!(x)){ if(DEBUG) {PDEBUG("DEBUG: malloc failed");} return (y); }	//returns integer y


void printPoint(SPPoint point);
void print_matrix(int** mat, int rows, int cols);
void print_kdarray(KDArray kdarray);


struct kdarray_t{
	SPPoint* points;
    int** mat;
    int dimension;
    int size;
};

int matrixcmp(const void * va, const void * vb){
    int * a = *((int**)va);
    int * b = *((int**)vb);
    return *a - *b;
}
//TODO: DKAMatrix should be double**!
int getIndexesSorted(SPPoint* arr, int size, int coor, int * res){
    SPPoint* p = (SPPoint*)malloc(size * sizeof(SPPoint));
    MALLOC_FAIL_INT(p,-1);
    for(int i=0;i<size;i++)
        p[i] = spPointCopy(arr[i]);


    int ** arranger =  (int**)malloc(size * sizeof(int*));
    MALLOC_FAIL_INT(arranger,-1);
    for(int i=0;i<size;i++){
        arranger[i] = (int*)malloc(2 * sizeof(int));
        MALLOC_FAIL_INT(arranger[i],-1);
    }

    for(int i=0;i<size;i++){
        arranger[i][0] = spPointGetAxisCoor(p[i],coor);
        arranger[i][1] = i;
    }
 //   _D print_matrix(arranger,size,2);
    qsort(arranger,size,sizeof(int*),matrixcmp);
    PDEBUG("sorted array");
    for(int i=0;i<size;i++){
        res[i] = arranger[i][1];
    }

    for(int i=0;i<size;i++)
        spPointDestroy(p[i]);
    free(p);

    free(arranger[0]);
    free(arranger[1]);
    free(arranger);
    return 0;
}

KDArray KDAInit(SPPoint* arr, int size){
	PDEBUG("Initilazing KDArray.");
    if(arr == NULL){
    	PDEBUG("null pointer was sent as argument 'arr'.");
        return NULL;
    }

    KDArray  kdarray = (KDArray)malloc(sizeof(struct kdarray_t));
    MALLOC_FAIL_NULL(kdarray)
    if(size == 0){
    	PDEBUG("null pointer returned.");
        kdarray->mat = NULL;
        return kdarray; //TODO: Check this result!
    }

    int sortRes;
    int dim = spPointGetDimension(arr[0]);
    int ** dimMatrix = (int**)malloc(dim * sizeof(int*));
    MALLOC_FAIL_NULL(dimMatrix)
    for(int i=0;i<dim;i++){
        dimMatrix[i] = (int*)malloc(size * sizeof(int));
        MALLOC_FAIL_NULL(dimMatrix[i])
        sortRes=getIndexesSorted(arr, size ,i,dimMatrix[i]);
        if(sortRes != 0){
        	PDEBUG("sorting failed.");
        	//TODO: memory leak?
        	return NULL;
        }
    }

    kdarray->mat = dimMatrix;
    kdarray->dimension = dim;
    kdarray->size = size;
    kdarray->points = arr;

    if(DEBUG){
    	//print_kdarray(kdarray);
    }
PDEBUG("__was here too");
    return kdarray;
}


int KDASplit(KDArray kdArr, int coor, KDArray* kdLeft, KDArray* kdRight){
	PDEBUG("Splitting KDArray.");
    if(kdArr == NULL){
    	PDEBUG("null pointer was sent in argument kdArr. ");
        return -1;
    }
    if(coor >= kdArr->dimension){
    	PDEBUG("'coor' is invalid.")
    	return -1;
    }




    int size = kdArr->size;

    int* arrangement = kdArr->mat[coor];
    int ptsOnLeft = size%2==0 ? size/2 : (size+1)/2;

     PDEBUG("\t\tTHis is ME")
    _D printf("size: %d points: %d\n", size,ptsOnLeft);

    int* X = (int*)malloc(sizeof(int)*size);
    MALLOC_FAIL_INT(X,-1)
    for(int i=0;i<size;i++)
    	X[i] = 1;
PDEBUG("\t\tTHis is ME again")
    for(int i=0;i<ptsOnLeft;i++){
        _D printf("arrangement[%d] = %d", i, arrangement[i]);
    	*(X+arrangement[i]) = 0;
    }



    SPPoint *left = (SPPoint*)malloc(sizeof(SPPoint)*ptsOnLeft);
    SPPoint *right = (SPPoint*)malloc(sizeof(SPPoint)*(size-ptsOnLeft));
    int *map1 = (int*)malloc(sizeof(int)*ptsOnLeft);
    int *map2 = (int*)malloc(sizeof(int)*(size-ptsOnLeft));
    MALLOC_FAIL_INT(left,-1)
    MALLOC_FAIL_INT(right,-1)
	MALLOC_FAIL_INT(map1,-1)
	MALLOC_FAIL_INT(map2,-1)


	 int dim = kdArr->dimension;
	 int** lMatrix = NULL;
	 int** rMatrix = NULL;
	 lMatrix = (int**)malloc(sizeof(int*)*dim);
	 rMatrix = (int**)malloc(sizeof(int*)*dim);
	 MALLOC_FAIL_INT(lMatrix,-1);
	 MALLOC_FAIL_INT(rMatrix,-1);


	 for(int i=0;i<size;i++){
	 		map1[i] = map2[i] = -1;
	 }

	int li=0,ri=0;
	for(int i=0;i<size;i++){
		if(X[i] == 0){
			left[li] = kdArr->points[i];
			map1[i] = li;
			li++;
		}else{
			right[ri] = kdArr->points[i];
			map2[i] = ri;
			ri++;
		}
	}


	 for(int i=0;i<dim;i++){
	  	lMatrix[i] = (int*)malloc(ptsOnLeft * sizeof(int));
	  	rMatrix[i] = (int*)malloc((size-ptsOnLeft) * sizeof(int));
	   	MALLOC_FAIL_INT(lMatrix[i],-1)
	  	MALLOC_FAIL_INT(rMatrix[i],-1)


		li=0; ri=0;
	   	//TODO: The calculations here are probably incorrect. Need to be checked.
		for(int j=0;j<size;j++){
			if(j < ptsOnLeft){
				lMatrix[i][li] = map1[kdArr->mat[i][j]];
				li++;
			}else{
				rMatrix[i][ri] = map2[kdArr->mat[i][j]];
				ri++;
			}
		}
	 }

	 PDEBUG("\t\t>>>>>>>>>>>>>>THis is ME again and so on..")

	 KDArray leftkda,rightkda;
	 leftkda = (KDArray)malloc(sizeof(struct kdarray_t));
	 rightkda = (KDArray)malloc(sizeof(struct kdarray_t));
	 leftkda->dimension = dim;
	 leftkda->size = ptsOnLeft;
	 leftkda->points = left;
	 leftkda->mat = lMatrix;


	 rightkda->dimension = dim;
	 rightkda->size = size-ptsOnLeft;
	 rightkda->points = right;
	 rightkda->mat = rMatrix;


	*kdLeft = leftkda;
	*kdRight = rightkda;

	if(DEBUG){
		PDEBUG("splitted into:")
		//print_kdarray(*kdLeft);
		//printf("and:\n");
		//print_kdarray(*kdRight);
	}

	//free(map2); //TODO: these memory-freeing cause an error for some reason.
	//free(map1);
	free(X);

	return 1;
}



void printPoint(SPPoint point) {
	if(point == NULL)
		return;
    printf("DEBUG : -------------- Index %d ---------------\n", spPointGetIndex(point));
    printf("DEBUG : \t\tdim : %d\n", spPointGetDimension(point));
    printf("DEBUG : \t\tcoordinates :\n");
    for (int i=0; i<spPointGetDimension(point); i++) {
        printf("DEBUG : \t\t\tcoor[%d] - %f\n",i,spPointGetAxisCoor(point,i));
    }
}

void print_matrix(int** mat, int rows, int cols){
	for(int i=0;i<rows;i++){
		printf("\t[");
		for(int j=0;j<cols;j++)
			printf("%3d ",mat[i][j]);
		printf("]\n");
	}
}

void print_kdarray(KDArray kdarray){
	printf("DEBUG: (KDAInit) Returned kdarray:\ndimention: %d\nsize: %d\n points:\n",kdarray->dimension,kdarray->size);
	for(int i=0;i<kdarray->size;i++)
		printPoint(kdarray->points[i]);
	printf("matrix: \n");
	print_matrix(kdarray->mat, kdarray->dimension,kdarray->size);
}

void KDADestroy(KDArray kdArr){
	if(kdArr == NULL)
		return;
	for(int i=0;i<kdArr->size;i++)
		spPointDestroy((kdArr->points)[i]);
	free(kdArr->points);

	for(int i=0;i<kdArr->dimension;i++)
		free((kdArr->mat[i]));
	free(kdArr->mat);
	free(kdArr);
}
void KDADestroySaveSPPoints(KDArray kdArr){
	if(kdArr == NULL)
		return;
	free(kdArr->points);

	for(int i=0;i<kdArr->dimension;i++)
		free((kdArr->mat[i]));
	free(kdArr->mat);
	free(kdArr);
}


int KDAGetDimension(KDArray kdArr){
	if(!kdArr){
		PDEBUG("null pointer was sent.")
		return 0;
	}
	return kdArr->dimension;
}
int KDAGetSize(KDArray kdArr){
	if(!kdArr){
		PDEBUG("null pointer was sent.")
		return 0;
	}
	return kdArr->size;
}
SPPoint* KDAGetPoints(KDArray kdArr){
	if(!kdArr){
		PDEBUG("null pointer was sent.")
		return NULL;
	}
	return kdArr->points;
}
int** KDAGetMatrix(KDArray kdArr){
	if(!kdArr){
		PDEBUG("null pointer was sent.")
		return NULL;
	}
	return kdArr->mat;
}
