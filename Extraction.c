#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "Extraction.h"
#include "Debug.h"

#define FEATS_EXTINTION ".feats"
/*
    Saves the the extracted features into file. the file will be stored in the same directory of the image, and with the same name.
    The format is as follow:
        <index>
        <dim>
        <p_coor>
    @param config - configuration file
    @param feats - the array of the extracted features
    @param count - the length of the array 'feats'
    @return a negative number in case of error. 0 otherwise.
*/
int saveExtractedFeatures(SPConfig config ,SPPoint* feats, int count){
    PDEBUG("saving features");
    if(config == NULL)
        return -1;
    if(feats == NULL)
        return -1;
    if(count < 0)
        return -1;



    if(count == 0)
        return 0;

    int index = spPointGetIndex(feats[0]);
    char filename[256];



    SP_CONFIG_MSG msg = spConfigGetFeatFilePath(filename,config,index,FEATS_EXTINTION);

    IF_ERROR_EXIT(msg);


    FILE* f = fopen(filename,"w");
    if(f == NULL){
        PDEBUG("the file could not be opened");
        LOG_WRITE_ERROR("the file could not be opened");
        return -1;
    }
    fprintf(f,"%d %d\n",count,index);
    for(int i=0;i<count;i++){
        for(int j=0;j<spPointGetDimension(feats[i]);j++){
            fprintf(f,"%f",spPointGetAxisCoor(feats[i],j));
            if(j == spPointGetDimension(feats[i])-1)
                fprintf(f,"\n");
            else
                fprintf(f," ");
        }
    }
    fclose(f);
    return 0;
}



/*
    Get image features from its corresponding '.feats' file
    @param config - configuration file
    @param feats - the index of the image we want to get the features of
    @param pfeats - the feature array will be stored here.
    @return the number of features
*/
int getExtractedFeatures(SPConfig config, int indexOfImage ,SPPoint** pfeats) {
    if(config == NULL)
        return -1;
    SP_CONFIG_MSG msg;
    if(indexOfImage >= spConfigGetNumOfImages(config,&msg) || indexOfImage < 0)
        return -1;

     char filename[256];

    msg = spConfigGetFeatFilePath(filename,config,indexOfImage,FEATS_EXTINTION);
    IF_ERROR_EXIT(msg);

     FILE* f = fopen(filename,"r");
    if(f == NULL){
        PDEBUG("the file could not be opened");
        LOG_WRITE_ERROR("the file could not be opened");
        return -1;
    }

    int count;
    int readIndex;
    int dim = spConfigGetPCADim(config,&msg);
    IF_ERROR_EXIT(msg);
    fscanf(f,"%d",&count);
    fscanf(f,"%d",&readIndex);
_D printf(" index is %d\n", readIndex);
    double* coords;
    SPPoint* featsarr = (SPPoint*)malloc(sizeof(SPPoint)*count);
    MALLOC_FAIL_INT(featsarr,-1);
    for(int i=0;i<count;i++){
        coords = (double*)malloc(sizeof(double)*dim);
        MALLOC_FAIL_INT(coords,-1);
        for(int j=0;j<dim;j++){
            fscanf(f,"%lf",&(coords[j]));
        }
        featsarr[i] = spPointCreate(coords,dim,readIndex);
        if(featsarr[i] == NULL){
            PDEBUG("NULL SPPoint read from file");
            LOG_WRITE_WARNING("NULL SPPoint read from file");
        }
    }
    *pfeats = featsarr;

    fclose(f);
    PDEBUG("finish reading points from file");
    return count;
}
