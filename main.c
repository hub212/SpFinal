/*
 * main.c
 *
 *  Created on: 14 баев„ 2016
 *      Author: user
 */
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>

#include "SPConfig.h"
#include "Debug.h"
#include "Query.h"
#include "KDTree.h"
#include "GUI.h"

#undef IF_ERROR_EXIT
#define IF_ERROR_EXIT(x) if((x) != SP_CONFIG_SUCCESS){  PDEBUG("last called method did not success"); return ERROR_VALUE; }

#define ERROR_VALUE -1
#define DEFAULT_CONFIG "spcbir.config"


//TODO: this should be converted to c++



int main(int argc,char** argv){

	_DBLOCK
		printf("=====the program is running in debug mode====\n");
		printf("=====do not forget to switch off debug-mode before submission====\n\n\n");
	_DEND

	setvbuf(stdout, NULL, _IONBF, 0); //TODO: delete at the end.

	bool configSent = false;
	char* configFileName;
	if(argc > 1 && strcmp(argv[argc-1],"-c") == 0){
		perror("Configuration file name is required after the parameter -c.\nProgram terminated.\n");
		return ERROR_VALUE;
	}
	for(int i=1;i<argc-1;i++)
		if(strcmp(argv[i],"-c") == 0){
			configSent = true;
			configFileName = argv[i+1];
		}

	if(configSent == false){
		configFileName = DEFAULT_CONFIG;
		_D printf("DEBUG: using default configuration file: %s\n",DEFAULT_CONFIG);
	}

	SP_CONFIG_MSG msg;

	SPConfig config = spConfigCreate(configFileName,&msg);

	switch(msg){
	case SP_CONFIG_CANNOT_OPEN_FILE:
		printf("the file %s cannot be opened.\nProgram terminated.\n",configFileName);
		return ERROR_VALUE;
	case SP_CONFIG_ALLOC_FAIL:
		printf("An error occurred\nProgram terminated.\n");
		PDEBUG("allocation failed.")
		return ERROR_VALUE;
	case SP_CONFIG_INVALID_ARGUMENT:
		printf("An error occurred\nProgram terminated.\n");
		PDEBUG("configFileName == NULL")
		return ERROR_VALUE;

	case SP_CONFIG_INVALID_INTEGER:
	case SP_CONFIG_INVALID_STRING:
	case SP_CONFIG_MISSING_DIR:
	case SP_CONFIG_MISSING_PREFIX:
	case SP_CONFIG_MISSING_SUFFIX:
	case SP_CONFIG_MISSING_NUM_IMAGES:
		printf("One or more of the system variables in %s is missing or contains invalid data.\nProgram terminated.\n",configFileName);
		return ERROR_VALUE;
	case SP_CONFIG_SUCCESS: break;
	default:
		printf("An unexpected error occurred\nProgram terminated.\n");
		PDEBUG("'msg' contains invalid message.")
		return ERROR_VALUE;
	}
	bool extractionMode = spConfigIsExtractionMode(config, &msg);
	IF_ERROR_EXIT(msg);


	SPPoint* featured = NULL; //TODO: extracted features will be stored here.
	int numOfFeatures = 0; //TODO: Number of extracted features will be stored here.
	if(extractionMode == true){
		//TODO: extraction mode here.
	}else if(extractionMode == false){
		//TODO: non-extraction mode here.
	}



	splitMethod split_method = spConfigGetSplitMethod(config,&msg);
	IF_ERROR_EXIT(msg);

	KDTree kdtree = KDTInit(featured, numOfFeatures, split_method);
	if(kdtree == NULL){
		PDEBUG("KDTInit returned NULL.");
		return ERROR_VALUE;
	}


	int queryChoice = Query();
	int* similarImages = NULL; //TODO: similar imaged go here.
	int numOfSimilarImages = 0;


	while(queryChoice == 1){
		/*
		 * TODO: k nearest neighbours searching here.
		 */

		bool minimalGui = spConfigMinimalGui(config,&msg);
		IF_ERROR_EXIT(msg);

		if(minimalGui == true)
			showMinimalGUI(config,similarImages,numOfSimilarImages);
		else
			showNonMinimalGUI(config,similarImages,numOfSimilarImages);

		queryChoice = Query();
	}
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
