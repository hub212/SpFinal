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

#undef IF_ERROR_EXIT
#define IF_ERROR_EXIT(x) if((x) != SP_CONFIG_SUCCESS){  PDEBUG("last called method did not success"); spConfigDestroy(config); return ERROR_VALUE; }

#define ERROR_VALUE -1
#define DEFAULT_CONFIG "D:\\Documents\\Dropbox\\TAU\\Software_Project\\HW\\Final_as_cpp\\spcbir.config"
#define PRINT_ERROR_HERE(x) spLoggerPrintError((x), __FILE__, __func__, __LINE__);

#define GET_CONFIG_FROMFILE 1
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

int* findKMaximums(int * arr,int size, int k){ //assuming all elements are non-negative
	if(k >= size) //if k > size than the entire array is k-maximums
		return arr;
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
	_DBLOCK
		printf("=====the program is running in debug mode====\n");
		printf("=====do not forget to switch off debug-mode before submission====\n\n\n");
		setvbuf(stdout, NULL, _IONBF, 0); //TODO: delete at the end.
	_DEND

	SP_CONFIG_MSG msg;
	SPConfig config; //TODO: free
	SP_LOGGER_MSG logmsg = spLoggerCreate("log.txt",SP_LOGGER_ERROR_LEVEL); //TODO: free

	if(GET_CONFIG_FROMFILE){
		if(logmsg == SP_LOGGER_DEFINED){
			PDEBUG("the logger is already set.");
		}else if(logmsg != SP_LOGGER_SUCCESS){
			PDEBUG("an error occurred while trying to set the Logger.");
			spConfigDestroy(config); //TODO: check
			return ERROR_VALUE;
		}

		bool configSent = false;
		char configFileName[256];
		if(argc > 1 && strcmp(argv[argc-1],"-c") == 0){
			perror("Invalid command line : use -c <config_filename>");
			spConfigDestroy(config);//TODO: check
			free(configFileName);
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
    _D printf(">>>>>>>>configFileName: %s\n", configFileName);
	    config = spConfigCreate(configFileName,&msg);
	    PDEBUG("here is good");
		switch(msg){
		case SP_CONFIG_CANNOT_OPEN_FILE:
			if(configSent)
				printf("The configuration file %s couldn’t be open.\n",configFileName);
			else
				printf("The default configuration file spcbir.config couldn’t be open\n");
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

	PDEBUG("here is good too");

	bool extractionMode = spConfigIsExtractionMode(config, &msg);
	IF_ERROR_EXIT(msg);


	SPPoint* features = NULL;//TODO: free //TODO: extracted features will be stored here.
	int numOfFeatures = 0; //TODO: Number of extracted features will be stored here.

	int numOfImages = spConfigGetNumOfImages(config, &msg);
	IF_ERROR_EXIT(msg);

	PDEBUG("here is good again");

	sp::ImageProc improc (config);

	SPPoint* featForImage; //TODO: free
	if(extractionMode == true){
		PDEBUG("extracting features");
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

//TODO: free memory cause error
            //Freeing 'featForImage'
           // for(int i=0;i<noOfFeatsForImage;i++){
           //     spPointDestroy(featForImage[i]);
           // }
          //  free(featForImage);
		}
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

                //TODO: free memory cause error
             //Freeing 'featForImage'
           // for(int i=0;i<noOfFeatsForImage;i++){
           //     spPointDestroy(featForImage[i]);
           // }
           // free(featForImage);
		}

	}

	splitMethod split_method = spConfigGetSplitMethod(config,&msg);


	KDTree kdtree = KDTInit(features, numOfFeatures, split_method);


	if(kdtree == NULL){
		PDEBUG("KDTInit returned NULL.");
		return ERROR_VALUE;
	}


	int* similarImages = NULL; //TODO: similar imaged go here.
	int* kNearest;
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
		for(int i=0;i<numOfQueryFeatures;i++){
			currFeat = queryFeatures[i];
        kNearest = getNearestNeighbors(kdtree,currFeat,config);

        spPointDestroy(currFeat);

        for(int i=0;i<k;i++)
            candidates[kNearest[i]]++;
		}

		similarImages = findKMaximums(candidates,totalNumOfImages,numOfSimilarImages);

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
			free(queryImage);
			for (int i = 0; i < numOfSimilarImages; i++) {
				msg = spConfigGetImagePath(imagesPath, config, similarImages[i]);
				IF_ERROR_EXIT(msg);
				printf("%s\n", imagesPath);
			}
		}
        if(DEBUG)
            queryChoice = 0; //In debug mode it won't be possible to enter a new query image.
        else
            queryChoice = Query();
	}

	free(candidates);
	free(kNearest);
	free(similarImages);



	/* TODO: free memory cause error
	for(int i=0;i<numOfQueryFeatures;i++){
        spPointDestroy(queryFeatures[i]);
	}
	free(queryFeatures);

	for(int i=0;i<numOfFeatures;i++){
        spPointDestroy(features[i]);
	}
	free(features);
*/

	KDTDestroy(kdtree);
	spConfigDestroy(config);


	if(queryChoice == 0){
		printf("Have a nice day :)\n");
		PDEBUG("the user terminated the program");
		return 0;
	}else{
		PDEBUG("The program terminated due to error in Query.");
		return ERROR_VALUE;
	}
}
