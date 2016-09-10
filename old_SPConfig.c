//
//  SPConfig.c
//  SpFinalX
//
//  Created by Shlomi Zabari on 8/19/16.
//  Copyright (c) 2016 Shlomi Zabari. All rights reserved.
//


#define PAR_NUM 14
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include "SPConfig.h"
#include "debug.h"


struct sp_config_t {
    int spNumOfImages;
    int spNumOfFeatures;
    int spPCADimension;
    int spNumOfSimilarImages;
    int spKNN;
    int spLoggerLevel;

    bool spExtractionMode;
    bool spMinimalGUI;

    char* spImagesDirectory;
    char* spImagesPrefix;
    char* spImagesSuffix;
    char* spPcaFilename;
    char* spLoggerFilename;

    splitMethod spKDTreeSplitMethod;
};

char** str_split(char* a_str, const char a_delim);
void removeWhiteSpaces(char* str);

/* Creates a new system configuration struct. The configuration struct
 * is initialized based on the configuration file given by 'filename'.*/
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){
    char*  validVarsNames[PAR_NUM] = {"spImagesDirectory", "spImagesPrefix", "spImagesSuffix", "spNumOfImages",                     "spPCADimension","spPcaFilename", "spNumOfFeatures", "spExtractionMode", "spNumOfSimilarImages", "spKDTreeSplitMethod","spKNN" ,"spMinimalGUI","spLoggerLevel","spLoggerFilename"};
   // char*   varVal[PAR_NUM]   = {NULL, NULL, NULL, NULL, "20","pca.yml", "100", "true", "false", "1", "1", "MAX_SPREAD", "3", "stdout"          };
    int     varValInt[PAR_NUM]= {-1,-1,-1,-1,20,-1,100,1,0,1,1,-1,3,-1};
   // char*   imgsuffix[4] = {".jpg",".png",".bmp",".gif"};
    char*   line;
    char**  splited_line;
    char*   tmp;
    char*   endptr;
    int     count_eq;
    int     index;
    int     str_flag = 0;
    long    int_val;
    int  size = 1024;
    FILE*   configFile;
    struct  stat fd;
    SPConfig spconfig = NULL;


    (void)varValInt;//TODO delete

    // checks if the file is regular
        if (stat(filename, &fd) <0){
            PDEBUG("[ERROR][CONFIG] <spConfigCreate> failure while trying to access filename\n");
            *msg = SP_CONFIG_INVALID_ARGUMENT;
            return NULL;
        }

    if (!S_ISREG(fd.st_mode)){
        PDEBUG("[ERROR][CONFIG] <spConfigCreate> filename is not a regular file\n");
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return NULL;
    }

    // open the files for read
    if ((configFile = fopen(filename, "rb")) == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigCreate> open copy source path failure\n");
        *msg = SP_CONFIG_CANNOT_OPEN_FILE;
        return NULL;
    }

    // allocating memory for buffer reading
    if ((line = (char*) malloc(size*sizeof(char))) == NULL) {
        PDEBUG("[ERROR][CONFIG] <spConfigCreate> memory allocation failed\n");
        *msg = SP_CONFIG_ALLOC_FAIL;
        return NULL;
    }

    // reading all file
    while (fgets(line, size, configFile) != NULL) { //TODO: check usage of "getline".

        str_flag = 0;
        errno = 0;

        // ignore leading whitespace

        // Trim leading space
        while(isspace(*line)) line++;

        if(*line == 0)  // All spaces? go to the next line
            continue;

        if (*line == '#') // A comment line? go to the next line
            continue;

        // deals with '=' delimiter
        tmp = line;

        // count how many times '=' occurs
        while(*tmp) {
            if (*tmp == '=')
                count_eq++;
            if (count_eq >1) {
                PDEBUG("[ERROR][CONFIG] <spConfigCreate> more than one '=' failure\n");
                *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                return NULL;
            }
            tmp++;
        }

        // Split with respect to '='  - check if legal
        if ((splited_line = (char**) str_split(line, '=')) == NULL) {
            PDEBUG("[ERROR][CONFIG] <spConfigCreate> split line with respect to '=' failure\n");
            *msg = SP_CONFIG_INVALID_CONFIG_LINE;
            return NULL;
        }

        // sanity check should have two parts only
        if ((sizeof(splited_line)/sizeof(char*)) > 2) {
            PDEBUG("[ERROR][CONFIG] <spConfigCreate> more than one '=' where found (sanity check failure)\n");
            *msg = SP_CONFIG_INVALID_CONFIG_LINE;
            return NULL;
        }

        // remove all whitespaces from lines
        for (int i = 0 ; i<2 ; i++)
            removeWhiteSpaces(splited_line[i]);

        // find the right parameter (if legal)
        for(index=0; index<PAR_NUM; index++) {
            if(strcmp(*splited_line, validVarsNames[index]))
                continue;
        }

        // check for type


        // TODO : add check type - assign str_flag to 0 or 1


        if (str_flag) { // the var should be string


            // TODO : add case switch for all string values parameters and their constraints


        }
        else {

            // Extracting integer from the string and dealing errors

            int_val = strtol(splited_line[1], &endptr, 10);
            if (errno != 0) {
                // conversion failed (EINVAL, ERANGE)
                if(errno == EINVAL){
                    PDEBUG("[ERROR][CONFIG] <spConfigCreate> conversion failed, not an integer\n");
                }
                else if(errno == ERANGE) {
                    PDEBUG("[ERROR][CONFIG] <spConfigCreate> conversion failed, out of range\n");
                }
                else
                    PDEBUG("[ERROR][CONFIG] <spConfigCreate> unkown error occured\n");
                *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                return NULL;
            }

            if (splited_line[1] == endptr) {
                // conversion failed (no characters consumed)
                PDEBUG("[ERROR][CONFIG] <spConfigCreate> conversion failed (no characters consumed)\n");
                *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                return NULL;
            }
            if (*endptr != 0) {
                // conversion failed (trailing data)
                PDEBUG("[ERROR][CONFIG] <spConfigCreate> conversion failed (trailing data) - none integer charecters\n");
                *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                return NULL;
            }

            // extraction done!

            switch (index) {
                case 4:
                {
                    if (int_val<=0) {
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spNUMOfImages should be positive int\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        return NULL;
                    }
                    varValInt[index] = (int) int_val;
                    break;
                }
                case 5:
                {
                    if (int_val<10 || int_val>28) {
                        // constraints - 10<= int_val <= 28
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spPCADimensions should be in rang 10 to 28\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        return NULL;
                    }
                    varValInt[index] = (int) int_val;
                    break;
                }
                case 7:
                {
                    if (int_val<=0) {
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spNUMOfFeatures should be positive int\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        return NULL;
                    }
                    varValInt[index] = (int) int_val;
                    break;
                }
                case 9: {
                    if (int_val<=0) {
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spNUMOfSimilarImigaes should be positive int\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        return NULL;
                    }
                    varValInt[index] = (int) int_val;
                    break;
                }
                case 11:
                {
                    if (int_val<=0) {
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spKNN should be positive int\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        return NULL;
                    }
                    varValInt[index] = (int) int_val;
                    break;

                }
                case 13:
                {
                    if (int_val<1 || int_val>4) {
                        // constraints - 1<= int_val <= 4
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spLoggerLevel should be in rang 10 to 28\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        return NULL;
                    }
                    varValInt[index] = (int) int_val;
                    break;
                }
                default:
                {
                    PDEBUG("[ERROR][CONFIG] <spConfigCreate> PROGRAMMING ERROR - unknown index - if got here, check if none int (string or bool) parameter defined by mistake as integer value processing\n");
                    *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                    return NULL;
                }
                    break;
            }

        }
    }

    // Dealling integer parameters - done

    // checking all none defaultive parameters where assigned
    /*if (check_default_values() == -1){

        // TODO : implement check_default_values()


        PDEBUG("[ERROR][CONFIG] <spConfigCreate> a parameter with no default value wasn't set\n");
        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
        return NULL;
    }
    */
    // TODO - set all values to the configStruct

    PDEBUG("[CONFIG] <spConfigCreate> config done!\n");
    return spconfig;
}

