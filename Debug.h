/*
 * Debug.h
 *
 *  Created on: 12 αιεμι 2016
 *      Author: user
 */
#include "SPLogger.h"

#define DEBUG 1

#define STR_MAX 256

#define PDEBUG(x) if(DEBUG){ printf("DEBUG: %s (in %s; line %d)\n",x,__func__,__LINE__); fflush(NULL);}

#define DEBUG_EXIT if(DEBUG){ PDEBUG(">>>>The program or function was terminted on debug mode<<<<"); return 2; }

#define _D if(DEBUG)

#define _DBLOCK if(DEBUG){
#define _DEND }


#define LOG_WRITE_ERROR(x) SP_LOGGER_MSG logm = spLoggerPrintError((x), __FILE__, __func__, __LINE__); if(logm != SP_LOGGER_SUCCESS){ \
                                            printf("File: %s\nLine: %d\nMessage: Could not print message to logger.\n",__FILE__,__LINE__); }
#define LOG_WRITE_WARNING(x) SP_LOGGER_MSG logm = spLoggerPrintWarning((x), __FILE__, __func__, __LINE__); if(logm != SP_LOGGER_SUCCESS){ \
                                            printf("File: %s\nLine: %d\nMessage: Could not print message to logger.\n",__FILE__,__LINE__); }


#define ERROR_VALUE -1


#define IF_ERROR_EXIT(x) if((x) == SP_CONFIG_INVALID_ARGUMENT){ LOG_WRITE_ERROR("The function recived NULL pointer as argument 'config'");\
                                                                PDEBUG("last called method did not success"); spConfigDestroy(config); return ERROR_VALUE; }
#define IF_ERROR_EXIT_RET(x,y) if((x) == SP_CONFIG_INVALID_ARGUMENT){ LOG_WRITE_ERROR("The function recived NULL pointer as argument 'config'");\
                                                                PDEBUG("last called method did not success"); spConfigDestroy(config); return (y);}

#define CHECK_ARGUMENT_DATA(x,y,z) if((x) == (y)){ if(DEBUG){ printf("DEBUG: invalid argument '%s' (in %s, line %s)\n",x,__func__,__LINE__); fflush(NULL);} return (z);}

#define CHECK_ARGUMENT(x,y) CHECK_ARGUMENT_DATA(x,NULL,y)
/*
#define SPCONFIG_INV_ARG_OR_RANGE(x) if((x) == SP_CONFIG_INVALID_ARGUMENT){\
                                        LOG_WRITE_ERROR("The function recived NULL pointer as argument 'config'"); return; \
                                        PDEBUG("last called method did not success"); spConfigDestroy(config); return ERROR_VALUE;\
                                     }else if((x) == SP_CONFIG_INDEX_OUT_OF_RANGE){\
                                        LOG_WRITE_ERROR("The function recived illegal Index value"); return; \
                                        PDEBUG("last called method did not success"); spConfigDestroy(config); return ERROR_VALUE;\
                                     }
*/
#define MALLOC_FAIL(x) if(!(x)){ PDEBUG("DEBUG: malloc failed"); LOG_WRITE_WARNING("Memory Allocation Failed."); return;  }				//void
#define MALLOC_FAIL_NULL(x) if(!(x)){ PDEBUG("DEBUG: malloc failed"); LOG_WRITE_WARNING("Memory Allocation Failed."); return NULL; }	//returns null pointer
#define MALLOC_FAIL_INT(x,y) if(!(x)){ PDEBUG("DEBUG: malloc failed"); LOG_WRITE_WARNING("Memory Allocation Failed."); return (y); }	//returns integer y


#define NOVA_FIX 1
