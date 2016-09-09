/*
 * main.c


 *
 *  Created on: 14 ·‡Â‚◊ 2016
 *      Author: user
 */


extern "C"{
	#include "SPConfig.h"
	#include "Debug.h"
	#include "Query.h"
	#include "KDTree.h"
	//#include "GUI.h"
	#include "Extraction.h"
}
#include "SPImageProc.h"

#include <cstdlib>
#include <cstdbool>
#include <cstring>
#include <cstdio>

#undef IF_ERROR_EXIT
#define IF_ERROR_EXIT(x) if((x) != SP_CONFIG_SUCCESS){  PDEBUG("last called method did not success"); spConfigDestroy(config); return ERROR_VALUE; }

#define ERROR_VALUE -1
#define DEFAULT_CONFIG "spcbir.config"
#define PRINT_ERROR_HERE(x) spLoggerPrintError((x), __FILE__, __func__, __LINE__);

#define GET_CONFIG_FROMFILE 0
#define STR_MAX 256



void printPoint(SPPoint point) {
	if(point == NULL)
		return;
    printf("DEBUG : -------------- Index %d ---------------\n", spPointGetIndex(point));
    printf("DEBUG : \t\tdim : %d\n", spPointGetDimension(point));
    printf("DEBUG : \t\tcoordinates :\n");
    for (int i=0; i<spPointGetDimension(point); i++) {
        printf("DEBUG : \t\t\tcoor[%d] - %f\n",i,spPointGetAxisCoor(point,i));
    }
}//TODO delete at the end