/* Returns true if spExtractionMode = true, false otherwise. */
bool spConfigIsExtractionMode(const SPConfig config, SP_CONFIG_MSG* msg) {

    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }

    *msg = SP_CONFIG_SUCCESS;

    return config->spExtractionMode;
}



/* Returns the number of images set in the configuration file, i.e the value
 * of spNumOfImages. */
int spConfigGetNumOfImages(const SPConfig config, SP_CONFIG_MSG* msg) {

    if (config == NULL) {
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }

    *msg = SP_CONFIG_SUCCESS;

    return config->spNumOfImages;
}


/* Returns the number of features to be extracted. i.e the value
 * of spNumOfFeatures. */
int spConfigGetNumOfFeatures(const SPConfig config, SP_CONFIG_MSG* msg) {

    if (config == NULL) {
        PDEBUG("[CONFIG] <spConfigGetNumOfFeatures> config null pointer");
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }

    *msg = SP_CONFIG_SUCCESS;

    return config->spNumOfFeatures;

}


/* Returns the dimension of the PCA. i.e the value of spPCADimension.*/
int spConfigGetPCADim(const SPConfig config, SP_CONFIG_MSG* msg) {

    if (config == NULL) {
        PDEBUG("[CONFIG] <spConfigGetPCADim> config null pointer");
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return false;
    }

    *msg = SP_CONFIG_SUCCESS;

    return config->spPCADimension;
}

