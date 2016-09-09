#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Debug.h"
#include "Query.h"
//TODO: Consider join it to main.c

#define EXIT_STRING "<>"

#define D_USE_DEFAULT 1
#define D_DEFAULT_IMAGE "D:\\Documents\\Dropbox\\TAU\\Software_Project\\HW\\Final_as_cpp\\Experiments\\exper1.png"

char fileName[256];
FILE* file;

int Query(){
	char c[256];
	FILE* f;
	printf("Please enter an image path:\n");
	if(D_USE_DEFAULT == 0){
        scanf("%s",c);
        _D printf("DEBUG: The user entered the path: %s\n",c);
        //TODO: consider checking if the input file is an image.
	}else{
        _D printf("[DEBUG]: Using Default Query image %s",D_DEFAULT_IMAGE);
        strcpy(c,D_DEFAULT_IMAGE);
	}
    if(strcmp(c,"") == 0 || strcmp(c,EXIT_STRING) == 0){
        PDEBUG("exit sting was entered.")
        return 0;
    }
    f = fopen(c,"r");
    if(f == NULL){
        _D printf("DEBUG: File not Found! - %s\n",c);
        return -1;
    }
    strcpy(fileName,c);
    file = f;
	return 1;
}

char* QueryGetFileName(){
	if(fileName == NULL)
		PDEBUG("FileName is NULL.");
	return fileName;
}

FILE* QueryGetFile(){
	if(file == NULL)
		PDEBUG("file is NULL.");
	return file;
}
