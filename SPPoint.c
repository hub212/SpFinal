//
//  SPPoint.c
//  SP3
//
//  Created by Shlomi Zabari on 5/25/16.
//  Copyright (c) 2016 Shlomi Zabari. All rights reserved.
//
#define DEBUG 0

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "SPPoint.h"

void print_point(SPPoint point);

/** Copy arrays **/
void copying_arr(double* orig, double* dest, int dim){

    int i;
    for (i = 0; i < dim; i++)
        *(dest +i) = *(orig + i);
}

/**
 * Allocates a new point in the memory.
 * Given data array, dimension dim and an index.
 * The new point will be P = (p_0,p_2,...,p_{dim-1})
 * such that the following holds
 *
 * - The ith coordinate of the P will be p_i
 * - p_i = data[i]
 * - The index of P = index
 *
 * @return
 * NULL in case allocation failure ocurred OR data is NULL OR dim <=0 OR index <0
 * Otherwise, the new point is returned
 */
SPPoint spPointCreate(double* data, int dim, int index) {

    assert(dim>0);
    assert(index>=0);

    SPPoint new_point;

    // allocationg memory for point
    if((new_point = (SPPoint)malloc(sizeof(int)*2+sizeof(double*))) == NULL){
        perror("ERROR : spPointCreate failed - ");
        return NULL;
    }

    // alocating memory for point's coordinates
    if ((new_point->p_coor = (double*)malloc(sizeof(double)*dim)) == NULL){
        perror("ERROR : spPointCreate failed - ");
        free(new_point);
        return NULL;
    }

    // copying data to point
    copying_arr(data, new_point->p_coor, dim);
    new_point->index = index;
    new_point->dim   = dim;

    if (DEBUG){
        printf("DEBUG : [New point Created]\n");
        //print_point(new_point);
    }

    return new_point;
}

/**
 * Allocates a copy of the given point.
 *
 * Given the point source, the functions returns a
 * new pint P = (P_1,...,P_{dim-1}) such that:
 * - P_i = source_i (The ith coordinate of source and P are the same)
 * - dim(P) = dim(source) (P and source have the same dimension)
 * - index(P) = index(source) (P and source have the same index)
 *
 * @param source - The source point
 * @assert (source != NUlL)
 * @return
 * NULL in case memory allocation occurs
 * Others a copy of source is returned.
 */
SPPoint spPointCopy(SPPoint source) {
    assert(source != NULL);
    if (DEBUG)
        printf("DEBUG : [copying point]\n");
    return spPointCreate(source->p_coor, source->dim, source->index);
}

/**
 * Free all memory allocation associated with point,
 * if point is NULL nothing happens.
 */
void spPointDestroy(SPPoint point) {
    if (DEBUG)
        printf("DEBUG : [freeing memory]\n");
    // free coordenation allocation
    free(point->p_coor);
    // free point's allocation
    free(point);
}

/**
 * A getter for the dimension of the point
 *
 * @param point - The source point
 * @assert point != NULL
 * @return
 * The dimension of the point
 */
int spPointGetDimension(SPPoint point){
    assert(point != NULL);
    if(DEBUG){
        printf("DEBUG : [spPointGetDimension]\n");
        //print_point(point);
        printf("DEBUG : dim is %d\n", point->index);
    }
    return point->dim;
}

/**
 * A getter for the index of the point
 *
 * @param point - The source point
 * @assert point != NULL
 * @return
 * The index of the point
 */
int spPointGetIndex(SPPoint point){
    assert(point != NULL);
    if(DEBUG){
        printf("DEBUG : [spPointGetIndex]\n");
        //print_point(point);
        printf("DEBUG : index is %d\n", point->index);
    }
    return point->index;
}

/**
 * A getter for specific coordinate value
 *
 * @param point - The source point
 * @param axis  - The coordinate of the point which
 * 				  its value will be retreived
 * @assert point!=NULL && axis < dim(point)
 * @return
 * The value of the given coordinate (p_axis will be returned)
 */
double spPointGetAxisCoor(SPPoint point, int axis){
    assert (point != NULL);
    assert (axis<point->dim);
    if(DEBUG){
        printf("DEBUG : [spPointGetAxisCoor]\n");
        //print_point(point);
        printf("DEBUG : coordinate on axis %d is %f\n", axis, point->p_coor[axis]);
    }
    return point->p_coor[axis];
}

/**
 * Calculates the L2-squared distance between p and q.
 * The L2-squared distance is defined as:
 * (p_1 - q_1)^2 + (p_2 - q_1)^2 + ... + (p_dim - q_dim)^2
 *
 * @param p - The first point
 * @param q - The second point
 * @assert p!=NULL AND q!=NULL AND dim(p) == dim(q)
 * @return
 * The L2-Squared distance between p and q
 */
double spPointL2SquaredDistance(SPPoint p, SPPoint q){
    assert((p != NULL) && (q != NULL) && (p->dim == q->dim));

    double dist = 0;
    int i;

    for (i = 0; i < q->dim; i++)
        dist+=(q->p_coor[i]-p->p_coor[i])*(q->p_coor[i]-p->p_coor[i]);
    if (DEBUG) {
        printf("DEBUG : [spPointL2SquaredDistance]\n");
        //print_point(p);
        //print_point(q);
        printf("DEBUG : distance between points is %f\n", dist);
    }

    return dist;
}


void print_point(SPPoint point) {
    printf("DEBUG : %d ;", point->index);
    for (int i=0; i<point->dim; i++) {
        printf("%.1f , ",point->p_coor[i]);
    }
    printf("\n");
}

int sizeofSPPoint(){
	return sizeof(struct sp_point_t);
}
