/*
 * Debug.h
 *
 *  Created on: 12 αιεμι 2016
 *      Author: user
 */


#define DEBUG 1

#define PDEBUG(x) if(DEBUG){ printf("DEBUG: %s (in %s; line %d)\n",x,__func__,__LINE__); fflush(NULL);}

#define CHECK_ARGUMENT_DATA(x,y,z) if((x) == (y)){ if(DEBUG){ printf("DEBUG: invalid argument '%s' (in %s, line %s)\n",x,__func__,__LINE__); fflush(NULL);} return (z);}

#define CHECK_ARGUMENT(x,y) CHECK_ARGUMENT_DATA(x,NULL,y)

#define DEBUG_EXIT if(DEBUG){ PDEBUG(">>>>The program or function was terminted on debug mode<<<<"); return 2; }

#define _D if(DEBUG)

#define _DBLOCK if(DEBUG){
#define _DEND }

