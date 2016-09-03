/*
 * Debug.h
 *
 *  Created on: 12 αιεμι 2016
 *      Author: user
 */

#ifndef DEBUG_H_
#define DEBUG_H_

#define DEBUG 1

#define PDEBUG(x) if(DEBUG){ printf("DEBUG: %s (in %s)\n",x,__func__); fflush(NULL);}

#define CHECK_ARGUMENT_DATA(x,y,z) if((x) == (y)){ if(DEBUG){ printf("DEBUG: invalid argument '%s' (in %s)\n",x,__func__); fflush(NULL);} return (z);}

#define CHECK_ARGUMENT(x,y) CHECK_ARGUMENT_DATA(x,NULL,y)


#define _D if(DEBUG)

#define _DBLOCK if(DEBUG){
#define _DEND }

#endif /* DEBUG_H_ */
