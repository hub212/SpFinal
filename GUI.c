#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "Debug.h"
#include "Query.h"
#include "SPConfig.h"
#include "GUI.h"
//#include "SPImageProc.h"

#define IF_ERROR_EXIT(x) if((x) != SP_CONFIG_SUCCESS){ PDEBUG("last called method did not success'"); return; }
#define STR_MAX 256

//TODO: this macro simulates a presentation of an image. WHen the project is ready the actual image should be shown by using OPENCV.

//#define SHOWIM(x) printf("{ .. Showing image %s%s%d%s .. }\n",imagesPath,imagesPrefix,x,imagesSuffix);
#define SHOWIM(x) printf("{ .. Showing image %s .. }\n",x);

void showMinimalGUI(SPConfig config, int* similarImages, int numOfSimilarImages){
	if(config == NULL){
		PDEBUG("null pointer was sent as argument 'config'");
		return;
	}

	SP_CONFIG_MSG msg;
	char imagesPath[STR_MAX];

	for(int i=0;i<numOfSimilarImages;i++){
		msg = spConfigGetImagePath(imagesPath,config,i);
		IF_ERROR_EXIT(msg);
		SHOWIM(imagesPath);
		printf("press any key to continue...");
		getchar();
		printf("\n");
	}
}
void showNonMinimalGUI(SPConfig config, int* similarImages, int numOfSimilarImages){
	if(config == NULL){
		PDEBUG("null pointer was sent as argument 'config'");
		return;
	}
	SP_CONFIG_MSG msg;
	char imagesPath[STR_MAX];

	char* queryImage = QueryGetFileName();
	printf("Best candidates for - %s - are:\n",queryImage);
	for(int i=0;i<numOfSimilarImages;i++){
		msg = spConfigGetImagePath(imagesPath,config,i);
		IF_ERROR_EXIT(msg);
		printf("%s\n",imagesPath);
	}
}

