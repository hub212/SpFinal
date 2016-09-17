//
//  SPConfig.c
//  SpFinalX
//
//  Created by Shlomi Zabari on 8/19/16.
//  Copyright (c) 2016 Shlomi Zabari. All rights reserved.
//
#define PAR_NUM 14
#define NUM_SUFF 4

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <assert.h>
#include <ctype.h>
#include <errno.h>
#include <math.h>
#include "SPConfig.h"
#include "Debug.h"


// functions decleration
char** str_split(char* a_str, const char a_delim, int* len);
void removeWhiteSpaces(char** src);
void initializing_SPConfig(SPConfig spconfig);
void free_splited(char** strArr, int len);
void free_spconfig(SPConfig spconfig);
void remove_newLine(char* str);
int check_default_params(SPConfig config, const char* filename, int line_num, SP_CONFIG_MSG *msg);

/* Creates a new system configuration struct. The configuration struct
 * is initialized based on the configuration file given by 'filename'.*/
SPConfig spConfigCreate(const char* filename, SP_CONFIG_MSG* msg){

    // validVarsNames - holds valid variables names
    char*  validVarsNames[PAR_NUM]  = {"spImagesDirectory", "spImagesPrefix", "spImagesSuffix", "spNumOfImages",                     "spPCADimension","spPCAFilename", "spNumOfFeatures", "spExtractionMode", "spNumOfSimilarImages", "spKDTreeSplitMethod","spKNN"         ,"spMinimalGUI","spLoggerLevel","spLoggerFilename"};

    // strIndc - holds indecators for each parameter telling if it's an str or not(enum and bool considered as strings)
    int     strIndc[PAR_NUM]     = {1,1,1,0,0,1,0,1 ,0,1,0,1,0,1};

    char*   suffArr[NUM_SUFF] = {".jpg",".png",".bmp",".gif"};
    char*   line;
    char*   line_cpy;
    char**  splited_line;
    char*   valueStrCpy;
    char**  mid_white;
    char*   tmp;
    char*   endptr;

    int     count_eq = 0;
    int     index;
    int     suffix_int;
    int     line_num = 0;
    int     str_flag = 0;
    int     no_value = 0;
    int     len = 0;
    int     mw_len;

    long    int_val = 0;

    size_t  size = 1024;
    FILE*   configFile;
    
    SPConfig spconfig;


    // open the files for read
    if ((configFile = fopen(filename, "rb")) == NULL){
        PDEBUG("[ERROR][CONFIG] <spConfigCreate> open copy source path failure\n");
        printf("[ERROR] %s filename can't be opened\n", filename);
        *msg = SP_CONFIG_CANNOT_OPEN_FILE;
        return NULL;
    }

    // allocating memory for buffer reading
    if ((line = (char*) malloc(size*sizeof(char))) == NULL) {
        PDEBUG("[ERROR][CONFIG] <spConfigCreate> intial read line memory allocation failed\n");
        *msg = SP_CONFIG_ALLOC_FAIL;
        // free all
        fclose(configFile);
        return NULL;
    }

    // allocating memoryfor SPconfig struct
    if ((spconfig = (SPConfig) malloc(sizeof(spconfig)))== NULL) {
        PDEBUG("[ERROR][CONFIG] <spConfigCreate> SPConfig struct memory allocation failed\n");
        *msg = SP_CONFIG_ALLOC_FAIL;
        // free all
        fclose(configFile);
        free(line);
        return NULL;
    }

    // initializing spconfig
    initializing_SPConfig(spconfig);

    // reseting default values

    spconfig->spPCADimension = 20;
    spconfig->spPcaFilename = "pca.yml";
    spconfig->spNumOfFeatures = 100;
    spconfig->spExtractionMode = true;
    spconfig->spMinimalGUI = false;
    spconfig->spNumOfSimilarImages = 1;
    spconfig->spKNN = 1;
    spconfig->spKDTreeSplitMethod = MAX_SPREAD;
    spconfig->spLoggerLevel = 3;
    spconfig->spLoggerFilename = "stdout";

    // TODO - free

    // reading all file
    while ((len = (int) getline(&line, &size, configFile))!= -1) {
        
        PDEBUG("[CONFIG] new line: -----------------------");
        printf("%s\n\n",line);
        
        if (line == NULL) {
            PDEBUG("[CONFIG] error was occure while trying to get new line");
            *msg = SP_CONFIG_ALLOC_FAIL;
            return NULL;
        }
        
        line_num++;
        str_flag = 0;
        count_eq = 0;
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
                PDEBUG("[ERROR][CONFIG] <spConfigCreate> more than one '=' failure, line:")
                PDEBUG(line);
                printf("File: %s\nLine: %0d\nMessage: Invalid configuration line\n",filename,line_num);
                *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                // free all
                fclose(configFile);
                free(line);
                free_spconfig(spconfig);
                return NULL;
            }
            tmp++;
        }

        // making a line copy for debug - first allocting pepory
        if((line_cpy = (char*) malloc(sizeof(char)*(len+2))) == NULL) {
            PDEBUG("[ERROR][CONFIG] <spConfigCreate> original line copy memory allocation failed");
            *msg = SP_CONFIG_ALLOC_FAIL;
            // free all
            fclose(configFile);
            free(line);
            free_spconfig(spconfig);
            return NULL;
        }

        // then copying
        strcpy(line_cpy,line);

        // Split with respect to '='  - check if legal (line no longer contains the current line - only the copy)
        if ((splited_line = (char**) str_split(line, '=', &len)) == NULL) {
            PDEBUG("[ERROR][CONFIG] <spConfigCreate> couldn't split line with respect to '=' , line:");
            PDEBUG(line_cpy);
            // free all
            fclose(configFile);
            free(line);
            free_spconfig(spconfig);
            free(line_cpy);
            *msg = SP_CONFIG_INVALID_CONFIG_LINE;
            return NULL;
        }
        
        
        // sanity check should have two parts only
        if (len > 2) {
            PDEBUG("[ERROR][CONFIG] <spConfigCreate> more than one '=' where found (sanity check failure - Programming error), line:");
            PDEBUG(line_cpy);
            PDEBUG("splited line:");
            for (int i = 0 ; i< len ; i++) {
                PDEBUG(splited_line[i]);
            }
            printf("File: %s\nLine: %0d\nMessage: Invalid configuration line\n",filename,line_num);
            *msg = SP_CONFIG_INVALID_CONFIG_LINE;
            // free all
            fclose(configFile);
            free(line);
            free_spconfig(spconfig);
            free(line_cpy);
            free_splited(splited_line, len);
            return NULL;
        }

        // check for mid white spaces in the value string - uses the split line with " " delemiter
        if ((valueStrCpy = (char*) malloc(sizeof(char)*strlen(splited_line[1]+1)))== NULL) {
            PDEBUG("[ERROR][CONFIG] <spConfigCreate> value copy (splited_line[1]) memory allocation failed");
            *msg = SP_CONFIG_ALLOC_FAIL;
            // free all
            fclose(configFile);
            free(line);
            free_spconfig(spconfig);
            free(line_cpy);
            free_splited(splited_line, len);
            return NULL;
        }
        strcpy(valueStrCpy, splited_line[1]);
        mid_white = str_split(valueStrCpy, ' ', &mw_len);
        if (mw_len != 1) { // whiteSpace in the mid of the word
            PDEBUG("[ERROR][CONFIG] <spConfigCreate> whitespace in the middle of the value string, line:");
            PDEBUG(line_cpy);
            PDEBUG("splited line:");
            for (int i = 0 ; i< len ; i++) {
                PDEBUG(*(splited_line+i));
            }
            printf("File: %s\nLine: %0d\nMessage: Invalid value - constraint not met\n",filename,line_num);
            *msg = SP_CONFIG_INVALID_CONFIG_LINE;
            printf("File: %s\nLine: %0d\nMessage: Invalid configuration line\n",filename,line_num);
            *msg = SP_CONFIG_INVALID_CONFIG_LINE;
            // free all
            fclose(configFile);
            free(line);
            free_spconfig(spconfig);
            free(line_cpy);
            free_splited(splited_line, len);
            free_splited(mid_white, mw_len);
            free(valueStrCpy);
            return NULL;
        }

        // free value whitspace helpers allocation
        free_splited(mid_white, mw_len);
        free(valueStrCpy);


        PDEBUG("[CONFIG] <spConfigCreate> removing white sapces from lines:");
        // remove all whitespaces from line
        for (int i = 0 ; i<2 ; i++) {
            removeWhiteSpaces(&splited_line[i]);
        }

        PDEBUG("[CONFIG] finding parameter:");
        // find the right parameter (if legal)
        for(index=0; index<PAR_NUM; index++) {
            if(strcmp(*splited_line, validVarsNames[index]) == 0) {
                PDEBUG(validVarsNames[index]);
                break;
            }
        }

        // check for type
        if (index==PAR_NUM) {
            PDEBUG("[ERROR][CONFIG] <spConfigCreate> can't find parameter, line:");
            PDEBUG(line_cpy);
            PDEBUG("splited line:");
            for (int i = 0 ; i< len ; i++) {
                PDEBUG(*(splited_line+i));
            }
            printf("File: %s\nLine: %0d\nMessage: Invalid configuration line\n",filename,line_num);
            *msg = SP_CONFIG_INVALID_CONFIG_LINE;
            // free all
            fclose(configFile);
            free(line);
            free_spconfig(spconfig);
            free(line_cpy);
            free_splited(splited_line, len);
            return NULL;
        }

        // if we got here - the param is valid - checking if value exists
        if (strcmp(splited_line[1],"") == 0)
            no_value = 1;

        // removing newline
        remove_newLine(splited_line[1]);
        // check type - assign str_flag to 0 or 1
                       
        str_flag = strIndc[index];

        if (str_flag) { // the var should be valid string type parameter

            switch(index) {
                case 0: // spImageDirectory - string with no spaces (the spaces terminated before)
                {
                    if (no_value) {
                        printf("File: %s\nLine: %0d\nMessage: Parameter %s is not set\n",filename,line_num,splited_line[0]);
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spNUMOfFeatures - no value for none defaultive parameter");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }

                    // allocating memory for buffer reading
                    if ((spconfig->spImagesDirectory = (char*) malloc((strlen(splited_line[1])+2)*sizeof(char))) == NULL) {
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> intial read line memory allocation failed, line:");
                        PDEBUG(line_cpy);
                        PDEBUG("splited line:");
                        for (int i = 0 ; i<len ; i++) {
                            PDEBUG(*(splited_line+i));
                        }
                        *msg = SP_CONFIG_ALLOC_FAIL;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }

                    spconfig->spImagesDirectory = strcpy(spconfig->spImagesDirectory,splited_line[1]);
                    spconfig->assignArr[0] = 1; // marking allocation

                    break;
                }
                case 1: // spImagesPrefix - no spaces string
                {
                    if (no_value) {
                        printf("File: %s\nLine: %0d\nMessage: Parameter %s is not set\n",filename,line_num,splited_line[0]);
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spNUMOfFeatures - no value for none defaultive parameter");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }

                    // allocating memory for copy buffer
                    if ((spconfig->spImagesPrefix = (char*) malloc((strlen(splited_line[1])+2)*sizeof(char))) == NULL) {
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> intial read line memory allocation failed, line:");
                        PDEBUG(line_cpy);
                        PDEBUG("splited line:");
                        for (int i = 0 ; i<len ; i++) {
                            PDEBUG(*(splited_line+i));
                        }
                        *msg = SP_CONFIG_ALLOC_FAIL;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }

                    strcpy(spconfig->spImagesPrefix,splited_line[1]);
                    spconfig->assignArr[1] = 1; // marking allocation

                    break;
                }
                case 2: // spImagesSuffix - inside valid format
                {
                    if (no_value) {
                        printf("File: %s\nLine: %0d\nMessage: Parameter %s is not set\n",filename,line_num,splited_line[0]);
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spNUMOfFeatures - no value for none defaultive parameter");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }

                    for (suffix_int=0; suffix_int<NUM_SUFF ; suffix_int++ ) { // suffix loop
                        if (strcasecmp(suffArr[suffix_int],splited_line[1])==0) {
                            // allocating memory for copy buffer
                            if ((spconfig->spImagesSuffix = (char*) malloc((strlen(splited_line[1])+2)*sizeof(char))) == NULL) {
                                PDEBUG("[ERROR][CONFIG] <spConfigCreate> intial read line memory allocation failed, line:");
                                PDEBUG(line_cpy);
                                PDEBUG("splited line:");
                                for (int i = 0 ; i<len ; i++) {
                                    PDEBUG(*(splited_line+i));
                                }
                                *msg = SP_CONFIG_ALLOC_FAIL;
                                // free all
                                fclose(configFile);
                                free(line);
                                free_spconfig(spconfig);
                                free(line_cpy);
                                free_splited(splited_line, len);
                                return NULL;
                            }

                            strcpy(spconfig->spImagesSuffix,suffArr[suffix_int]);
                            spconfig->assignArr[2] = 1; // marking allocation

                            break;
                        }
                    } // end suffix loop

                    // if we got here with the last index it means we didnt match any of them
                    if (suffix_int == (NUM_SUFF)) { // invalid image suffix
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> invalid suufix type, line:");
                        PDEBUG(line_cpy);
                        PDEBUG("splited line:");
                        for (int j = 0 ; j<len; j++) {
                            PDEBUG(*(splited_line+j));
                        }
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }
                    break;
                }
                case 5: // spPCAfilename - no spaces string (trimed before)
                {


                    // allocating memory for copy buffer
                    if ((spconfig->spPcaFilename = (char*) malloc((strlen(splited_line[1])+2)*sizeof(char))) == NULL) {
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> intial read line memory allocation failed, line:");
                        PDEBUG(line_cpy);
                        PDEBUG("splited line:");
                        for (int i = 0 ; i<len ; i++) {
                            PDEBUG(*(splited_line+i));
                        }
                        *msg = SP_CONFIG_ALLOC_FAIL;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }

                    strcpy(spconfig->spPcaFilename ,splited_line[1]);
                    spconfig->assignArr[3] = 1; // marking allocation

                    break;
                }
                case 7: // spExtractionMode - boolean
                {
                    if(strcmp(splited_line[1],"true")==0) {
                        spconfig->spExtractionMode = true;
                    }
                    else if (strcmp(splited_line[1], "false") == 0) {
                        spconfig->spExtractionMode = false;
                    }
                    else {
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> invalid boolean value (extractionMode), line:");
                        PDEBUG(line_cpy);
                        PDEBUG("splited line:");
                        for (int i = 0 ; i<(int) (sizeof(splited_line)/sizeof(char*)); i++) {
                            PDEBUG(*(splited_line+i));
                        }
                        *msg = SP_CONFIG_ALLOC_FAIL;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }
                    break;
                }
                case 9 : // spKDTreeSplitMethod - enum - {RANDOM, MAX_SPREAD, INCREMENTAL}
                {
                    if (strcmp("RANDOM", splited_line[1]) == 0) {
                        spconfig->spKDTreeSplitMethod = RANDOM;
                    }
                    else if (strcmp("MAX_SPREAD", splited_line[1]) == 0) {
                        spconfig->spKDTreeSplitMethod = MAX_SPREAD;
                    }
                    else if (strcmp("INCREMENTAL", splited_line[1]) == 0) {
                        spconfig->spKDTreeSplitMethod = INCREMENTAL;
                    }
                    else { // none of the enum was found
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> invalid enum of value (KDTreeSplitMethod), line:");
                        PDEBUG(line_cpy);
                        PDEBUG("splited line:");
                        for (int i = 0 ; i<len ; i++) {
                            PDEBUG(*(splited_line+i));
                        }
                        *msg = SP_CONFIG_ALLOC_FAIL;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }
                    break;
                }
                case 11 : // spMinimalGUI - boolean
                {
                    if(strcmp(splited_line[1],"true")==0) {
                        spconfig->spMinimalGUI = true;
                    }
                    else if (strcmp(splited_line[1], "false") == 0) {
                        spconfig->spMinimalGUI = false;
                    }
                    else {
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> invalid boolean value (minimalGUI), line:");
                        PDEBUG(line_cpy);
                        PDEBUG("splited line:");
                        for (int i = 0 ; i<len ; i++) {
                            PDEBUG(*(splited_line+i));
                        }
                        *msg = SP_CONFIG_ALLOC_FAIL;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }
                    break;
                }
                case 13: // spLoggerFilename
                {
                    // allocating memory for buffer reading
                    if ((spconfig->spLoggerFilename = (char*) malloc((strlen(splited_line[1])+2)*sizeof(char))) == NULL) {
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> intial read line memory allocation failed , line:");
                        PDEBUG(line_cpy);
                        PDEBUG("splited line:");
                        for (int i = 0 ; i<len ; i++) {
                            PDEBUG(*(splited_line+i));
                        }
                        *msg = SP_CONFIG_ALLOC_FAIL;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }

                    strcpy(spconfig->spLoggerFilename,splited_line[1]);
                    spconfig->assignArr[4] = 1; // marking allocation

                    break;
                }
            }

        }
        else {

            // Extracting integer from the string and dealing errors
            if (!no_value) {
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
                    // free all
                    fclose(configFile);
                    free(line);
                    free_spconfig(spconfig);
                    free(line_cpy);
                    free_splited(splited_line, len);
                    return NULL;
                }

                if (splited_line[1] == endptr) {
                    // conversion failed (no characters consumed)
                    PDEBUG("[ERROR][CONFIG] <spConfigCreate> conversion failed (no characters consumed)\n");
                    *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                    // free all
                    fclose(configFile);
                    free(line);
                    free_spconfig(spconfig);
                    free(line_cpy);
                    free_splited(splited_line, len);
                    return NULL;
                }
                if (*endptr != 0) {
                    // conversion failed (trailing data)
                    PDEBUG("[ERROR][CONFIG] <spConfigCreate> conversion failed (trailing data) - none integer charecters\n");
                    *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                    // free all
                    fclose(configFile);
                    free(line);
                    free_spconfig(spconfig);
                    free(line_cpy);
                    free_splited(splited_line, len);
                    return NULL;
                }
            }

            // extraction done!


            switch (index) {
                case 3: // spNumOfImages
                {
                    if (no_value) {
                        printf("File: %s\nLine: %0d\nMessage: Parameter %s is not set\n",filename,line_num,splited_line[0]);
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spNUMOfFeatures - no value for none defaultive parameter");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }

                    if (int_val<=0) {
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spNUMOfImages should be positive int\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }
                    spconfig->spNumOfImages = (int) int_val;
                    break;
                }
                case 4: // spPCADimension
                {
                    if (no_value) // defaultive value - 20
                        break;
                    if (int_val<10 || int_val>28) {
                        // constraints - 10<= int_val <= 28
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spPCADimensions should be in rang 10 to 28\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }
                    spconfig->spPCADimension = (int) int_val;
                    break;
                }
                case 6: // spNumOfFeatures
                {
                    if (no_value) // defaultive value - 100
                        break;
                    if (int_val<=0) {
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spNUMOfFeatures should be positive int\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }
                    spconfig->spNumOfFeatures = (int) int_val;
                    break;
                }
                case 8: { // spNumOfSimilarImages
                    if (no_value) // defaultive value - 1
                        break;

                    if (int_val<=0) {
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spNUMOfSimilarImigaes should be positive int\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }
                    spconfig->spNumOfSimilarImages = (int) int_val;
                    break;
                }
                case 10: // spKNN
                {
                    if (no_value) // defaultive value - 1
                        break;

                    if (int_val<=0) {
                        // constraints - pos. int.
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spKNN should be positive int\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }
                    spconfig->spKNN = (int) int_val;
                    break;

                }
                case 12: // spLoggerLevel
                {
                    if (no_value) // defaultive value - 3
                        break;

                    if (int_val<1 || int_val>4) {
                        // constraints - 1<= int_val <= 4
                        PDEBUG("[ERROR][CONFIG] <spConfigCreate> spLoggerLevel should be in rang 10 to 28\n");
                        *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                        // free all
                        fclose(configFile);
                        free(line);
                        free_spconfig(spconfig);
                        free(line_cpy);
                        free_splited(splited_line, len);
                        return NULL;
                    }
                    spconfig->spLoggerLevel = (int) int_val;
                    break;
                }
                default:
                {
                    PDEBUG("[ERROR][CONFIG] <spConfigCreate> PROGRAMMING ERROR - unknown index - if got here, check if none int (string or bool) parameter defined by mistake as integer value processing\n");
                    *msg = SP_CONFIG_INVALID_CONFIG_LINE;
                    // free all
                    fclose(configFile);
                    free(line);
                    free_spconfig(spconfig);
                    free(line_cpy);
                    free_splited(splited_line, len);
                    return NULL;
                }
                    break;
            } // switch case

        } // integer params

        free(line_cpy); // freeing each line's copy
        free_splited(splited_line, len);
    } // getline loop

    
    free(line); // freeing line allocation

    // checking if none defaultive field missed
    if (check_default_params(spconfig, filename, line_num, msg) == -1){
        fclose(configFile);
        return NULL;
    }
        
    *msg = SP_CONFIG_SUCCESS;
    PDEBUG("[CONFIG] <spConfigCreate> config done!\n");

    fclose(configFile);
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
char** str_split(char* a_str, const char a_delim, int* len)
{
    size_t  count       = 0;
    size_t  idx         = 0;
    
    char*   token;
    char**  result      = 0;
    char*   tmp         = a_str;
    char*   last_comma  = 0;
    char    delim[2];

    delim[0]    = a_delim;
    delim[1]    = 0;

    /* Count how many elements will be extracted. */

    //PDEBUG("[SPLIT] <str_split> inside str_split\n");
    
    // counting delimiter and removing 'newLine'
    while (*tmp)
    {

        
        if (a_delim == *tmp)
        {
            if (tmp != a_str) // checks for the last comma addrs in case it's not the string beggining
                if ((tmp -last_comma) != 1) // in case it's not sequencal commas - count it
                    count++;
            // if it's the first char don't count it

            last_comma = tmp;
        }
        tmp++;
    }

    /* Add space for trailing token. */
    count += last_comma < (a_str + strlen(a_str) - 1);

    *len = (int)count;
    
    
    result = malloc(sizeof(char*) * count);
    

    if (result)
    {
        token = strtok(a_str, delim);

        while (token)
        {
            if(idx >= count)
            {
             PDEBUG("[ERROR][SPLIT] <str_split> more tokens than expected\n");
                printf("[ERROR] a_srt = %s\tdelim = %s\n",a_str, delim);
                free(result);
                return NULL;
            }
            if ((*(result + idx++) = strdup(token)) == NULL) {
                PDEBUG("[ERROR][SPLIT] <str_split> strdup failed\n");
                PDEBUG("freeing splited line:");
                for (int i = 0 ; i< idx-1 ; i++) {
                    printf("%s\n",result[i]);
                    free(result[i]);
                }
                free(result);
                return NULL;
            }
            
            //PDEBUG("[SPLIT] <str_split> after duplication:\n");
            //printf("%s\n",result[idx-1]);

            token = strtok(NULL, delim);
        }
    }
    
    //PDEBUG("splited line:");
    //for (int i = 0 ; i<*len ; i++) {
    //    printf("%s\n",result[i]);
    //}
    
    return result;
}


/* The function remove all whitespaces from string - the original string is being modified -
 * the sring shouldn't be const or read-only.
 *
 * ****************************************************************************************
 * ***  Credit to Rachids comment on - (base for modification)                         ***
 * ***    http://stackoverflow.com/questions/22430980/removing-spaces-from-a-string ***
 * ****************************************************************************************
 *
 * @param str - string to whitespaces to be deleted from
 * @assert (str != NULL & str is not const (not read-only)
 * @return
 */
void removeWhiteSpaces(char** src)
{
    int i,j;
    char* dst = NULL;
    char* tmp = *src;
    int len   = (int) strlen(*src);
    
    i = 0;
    j = 0;
    
    if ((dst = (char*) malloc(sizeof(char)*(len +1)))== NULL) {
        printf("[ERROR] allocation failed\n");
        return;
    }
    
    while (*tmp != 0) {
        
        if (*tmp == ' ') {
            tmp++;
            continue;
        }
        dst[j] = *tmp;
        tmp++;
        j++;
    }
    
    // Copying the null terminating char
    dst[j] = 0;
    free(*src);
    *src = dst;
}

// remove new line charecter if exist - assuming it at the end of the string
void remove_newLine(char* str) {
    
    char* tmp = str;
    
    while (*tmp != '\n' && *tmp != 0)
        tmp++;
    
    if (*tmp == 0)
        return;
    
    if (*tmp == '\n')
        *tmp = 0;
    return;
}

SPConfig spConfigCreateManually(SP_CONFIG_MSG* msg){
    SPConfig result;

    if ((result = (SPConfig) malloc(sizeof(*result))) == NULL)
    {
        PDEBUG("ERROR : Allocation failed\n");
        return NULL;
    }

    result->spImagesDirectory   = "/specific/a/home/cc/students/cs/barangel/Desktop/Software_Project/FinalProject/images/";
    result->spImagesPrefix      = "img";
    result->spImagesSuffix      = ".png";
    result->spNumOfImages       = 5;
    result->spNumOfFeatures     = 5;
    result->spExtractionMode    = true;
    result->spNumOfSimilarImages= 4;
    result->spKDTreeSplitMethod = RANDOM;
    result->spKNN               = 4;
    result->spMinimalGUI        = false;
    result->spLoggerLevel       = 1;
    result->spLoggerFilename    = "log.log";
    result->spPcaFilename       = "pca.yml";
    result->spPCADimension      = 4;
    *msg = SP_CONFIG_SUCCESS;

    return result;
}

void initializing_SPConfig(SPConfig spconfig) {
    spconfig->spNumOfImages = -1 ;
    spconfig->spNumOfFeatures = -1;
    spconfig->spPCADimension = -1;
    spconfig->spNumOfSimilarImages = -1;
    spconfig->spKNN = -1;
    spconfig->spLoggerLevel = -1;

    spconfig->spImagesDirectory = NULL;
    spconfig->spImagesPrefix = NULL;
    spconfig->spImagesSuffix = NULL;
    spconfig->spPcaFilename = NULL;
    spconfig->spLoggerFilename = NULL;

    spconfig->spKDTreeSplitMethod = -1;

    for (int i =0 ; i < 5; i++)
        spconfig->assignArr[i] = 0;

}

int check_default_params(SPConfig config, const char* filename, int line_num, SP_CONFIG_MSG *msg) {
    if (config->spNumOfImages == -1) {
        printf("File: %s\nLine: %0d\nMessage: Parameter spNumOfImages is not set\n",filename,line_num);
        *msg = SP_CONFIG_MISSING_NUM_IMAGES;
        return -1;
    }
    
    if (config->spImagesDirectory == NULL) {
        printf("File: %s\nLine: %0d\nMessage: Parameter spImagesDirectory is not set\n",filename,line_num);
        *msg = SP_CONFIG_MISSING_DIR;
        return -1;
    }
    
    if (config->spImagesPrefix == NULL) {
        printf("File: %s\nLine: %0d\nMessage: Parameter spImagesPrefix is not set\n",filename,line_num);
        *msg = SP_CONFIG_MISSING_PREFIX;
        return -1;
    }
    
    if (config->spImagesSuffix == NULL) {
        printf("File: %s\nLine: %0d\nMessage: Parameter spImagesSuffix is not set\n",filename,line_num);
        *msg = SP_CONFIG_MISSING_SUFFIX;
        return -1;
    }
    
    return 1;

}
void free_spconfig(SPConfig spconfig) {

    if (spconfig->assignArr[0] == 1)
        free(spconfig->spImagesDirectory);

    if (spconfig->assignArr[1] == 1)
        free(spconfig->spImagesPrefix);

    if (spconfig->assignArr[2] == 1)
        free(spconfig->spImagesSuffix);

    if (spconfig->assignArr[3] == 1)
        free(spconfig->spPcaFilename);

    if (spconfig->assignArr[4] == 1)
        free(spconfig->spLoggerFilename);

}

void free_splited(char** strArr, int
                  len) {

    for (int i =0 ; i<len ; i++)
        free(*(strArr+i));
    free(strArr);
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
