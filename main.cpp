extern "C"{
	#include "SPConfig.h"
	#include "Debug.h"
	#include "Query.h"
	#include "KDTree.h"
	#include "SPLogger.h"
	#include "Extraction.h"
}
#include "SPImageProc.h"

#include <cstdlib>
#include <cstdbool>
#include <cstring>
#include <cstdio>

//#undef IF_ERROR_EXIT
//#define IF_ERROR_EXIT(x) if((x) != SP_CONFIG_SUCCESS){  PDEBUG("last called method did not success"); spConfigDestroy(config); return ERROR_VALUE; }

#define DEFAULT_CONFIG "D:\\Documents\\Dropbox\\TAU\\Software_Project\\HW\\Final_as_cpp\\spcbir.config"
#define PRINT_ERROR_HERE(x) spLoggerPrintError((x), __FILE__, __func__, __LINE__);

#define GET_CONFIG_FROMFILE 1
#define STR_MAX 256

#define EXIT_MSG "Exiting..."

//TODO:redundant
#define INVALID_CONFIG_LINE_ERROR printf("File: %s\nLine: %d\nMessage: Invalid configuration line\n",__FILE__,__LINE__)
#define INVALID_VALUE_ERROR printf("File: %s\nLine: %d\nMessage: Invalid value - constraint not met\n",__FILE__,__LINE__)
#define PARAM_NOT_MET_ERROR(x) printf("File: %s\nLine: %d\nMessage: Parameter %s is not set\n",__FILE__,__LINE__,x)



void destroyArrayOfPoints(SPPoint* arr, int size){
    for(int i=0;i<size;i++)
        spPointDestroy(arr[i]);
    free(arr);
}

int* findMaximums(int * arr,int size, int k){ //assuming all elements are non-negative
	if(k > size) //if k > size than the entire array is k-maximums
    {
        PDEBUG("more similarities than pictures.");
        return findMaximums(arr,size,size);
    }
	int* res = (int*)malloc(sizeof(int)*k);
	int * arrcopy = (int*)malloc(sizeof(int)*size);

	for(int i=0;i<size;i++)
		arrcopy[i] = arr[i];

	int maxj;
	for(int i=0;i<k;i++){
		maxj = 0;
		for(int j=0;j<size;j++){
			if(arrcopy[j] > arrcopy[maxj])
				maxj = j;
		}
		res[i] = maxj;
		arrcopy[maxj] = -1;
	}
	free(arrcopy);
	return res;
}


