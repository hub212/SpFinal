/*
 * Query.h
 *
 *  Created on: 14 ����� 2016
 *      Author: user
 */

#ifndef QUERY_H_
#define QUERY_H_

/**
 * return 1 if some image path was entered. 0 if the user decided to quit the program. -1 for errors.
 */
int Query();

char* QueryGetFileName();

FILE* QueryGetFile();

#endif /* QUERY_H_ */