SP_CONFIG_MSG spConfigGetFeatFilePath(char fullPath[], SPConfig config, int index, char* suffix){
    if(config == NULL){
        PDEBUG("null pointer was sent as parameter 'config");
        return SP_CONFIG_SUCCESS; //TODO: fix;
    }
    if(index >= config->spNumOfImages){
        PDEBUG("invalid index")
        return SP_CONFIG_SUCCESS; //TODO: fix;
    }
    sprintf(fullPath,"%s%s%d%s",config->spImagesDirectory,config->spImagesPrefix,index,suffix);
     return SP_CONFIG_SUCCESS;
}

/* Given an index 'index' the function stores in imagePath the full path of the
 * ith image.*/
SP_CONFIG_MSG spConfigGetImagePath(char* imagePath, const SPConfig config, int index) {
    if(config == NULL){
        PDEBUG("null pointer was sent as parameter 'config");
        return SP_CONFIG_SUCCESS; //TODO: fix
    }
    if(index >= config->spNumOfImages){
        PDEBUG("invalid index")
        return SP_CONFIG_SUCCESS; //TODO: fix
    }
    sprintf(imagePath,"%s%s%d%s",config->spImagesDirectory,config->spImagesPrefix,index,config->spImagesSuffix);
    return SP_CONFIG_SUCCESS;
}