int main(int argc,char** argv){
	_DBLOCK // line in _DBLOCK will be execute only on debugging mode
		printf("=====the program is running in debug mode====\n");
		printf("=====do not forget to switch off debug-mode before submission====\n\n\n");
		//setvbuf(stdout, NULL, _IONBF, 0); //TODO: delete at the end.
	_DEND





/*

*/

    SP_CONFIG_MSG msg;
	SPConfig config = NULL;


	if(GET_CONFIG_FROMFILE){
		bool configSent = false;
		char configFileName[256];
		if(argc > 1 && strcmp(argv[argc-1],"-c") == 0){
			printf("Invalid command line : use -c <config_filename>\n");
			spConfigDestroy(config);//TODO: check
			return ERROR_VALUE;
		}
		for(int i=1;i<argc-1;i++)
			if(strcmp(argv[i],"-c") == 0){
				configSent = true;
				strcpy(configFileName,argv[i+1]);
			}

		if(configSent == false){
			strcpy(configFileName,DEFAULT_CONFIG);
			_D printf("DEBUG: using default configuration file: %s\n",DEFAULT_CONFIG);
		}
	    config = spConfigCreate(configFileName,&msg);

        if(msg != SP_CONFIG_SUCCESS){
            if(config != NULL){
                PDEBUG("Possible bug is spConfigCreate.")
                spConfigDestroy(config);
            }
            if(msg == SP_CONFIG_CANNOT_OPEN_FILE){
                if(configSent == true)
                    printf("The configuration file %s couldn't be open\n",configFileName);
                else
                    printf("The default configuration file spcbir.config couldn't be open\n");
            }

            return ERROR_VALUE;
        }
        if(config == NULL){
            PDEBUG("Major bug! spConfigCreate returned SUCCESS but config == NULL");
            return ERROR_VALUE;
        }
	}else{//if(USE_MANUAL_CONFIG)
	    PDEBUG("Config file created manually.")
		config = spConfigCreateManually(&msg);
		IF_ERROR_EXIT(msg);
	}


	char* logfileName = (char*)malloc(sizeof(char)*STR_MAX);
	int loglevel;
	if(logfileName == NULL){PDEBUG("Internal problem in function: allocation failed."); spConfigDestroy(config); return ERROR_VALUE;}

	msg = spConfigGetLogFilename(logfileName,config);
    if(msg == SP_CONFIG_INVALID_ARGUMENT){
        PDEBUG("Major bug: LoggerFilename is not set in config");
        spConfigDestroy(config);
        return ERROR_VALUE;
    }

    loglevel = spConfigGetLoggerLevel(config,&msg);
    if(msg == SP_CONFIG_INVALID_ARGUMENT){
        PDEBUG("Major bug: LoggerLevel is not set in config");
        spConfigDestroy(config);
        return ERROR_VALUE;
    }

	_D printf("DEBUG: logfilename: %s\n",logfileName);

	SP_LOGGER_MSG logmsg;
	switch(loglevel){
    case 1:
         logmsg = spLoggerCreate(logfileName,SP_LOGGER_ERROR_LEVEL);
        break;
    case 2:
         logmsg = spLoggerCreate(logfileName,SP_LOGGER_WARNING_ERROR_LEVEL);
        break;
    case 3:
          logmsg = spLoggerCreate(logfileName,SP_LOGGER_INFO_WARNING_ERROR_LEVEL);
        break;
    case 4:
         logmsg = spLoggerCreate(logfileName,SP_LOGGER_DEBUG_INFO_WARNING_ERROR_LEVEL);
        break;
	default:
        PDEBUG("Major Error. LoggerLevel invalid");
        spConfigDestroy(config);
        return ERROR_VALUE;
	}

    switch(logmsg){
        case SP_LOGGER_DEFINED:
            PDEBUG("the logger is already set.");
            break;
        case SP_LOGGER_OUT_OF_MEMORY:
             PDEBUG("logger memory allocation failed");
             spConfigDestroy(config);
             return ERROR_VALUE;
        case SP_LOGGER_CANNOT_OPEN_FILE:
            PDEBUG("cannot open file");
            spConfigDestroy(config);
            return ERROR_VALUE;
        case SP_LOGGER_SUCCESS:
            PDEBUG("logger was set with no errors");
            break;
        default:
            PDEBUG("Unexpected error occurred");
            spConfigDestroy(config);
            return ERROR_VALUE;
    }

	spLoggerPrintInfo("program started.");



	bool extractionMode = spConfigIsExtractionMode(config, &msg);
	if(msg == SP_CONFIG_INVALID_ARGUMENT){
       // spLoggerPrintError("NULL pointer was sent SPConfig.","SPConfig.c","spConfigIsExtractionMode",1);
        spConfigDestroy(config);
        spLoggerDestroy();
        return ERROR_VALUE;
	}

	SPPoint* features = NULL;//TODO: free //TODO: extracted features will be stored here.
	int numOfFeatures = 0; //TODO: Number of extracted features will be stored here.

	int numOfImages = spConfigGetNumOfImages(config, &msg);
	if(msg == SP_CONFIG_INVALID_ARGUMENT){
        //spLoggerPrintError("NULL pointer was sent SPConfig.","SPConfig.c","spConfigGetNumOfImages",1);
        spConfigDestroy(config);
        spLoggerDestroy();
        return ERROR_VALUE;
	}

	sp::ImageProc improc (config); //No need to free.
	SPPoint* featForImage; //TODO: free
int noOfFeatsForImage;
	if(extractionMode == true){
		PDEBUG("extracting features");

		char impath[256];
		for(int i=0;i<numOfImages;i++){
            msg = spConfigGetImagePath(impath, config, i);
			if(msg != SP_CONFIG_SUCCESS){
                spConfigDestroy(config);
                spLoggerDestroy();
                return ERROR_VALUE;
			}
			featForImage = improc.getImageFeatures(impath,i,&noOfFeatsForImage);
			int res = saveExtractedFeatures(config ,featForImage, noOfFeatsForImage);
			if(res != 0){
				PDEBUG("extracted features could not be saved.");
				spLoggerPrintWarning("The program could not save the extracted features.",__FILE__,__func__,__LINE__);
			}

			numOfFeatures += noOfFeatsForImage;
			features = (SPPoint*)realloc(features,numOfFeatures*sizeof(*features));
			for(int j=0;j<noOfFeatsForImage;j++)
				*(features + numOfFeatures - noOfFeatsForImage + j) = featForImage[j];
		}
	}else if(extractionMode == false){
		PDEBUG("read features from files");
		for(int i=0;i<numOfImages;i++){
			noOfFeatsForImage = getExtractedFeatures(config,i,&featForImage);
			numOfFeatures += noOfFeatsForImage;
			features = (SPPoint*)realloc(features,numOfFeatures*sizeof(*features));
			if(features == NULL){
                    PDEBUG("Internal problem in function: allocation failed.");
                    spConfigDestroy(config);
                    spLoggerDestroy();
                    destroyArrayOfPoints(featForImage,noOfFeatsForImage);
                    return ERROR_VALUE;
            }
			for(int j=0;j<noOfFeatsForImage;j++)
                *(features + numOfFeatures - noOfFeatsForImage + j) = featForImage[j];
		}

	}

//	destroyArrayOfPoints(featForImage,noOfFeatsForImage);
    free(featForImage);


	splitMethod split_method = spConfigGetSplitMethod(config,&msg);
	if(msg != SP_CONFIG_SUCCESS){
        spConfigDestroy(config);
        spLoggerDestroy();
        destroyArrayOfPoints(features,numOfFeatures);
        return ERROR_VALUE;
    }

	KDTree kdtree = KDTInit(features, numOfFeatures, split_method);

	if(kdtree == NULL){
		PDEBUG("KDTInit returned NULL.");
		spLoggerPrintError("Unable to construct the KD-Tree", __FILE__,__func__,__LINE__);
		spConfigDestroy(config);
        spLoggerDestroy();
        destroyArrayOfPoints(features,numOfFeatures);
		return ERROR_VALUE;
	}


	int* similarImages = NULL; //TODO: similar imaged go here.
	int* kNearest;
	SPPoint* queryFeatures;
	int numOfQueryFeatures;

	int totalNumOfImages = spConfigGetNumOfImages(config, &msg);
	if(msg != SP_CONFIG_SUCCESS){
        spConfigDestroy(config);
        spLoggerDestroy();
        KDTDestroy(kdtree);
        destroyArrayOfPoints(features,numOfFeatures);
        return ERROR_VALUE;
    }

	int numOfSimilarImages = spConfigGetNumOfSimilarImages(config, &msg);
	if(msg != SP_CONFIG_SUCCESS){
        spConfigDestroy(config);
        spLoggerDestroy();
        KDTDestroy(kdtree);
        destroyArrayOfPoints(features,numOfFeatures);
        return ERROR_VALUE;
    }

	int* candidates = (int*)malloc(totalNumOfImages*sizeof(int));
	if(candidates == NULL){
        PDEBUG("Internal problem in function: allocation failed.");
        spConfigDestroy(config);
        spLoggerDestroy();
        destroyArrayOfPoints(features,numOfFeatures);
        KDTDestroy(kdtree);
        return ERROR_VALUE;
    }

	for(int i=0;i<totalNumOfImages;i++){
        candidates[i] = 0;
	}

	int queryChoice = Query();

	int knn;

	while(queryChoice == 1){

        char* queryFile = QueryGetFileName();
        if(queryFile == NULL){
            PDEBUG("Major bug.");
             spConfigDestroy(config);
            spLoggerDestroy();
            destroyArrayOfPoints(features,numOfFeatures);
            KDTDestroy(kdtree);
            free(candidates);
            return ERROR_VALUE;
        }

        queryFeatures = improc.getImageFeatures(queryFile,0,&numOfQueryFeatures);


		SPPoint currFeat;

		for(int i=0;i<numOfQueryFeatures;i++){
			currFeat = queryFeatures[i];

            kNearest = getNearestNeighbors(kdtree,currFeat,config,&knn);
            if(kNearest == NULL){
               PDEBUG("Major bug.");
                 spConfigDestroy(config);
                spLoggerDestroy();
                destroyArrayOfPoints(features,numOfFeatures);
                destroyArrayOfPoints(queryFeatures,numOfQueryFeatures);
                KDTDestroy(kdtree);
                free(candidates);
                return ERROR_VALUE;
            }

            for(int i=0;i<knn;i++)
                candidates[kNearest[i]]++;
		}
		similarImages = findMaximums(candidates,totalNumOfImages,numOfSimilarImages);
        free(kNearest);
        destroyArrayOfPoints(queryFeatures,numOfQueryFeatures);

		bool minimalGui = spConfigMinimalGui(config,&msg);
		if(msg != SP_CONFIG_SUCCESS){
         spConfigDestroy(config);
            spLoggerDestroy();
            destroyArrayOfPoints(features,numOfFeatures);
            KDTDestroy(kdtree);
            free(candidates);
            free(similarImages);
            return ERROR_VALUE;
    }

		if(minimalGui == true){
            PDEBUG("[START SHOWING images on screen]");
			char imagesPath[STR_MAX];

			for (int i = 0; i < numOfSimilarImages; i++) {
				msg = spConfigGetImagePath(imagesPath, config, similarImages[i]);
				if(msg != SP_CONFIG_SUCCESS){
                    spConfigDestroy(config);
                    spLoggerDestroy();
                    destroyArrayOfPoints(features,numOfFeatures);
                    KDTDestroy(kdtree);
                    free(candidates);
                    free(similarImages);
                    return ERROR_VALUE;
                }
				_D printf("imagesPath: %s\n", imagesPath);
				improc.showImage(imagesPath);
				printf("press any key to continue to the next image...\n");
				getchar();
			}
		}
		else{
			char imagesPath[STR_MAX];

			char* queryImage = QueryGetFileName();
			printf("Best candidates for - %s - are:\n", queryImage);
			for (int i = 0; i < numOfSimilarImages; i++) {
				msg = spConfigGetImagePath(imagesPath, config, similarImages[i]);
				if(msg != SP_CONFIG_SUCCESS){
                    spConfigDestroy(config);
                    spLoggerDestroy();
                    destroyArrayOfPoints(features,numOfFeatures);
                    KDTDestroy(kdtree);
                    free(candidates);
                    free(similarImages);
                    return ERROR_VALUE;
                }
				printf("%s\n", imagesPath);
			}
            //free(queryImage); //TODO: cause error on NOVA
		}
        if(DEBUG)
            queryChoice = 0;
        else
            queryChoice = Query();
	}

    spConfigDestroy(config);
    destroyArrayOfPoints(features,numOfFeatures);
    KDTDestroy(kdtree);
    free(candidates);
	free(similarImages);

	spLoggerPrintInfo("program finished with no errors.");

    spLoggerDestroy();

	if(queryChoice == 0){
		printf("%s\n",EXIT_MSG);
		PDEBUG("the user terminated the program");
		return 0;
	}else{
		PDEBUG("The program terminated due to error in Query.");
		return ERROR_VALUE;
	}
}
