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
    char filename[STR_MAX];



    SP_CONFIG_MSG msg = spConfigGetFeatFilePath(filename,config,index,FEATS_EXTINTION);

   if(msg != SP_CONFIG_SUCCESS){
        PDEBUG("last method failed.")
        return ERROR_VALUE;
    }


    FILE* f = fopen(filename,"w");
    if(f == NULL){
        PDEBUG("the file could not be opened");
        return ERROR_VALUE;
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

     char filename[STR_MAX];

    msg = spConfigGetFeatFilePath(filename,config,indexOfImage,FEATS_EXTINTION);
    if(msg != SP_CONFIG_SUCCESS){
             PDEBUG("the file could not be opened");
        return ERROR_VALUE;
    }

     FILE* f = fopen(filename,"r");
    if(f == NULL){
        PDEBUG("the file could not be opened");
        return ERROR_VALUE;
    }

    int count;
    int readIndex;
    int dim = spConfigGetPCADim(config,&msg);
     if(msg != SP_CONFIG_SUCCESS){
        PDEBUG("the file could not be opened");
        fclose(f);
        return ERROR_VALUE;
    }

    fscanf(f,"%d",&count);
    fscanf(f,"%d",&readIndex);
    double* coords;
    SPPoint* featsarr = (SPPoint*)malloc(sizeof(SPPoint)*count);
     if(coords == NULL){
            PDEBUG("malloc failed.")
            fclose(f);
            return ERROR_VALUE;
        }
    for(int i=0;i<count;i++){
        coords = (double*)malloc(sizeof(double)*dim);
        if(coords == NULL){
                PDEBUG("malloc failed.")
            free(featsarr);
            fclose(f);
            return ERROR_VALUE;
        }
        for(int j=0;j<dim;j++){
            fscanf(f,"%lf",&(coords[j]));
        }
        featsarr[i] = spPointCreate(coords,dim,readIndex);
        if(featsarr[i] == NULL){
            PDEBUG("NULL SPPoint read from file");
        }
    }
    *pfeats = featsarr;

    fclose(f);
    PDEBUG("finish reading points from file");
    free(coords);
    return count;
}