SP_CONFIG_MSG __spConfigGetImagePath(char* imagePath, const SPConfig config,
                                   int index) { //TODO: delete at the end

    char buff[1024];
    char* srcPath;
    char indexS[1024];
    unsigned int long pathSize;
    struct stat fd;
    size_t size;
    FILE* src;
    FILE* dst;

    if (config == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> config null pointer\n");
        return SP_CONFIG_INVALID_ARGUMENT;
    }

    if (index < 0 || (index >= config->spNumOfImages)) {
        PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> index out of range\n");
        return SP_CONFIG_INDEX_OUT_OF_RANGE;
    }

    if (imagePath == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> imagePath null pointer\n");
        return SP_CONFIG_INVALID_ARGUMENT;
    }

    // TODO : maybe add restriction on size - free space check

    if (config->spImagesDirectory == NULL || config->spImagesPrefix == NULL || config->spImagesSuffix == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> image dir / prefix / sufix null pointer\n");
        return SP_CONFIG_INVALID_ARGUMENT;
    }

    // checks if the image path exists
    _D printf("DEBUG: imPath: [%s]\n",imagePath);
    if (stat(imagePath, &fd) <0){
        PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> failure while trying to access imPath\n");
        return SP_CONFIG_MISSING_DIR;
    }

    if (!S_ISDIR(fd.st_mode)){
        PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> imPath is nor directory\n");
        return SP_CONFIG_MISSING_DIR;
    }

    // allocating memory for destination path
    pathSize = strlen(config->spImagesDirectory)+strlen(config->spImagesPrefix)+strlen(config->spImagesSuffix)+20;
    if ((srcPath = (char*) malloc(pathSize*sizeof(char))) == NULL) {
        PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> memory allocation failed\n");
        return SP_CONFIG_ALLOC_FAIL;
    }

    // creating the dest. path string
    sprintf(indexS, "%d",index);

    strcat(srcPath, config->spImagesDirectory);
    strcat(srcPath, "/");
    strcat(srcPath, config->spImagesPrefix);
    strcat(srcPath, indexS);
    strcat(srcPath, ".");
    strcat(srcPath, config->spImagesSuffix);

    // open the files for read /write
    if ((src = fopen(srcPath, "rb")) == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> open copy source path failure\n");
        return SP_CONFIG_CANNOT_OPEN_FILE;
    }

    if ((dst = fopen(imagePath, "wb")) == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> open copy destination path failure\n");
        return SP_CONFIG_CANNOT_OPEN_FILE;
    }

    // copy files
    while ((size = fread(buff, 1, 1024, src))){
        if (fwrite(buff,1,1024,dst) !=1024){
            PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> copying files failure - write less than 1024 bytes\n");
            return SP_CONFIG_CANNOT_COPY_FILE;
        }
    }

    fclose(dst);
    fclose(src);
    free(srcPath);

    return SP_CONFIG_SUCCESS;

}


/* The function stores in pcaPath the full path of the pca file. */

SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {
     if(config == NULL){
        PDEBUG("null pointer was sent as parameter 'config");
        return SP_CONFIG_SUCCESS; //TODO: fix
    }
     _D printf("DEBUG: pcaPath: [%s]\n",config->spPcaFilename);
    sprintf(pcaPath,"%s%s",config->spImagesDirectory,config->spPcaFilename);
    _D printf("DEBUG: pcaPath: [%s]\n",pcaPath);
    return SP_CONFIG_SUCCESS;
}

/*
SP_CONFIG_MSG spConfigGetPCAPath(char* pcaPath, const SPConfig config) {

    char buff[1024];
    char* srcPath;
    unsigned int long pathSize;
    struct stat fd;
    size_t size;
    FILE* src;
    FILE* dst;

    if (config == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> config null pointer\n");
        return SP_CONFIG_INVALID_ARGUMENT;
    }


    if (pcaPath == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> imagePath null pointer\n");
        return SP_CONFIG_INVALID_ARGUMENT;
    }

    // TODO : maybe add restriction on size - free space check

    if (config->spImagesDirectory == NULL || config->spImagesPrefix == NULL || config->spImagesSuffix == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> image dir / prefix / sufix null pointer\n");
        return SP_CONFIG_INVALID_ARGUMENT;
    }

    // checks if the image path exists
    if (stat(pcaPath, &fd) <0){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> failure while trying to access imPath\n");
        return SP_CONFIG_MISSING_DIR;
    }

    if (!S_ISDIR(fd.st_mode)){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> imPath is nor directory\n");
        return SP_CONFIG_MISSING_DIR;
    }

    // allocating memory for destination path
    pathSize = strlen(config->spImagesDirectory)+strlen(config->spImagesPrefix)+strlen(config->spImagesSuffix)+20;
    if ((srcPath = (char*) malloc(pathSize*sizeof(char))) == NULL) {
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> memory allocation failed\n");
        return SP_CONFIG_ALLOC_FAIL;
    }

    // creating the dest. path string
    strcat(srcPath, config->spImagesDirectory);
    strcat(srcPath, "/");
    strcat(srcPath, config->spPcaFilename);

    // open the files for read /write
    if ((src = fopen(srcPath, "rb")) == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> open copy source path failure\n");
        return SP_CONFIG_CANNOT_OPEN_FILE;
    }

    if ((dst = fopen(pcaPath, "wb")) == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetImagePath> open copy destination path failure\n");
        return SP_CONFIG_CANNOT_OPEN_FILE;
    }

    // copy files
    while ((size = fread(buff, 1, 1024, src))){
        if (fwrite(buff,1,1024,dst) !=1024){
            PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> copying files failure - write less than 1024 bytes\n");
            return SP_CONFIG_CANNOT_COPY_FILE;
        }
    }

    fclose(dst);
    fclose(src);
    free(srcPath);

    return SP_CONFIG_SUCCESS;
}
*/

