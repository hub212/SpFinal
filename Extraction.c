/*
 * Extraction.c
 *
 *  Created on: 31 áàåâ× 2016
 *      Author: user
 */
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <errno.h>
#include <stdlib.h>


FILE filenameHandler(SPPoint config, int index) {
    
    FILE    dest;
    char*   buff;
    int     strSize;
    
    
    // The feature string size  - directory + prefix + index + ".feats" + null terminator
    strSize = strlen(config->spImagesDirectory) + strlen(config->spImagesPrefix) + (int) ceil(log(config->spNumOfImages)/log(2)) + 7;
    
    // filename allocation
    if ((buff = (char*) malloc(sizeof(char)*strSize)) == NULL) {
        // add debug error message
        return NULL;
    }
    
    // copying string to buffer
    if (sprintf(buff, "%s%s%0d.feats",config->spImagesDirectory,config->spImagesPrefix,index) < 0) {
        // add error meassage
        return NULL;
    }
    
    // open new file to write and read
    if((dest = fopen(buff,'r+')) == NULL) {
        // add debiug error message
        return NULL;
    }
    
    free(buff);
    
    return dest;
}


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


    char*   buff;
    FILE    dest;
    
    int     strSize;
    int     intgSize;
    int     remdrSize = 10;
    int     fd;
    int     index;
    
    
    for (int featureIndex = 0; featureIndex < count ; featureIndex++) {
        
        // the features picture index
        index = feats[featureIndex]->index;
    
        if ((dest = filenameHandler(config, index)) == NULL) {
            // add debug message
            return -1;
        }
    
        
        // index writing
        
        
        // index string size include null terminator and new line - \t + index + \n + \0
        strSize = ceil(log(index)/log(2)) + 3;
        
        // Allocating memory for index string
        if ((buff = (char*) malloc(sizeof(char)*strSize)) == NULL) {
            // add debug error message
            return -1;
        }
        
        // copying string to buffer
        if (sprintf(buff, "\t%0d\n", index) < 0) {
            // add error meassage
            return -1;
        }
        
        // writing index line
        if (fputs(buff, dest) ==  EOF) {
            // add error debug message
            return -1;
        }
        
        
        // writing p_coor parameter - loosing precision 10 digits after the point
        // p_coor string size  ~ integral part + reminder part (integral_part.remainder_part)
        
        
        // integral part size
        
        intgSize = (int) ceil(log((int) floor(feats[index]->p_coor))/log(2));
        
        // string size - \t + integral + '.' + reminder +\n + \0
        strSize = intgSize + remdrSize + 4;
        
        // Allocating buffer
        if ((buff = (char*) realloc(buff, sizeof(char)*strSize)) == NULL) {
            // adding error message
            return -1;
        }
        
        // Copying floating point into string
        if(sprintf(buff, "\t%.10f\n",feats[index]->p_coor) < 0) {
            // add error message
            return -1;
        }
        
        // writing p_coor line
        if (fputs(buff, dest) ==  EOF) {
            // add error debug message
            return -1;
        }

        
        // writing dim parameter
        
        strSize = (int) ceil(log(feats[index]->dim)/log(2)) + 3;
        
        // Allocating buffer
        if ((buff = (char*) realloc(buff, sizeof(char)*strSize)) == NULL) {
            // adding error message
            return -1;
        }
        
        // copying dim into string
        if(sprintf(buff, "\t%0d\n",feats[index]->dim) < 0) {
            // add error message
            return -1;
        }
        
        // writing dim line
        if (fputs(buff, dest) ==  EOF) {
            // add error debug message
            return -1;
        }
        
        close(dest);
        
    }
    
    free(buff);
}



/*
    Get image features from its corresponding '.feats' file

    @param config - configuration file
    @param feats - the index of the image we want to get the features of
    @param pfeats - the feature array will be stored here.

    @return the number of features
*/
int getExtractedFeatures(SPConfig config, int indexOfImage ,SPPoint** pfeats) {

    FILE    featsFile;
    char*   line = NULL;
    char*   endptr;
    size_t  size = 1024;
    int     line_num = 0;
    int     tmp_int;
    
    
    

    if ((featsFile = filenameHandler(config, index)) == NULL) {
        // add debug message
        return -1;
    }
    
    
    // add foreach line extract the value - checking for unmatches types and format
    
    // reading all file - assuming features files are formmated
    while (getline(&line, &size, featsFile) != -1) {
        // intializing errno
        errno = 0;
        
        // add debug message telling which line extracted
        
        if (line_num == 0 || line_num == 1) {
            // converting to integer
            tmp_int = strtol(line, &endptr, 10);
        } else if (line_num == 2) {
            // converting to float
            tmp_double = strtof(line, &endptr);
        }
        else {
            // add error message
            return -1;
        }
        
        // trying to catch errors
        if (errno != 0) {
            // conversion failed (EINVAL, ERANGE)
            if(errno == EINVAL){
                PDEBUG("[ERROR][EXTRCACTION MODE] <getExtractedFeatures> conversion failed, not an integer\n");
            }
            else if(errno == ERANGE) {
                PDEBUG("[ERROR][EXTRCACTION MODE] <getExtractedFeatures> conversion failed, out of range\n");
            }
            else
                PDEBUG("[ERROR][EXTRCACTION MODE] <getExtractedFeatures> unkown error occured\n");
            // free all
            return -1;
        }
        
        if (line == endptr) {
            // conversion failed (no characters consumed)
            PDEBUG("[ERROR][EXTRCACTION MODE] <getExtractedFeatures> conversion failed (no characters consumed)\n");
            return -1;
        }
        if (*endptr != 0) {
            // conversion failed (trailing data)
            PDEBUG("[ERROR][CONFIG] <spConfigCreate> conversion failed (trailing data) - none integer charecters\n");
            return -1;
        }
        
        switch (line_num) {
            case 0 : // index line
            {
                *pfeats[indexOfImage]->index = tmp_int;
                break;
            }
            case 1 : // dim line
            {
                *pfeats[indexOfImage]->dim = tmp_int;
                break;
            }
            case 2 : // p_coor line
            {
                *pfeats[indexOfImage]->p_coor = tmp_double;
                break;
            }
        }
    }
    
    free(line);
    close(featsFile);
    
    return 0;
}
