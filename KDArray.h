#include "SPPoint.h"

typedef struct kdarray_t* KDArray;

/**
returns a positive value if succeed
*/
KDArray KDAInit(SPPoint* arr, int size);

/**
returns a positive value if succeed
*/
int KDASplit(KDArray kdArr, int coor, KDArray* kdLeft, KDArray* kdRight);

void KDADestroy(KDArray kdArr);

/**
 * Destroy pointers to SPPoints related to the KDA. but does not destroy the points themselves.
 */
void KDADestroySaveSPPoints(KDArray kdArr);

int KDAGetDimension(KDArray kdArr);

int KDAGetSize(KDArray kdArr);

SPPoint* KDAGetPoints(KDArray kdArr);

int** KDAGetMatrix(KDArray kdArr);