/* Returns the KDTree split method - RANDOM, MAX_SPREAD, INCREMENTAL, i.e
 * the spKDTreeSplitMethod variable.*/
splitMethod spConfigGetSplitMethod(SPConfig config,  SP_CONFIG_MSG* msg) {

    if (config == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> config null pointer\n");
        *msg = SP_CONFIG_INVALID_ARGUMENT;
        return RANDOM;
    }

    *msg = SP_CONFIG_SUCCESS;

    return config->spKDTreeSplitMethod;
}


/* The function stores in prefix string the image prefix as extracted from the configuration file. */
SP_CONFIG_MSG spConfigGetImagesPrefix(char* prefix, SPConfig config) {

    prefix = NULL;

    if (config == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> config null pointer\n");
        return SP_CONFIG_INVALID_ARGUMENT;
    }

    if (config->spImagesPrefix == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> images prefix is not configured\n");
        return SP_CONFIG_MISSING_PREFIX;
    }

    prefix = config->spImagesPrefix;

    return SP_CONFIG_SUCCESS;
}


/* The function stores in suffix string the image suffix as extracted from the configuration file. */
SP_CONFIG_MSG spConfigGetImagesSuffix(char* suffix, SPConfig config) {

    suffix = NULL;

    if (config == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> config null pointer\n");
        return SP_CONFIG_INVALID_ARGUMENT;
    }

    if (config->spImagesPrefix == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> images suffix is not configured\n");
        return SP_CONFIG_MISSING_SUFFIX;
    }

    suffix = config->spImagesSuffix;

    return SP_CONFIG_SUCCESS;
}


/* Frees all memory resources associate with config. */
void spConfigDestroy(SPConfig config) {

    if (config == NULL) {
        PDEBUG("[ERROR][CONFIG] <spConfigGetPCAPath> config null pointer\n");
        return;
    }

    free(config->spImagesDirectory);
    free(config->spImagesPrefix);
    free(config->spImagesSuffix);
    free(config->spPcaFilename);
    free(config->spLoggerFilename);
    free(config);

}



/* Splits string with respect to delimiter -
 * On a given string with delimiter the function will split the string into dynamic
 * sring array. The function can handle concequentive delimiter string.
 * The function cannot handles constant string argument.
 * The function will change the string argument, in case the string is important for later use
 * please make sure you duplicate it first.
 * Please notice that the returned array is DYNAMICALLY allocated and should handle as such.
 *
 * ****************************************************************************************
 * ***  Credit to hmjd's comment on - (base for modification)                           ***
 * ***     http://stackoverflow.com/questions/9210528/split-string-with-delimiters-in-c ***
 * ****************************************************************************************
 *
 * @param str - string for split
 * @assert (str != NULL & str is not const)
 * @param a_delim - const char type
 * @assert (a_delim != NULL)
 * @return
 * String dynamic array - str splited with respect to a_delimiter
 */
