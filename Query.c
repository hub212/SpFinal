#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Debug.h"
#include "Query.h"
//TODO: Consider join it to main.c

#define EXIT_STRING "exit"

char fileName[256];
FILE* file;

int Query(){
	char c[256];
	FILE* f;
	printf("Please enter an image path:\n");
	scanf("%s",&c);
	_D printf("DEBUG: The user entered the path: %s\n",c);
	//TODO: consider checking if the input file is an image.

	if(strcmp(c,"") == 0 || strcmp(c,EXIT_STRING) == 0)
		return 0;

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