int getFearuresFromQuery(SPPoint** feats){
    FILE* queryFile = QueryGetFile();
	*feats = NULL;
	return 0;
}
int* findKMaximums(int * arr,int size, int k){ //assuming all elements are non-negative
	if(k >= size) //if k > size than the entire array is k-maximums
		return arr;
	int* res = (int*)malloc(sizeof(int)*k);
	int * arrcopy = (int*)malloc(sizeof(int)*size);

	for(int i=0;i<size;i++)
		arrcopy[i] = arr[i];
	/*
_DBLOCK
			printf("\narrcopy:");
			for(int i=0;i<size;i++){
                    printf("[%d]",arrcopy[i]);
			}
			printf("\n");
			_DEND

*/
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


int main(int argc,char** argv){ //TODO: this is the real main function. the other is used with manual config setting.

	_DBLOCK
		printf("=====the program is running in debug mode====\n");
		printf("=====do not forget to switch off debug-mode before submission====\n\n\n");
	_DEND
	setvbuf(stdout, NULL, _IONBF, 0); //TODO: delete at the end.

	SP_CONFIG_MSG msg;
	SPConfig config;
	SP_LOGGER_MSG logmsg = spLoggerCreate("log.txt",SP_LOGGER_ERROR_LEVEL);

	if(GET_CONFIG_FROMFILE){
		if(logmsg == SP_LOGGER_DEFINED){
			PDEBUG("the logger is already set.");
		}else if(logmsg != SP_LOGGER_SUCCESS){
			PDEBUG("an error occurred while trying to set the Logger.");
			spConfigDestroy(config); //TODO: check
			return ERROR_VALUE;
		}

		bool configSent = false;
		char* configFileName;
		if(argc > 1 && strcmp(argv[argc-1],"-c") == 0){
			perror("Invalid command line : use -c <config_filename>");
			spConfigDestroy(config);//TODO: check
			free(configFileName);
			return ERROR_VALUE;
		}
		for(int i=1;i<argc-1;i++)
			if(strcmp(argv[i],"-c") == 0){
				configSent = true;
				configFileName = argv[i+1];
			}

		if(configSent == false){
			strcpy(configFileName,DEFAULT_CONFIG);
			_D printf("DEBUG: using default configuration file: %s\n",DEFAULT_CONFIG);
		}

	    config = spConfigCreate(configFileName,&msg);


		switch(msg){
		case SP_CONFIG_CANNOT_OPEN_FILE:
			if(configSent)
				printf("The configuration file %s couldnít be open.\n",configFileName);
			else
				printf("The default configuration file spcbir.config couldnít be open\n");
			spConfigDestroy(config);
			free(configFileName);
			return ERROR_VALUE;
		case SP_CONFIG_ALLOC_FAIL:
			printf("An error occurred\nProgram terminated.\n");
			PDEBUG("allocation failed.");
			spConfigDestroy(config);
			free(configFileName);
			return ERROR_VALUE;
		case SP_CONFIG_INVALID_ARGUMENT:
			printf("An error occurred\nProgram terminated.\n");
			PDEBUG("configFileName == NULL");
			spConfigDestroy(config);
			free(configFileName);
			return ERROR_VALUE;

		case SP_CONFIG_INVALID_INTEGER:
		case SP_CONFIG_INVALID_STRING:
		case SP_CONFIG_MISSING_DIR:
		case SP_CONFIG_MISSING_PREFIX:
		case SP_CONFIG_MISSING_SUFFIX:
		case SP_CONFIG_MISSING_NUM_IMAGES:
			printf("One or more of the system variables in %s is missing or contains invalid data.\nProgram terminated.\n",configFileName);
			spConfigDestroy(config);
			free(configFileName);
			return ERROR_VALUE;
		case SP_CONFIG_SUCCESS: break;
		default:
			printf("An unexpected error occurred\nProgram terminated.\n");
			PDEBUG("'msg' contains invalid message.")
			spConfigDestroy(config);
			free(configFileName);
			return ERROR_VALUE;
		}
	}else{//if(USE_MANUAL_CONFIG)
	    PDEBUG("Config file created manually.")
		config = spConfigCreateManually(&msg);
		IF_ERROR_EXIT(msg);
	}


	bool extractionMode = spConfigIsExtractionMode(config, &msg);
	IF_ERROR_EXIT(msg);


	SPPoint* features = NULL; //TODO: extracted features will be stored here.
	int numOfFeatures = 0; //TODO: Number of extracted features will be stored here.

	int numOfImages = spConfigGetNumOfImages(config, &msg);
	IF_ERROR_EXIT(msg);

	sp::ImageProc improc (config);

	if(extractionMode == true){
		PDEBUG("extracting features");
		SPPoint* featForImage;
		int noOfFeatsForImage;
		char impath[256];
		for(int i=0;i<numOfImages;i++){
            msg = spConfigGetImagePath(impath, config, i);//TODO: change suffix.
			IF_ERROR_EXIT(msg);
			featForImage = improc.getImageFeatures(impath,i,&noOfFeatsForImage);
			int res = saveExtractedFeatures(config ,featForImage, noOfFeatsForImage);
			if(res != 0)
				PDEBUG("extracted features could not be saved.");

			numOfFeatures += noOfFeatsForImage;
			features = (SPPoint*)realloc(features,numOfFeatures*sizeof(*features));
			for(int j=0;j<noOfFeatsForImage;j++)
				*(features + numOfFeatures - noOfFeatsForImage + j) = featForImage[j];
		}
		free(featForImage);
		//free(impath);
		//delete improc; //TODO: is it possible?
	}else if(extractionMode == false){
		PDEBUG("read features from files");
		SPPoint* featForImage;
		int noOfFeatsForImage;
		for(int i=0;i<numOfImages;i++){
			noOfFeatsForImage = getExtractedFeatures(config,i,&featForImage);
			numOfFeatures += noOfFeatsForImage;
			features = (SPPoint*)realloc(features,numOfFeatures*sizeof(*features));
			for(int j=0;j<noOfFeatsForImage;j++)
			*(features + numOfFeatures - noOfFeatsForImage + j) = featForImage[j];
		}
		free(featForImage);
	}

	splitMethod split_method = spConfigGetSplitMethod(config,&msg);


	_D printf("\n\nthere are %d features\n",numOfFeatures);
	_D printPoint(*(features+numOfFeatures-1));
	_D printPoint(*(features+numOfFeatures-2));
	_D printPoint(*(features+numOfFeatures-3));
	_D printPoint(*(features+numOfFeatures-4));


	KDTree kdtree = KDTInit(features, numOfFeatures, split_method);


	if(kdtree == NULL){
		PDEBUG("KDTInit returned NULL.");
		return ERROR_VALUE;
	}



	int* similarImages = NULL; //TODO: similar imaged go here.
	int* kNearest;
	//int numOfSimilarImages = 0;

	SPPoint* queryFeatures;
	int numOfQueryFeatures;

	int totalNumOfImages = spConfigGetNumOfImages(config, &msg);
	IF_ERROR_EXIT(msg);

	int numOfSimilarImages = spConfigGetNumOfSimilarImages(config, &msg);
	IF_ERROR_EXIT(msg);

	int k = spConfigGetNumOfSimilarImages(config,&msg);
	 IF_ERROR_EXIT(msg);

	int* candidates = (int*)calloc(totalNumOfImages, sizeof(int));

	int queryChoice = Query();

	while(queryChoice == 1){

        char* queryFile = QueryGetFileName();

        queryFeatures = improc.getImageFeatures(queryFile,0,&numOfQueryFeatures);


		SPPoint currFeat;
		_D printf("numOfQueryFeatures = %d\n",numOfQueryFeatures);
		for(int i=0;i<numOfQueryFeatures;i++){
			currFeat = queryFeatures[i];
			_D printf("queryFeatures[%d] = %d\n",i,queryFeatures[i]);
			kNearest = getNearestNeighbors(kdtree,currFeat,config);
			_D printf(">>>>>fiouw<<<<",i,queryFeatures[i]);



			_DBLOCK
			printf("\nOut method");
			for(int i=0;i<k;i++){
                    printf("[%d]",kNearest[i]);
			}
			printf("\n");
			_DEND
			for(int i=0;i<k;i++)
				candidates[kNearest[i]]++;
		}
		_DBLOCK
			printf("\nCandidates");
			for(int i=0;i<totalNumOfImages;i++){
                    printf("[%d]",candidates[i]);
			}
			printf("\n");
        _DEND

		similarImages = findKMaximums(candidates,totalNumOfImages,numOfSimilarImages);


		_DBLOCK
			printf("\nSimilarImages:");
			for(int i=0;i<k;i++){
                    printf("[%d]",similarImages[i]);
			}
			printf("\n");
        _DEND

		bool minimalGui = spConfigMinimalGui(config,&msg);
		IF_ERROR_EXIT(msg);

		if(minimalGui == true){
			SP_CONFIG_MSG msg;
			char imagesPath[STR_MAX];

			for (int i = 0; i < numOfSimilarImages; i++) {
				msg = spConfigGetImagePath(imagesPath, config, similarImages[i]);
				IF_ERROR_EXIT(msg);
				improc.showImage(imagesPath);
				printf("press any key to continue...");
				getchar();
				printf("\n");
			}
		}
		else{
			SP_CONFIG_MSG msg;
			char imagesPath[STR_MAX];

			char* queryImage = QueryGetFileName();
			printf("Best candidates for - %s - are:\n", queryImage);
			for (int i = 0; i < numOfSimilarImages; i++) {
				msg = spConfigGetImagePath(imagesPath, config, similarImages[i]);
				IF_ERROR_EXIT(msg);
				printf("%s\n", imagesPath);
			}
		}
        if(DEBUG)
            queryChoice = 0;
        else
            queryChoice = Query();
	}
	//TODO: free cause error
	//free(candidates);
	//KDTDestroy(kdtree);
	//spConfigDestroy(config);

	//for(int i=0;i<numOfFeatures;i++)
	//	spPointDestroy(features[i]);
	//for(int i=0;i<numOfQueryFeatures;i++)
	//	spPointDestroy(queryFeatures[i]);
	//free(features);
	//free(queryFeatures);
	if(queryChoice == 0){
		printf("Have a nice day :)\n");
		PDEBUG("the user terminated the program");
		return 0;
	}else{
		PDEBUG("The program terminated due to error in Query.");
		return ERROR_VALUE;
	}
}