char** str_split(char* a_str, const char a_delim)
{
    char**  result      = 0;
    size_t  count       = 0;
    char*   tmp         = a_str;
    char*   last_comma  = 0;
    char    delim[2];

    delim[0]    = a_delim;
    delim[1]    = 0;

    /* Count how many elements will be extracted. */

    while (*tmp)
    {
        if (a_delim == *tmp)
        {
            if (tmp != a_str) // check the last comma addrs if not the beginning of the string
            {
                if ((last_comma - tmp) != 1) // in case it's not sequencal commas - don't count it
                    count++;
            } else // it's the first charecter of the string - don't check for last comma and count
                count++;
            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    /* Add space for terminating null string so caller
     knows where the list of returned strings ends. */
    count++;

    result = malloc(sizeof(char*) * count);

    if (result)
    {
        size_t idx  = 0;
        char* token = strtok(a_str, delim);

        while (token)
        {
            if(idx < count)
            {
             PDEBUG("[ERROR][SPLIT] <str_split> more tokens than expected\n");
                return NULL;
            }
            strcpy(*(result + idx++),token);
            token = strtok(NULL, delim);
        }
        if(idx == count - 1)
        {
            PDEBUG("[ERROR][SPLIT] <str_split> one more tokens than expected\n");
            return NULL;
        }
        *(result + idx) = 0;
    }

    return result;
}


/* The function remove all whitespaces from string - the original string is being modified -
 * the sring shouldn't be const or read-only.
 *
 * ****************************************************************************************
 * ***  Credit to Aaron's comment on - (base for modification)                         ***
 * ***    http://stackoverflow.com/questions/1726302/removing-spaces-from-a-string-in-c ***
 * ****************************************************************************************
 *
 * @param str - string to whitespaces to be deleted from
 * @assert (str != NULL & str is not const (not read-only)
 * @return
 */
void removeWhiteSpaces(char* str)
{
    char* i = str;
    char* j = str;
    while(*j != 0)
    {
        *i = *j++;
        if(isspace(*j))
            i++;
    }
    *i = 0;
}


SPConfig spConfigCreateManually(SP_CONFIG_MSG* msg){
    SPConfig result;

    if ((result = (SPConfig) malloc(sizeof(*result))) == NULL)
    {
        PDEBUG("ERROR : Allocation failed\n");
        return NULL;
    }

    result->spImagesDirectory   = "D:\\Documents\\Dropbox\\TAU\\Software_Project\\HW\\Final_as_cpp\\images\\";
    result->spImagesPrefix      = "img";
    result->spImagesSuffix      = ".png";
    result->spNumOfImages       = 15;
    result->spNumOfFeatures     = 14;
    result->spExtractionMode    = true;
    result->spNumOfSimilarImages= 4;
    result->spKDTreeSplitMethod = RANDOM;
    result->spKNN               = 4;
    result->spMinimalGUI        = false;
    result->spLoggerLevel       = 1;
    result->spLoggerFilename    = "log.log";
    result->spPcaFilename       = "pca.yml";
    result->spPCADimension      = 5;
    *msg = SP_CONFIG_SUCCESS;

    return result;
}

int spConfigGetSPKNN(const SPConfig config, SP_CONFIG_MSG* msg){
	PDEBUG(">");
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spKNN;
}

int spConfigGetNumOfSimilarImages(const SPConfig config, SP_CONFIG_MSG* msg){
	PDEBUG(">");
	if (config == NULL) {
		*msg = SP_CONFIG_INVALID_ARGUMENT;
		return false;
	}
	*msg = SP_CONFIG_SUCCESS;
	return config->spNumOfSimilarImages;
}

bool spConfigMinimalGui(const SPConfig config, SP_CONFIG_MSG* msg){
	if (config == NULL) {
			*msg = SP_CONFIG_INVALID_ARGUMENT;
			return false;
		}
		*msg = SP_CONFIG_SUCCESS;
		return config->spMinimalGUI;
}

