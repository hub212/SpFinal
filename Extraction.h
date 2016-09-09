/*
 * Extraction.h
 *
 *  Created on: 31 áàåâ× 2016
 *      Author: user
 */

#include "SPConfig.h"
#include "SPPoint.h"

#ifndef EXTRACTION_H_
#define EXTRACTION_H_


/*
    Saves the the extracted features into file. the file will be stored in the same directory of the image, and with the same name.

    @param config - configuration file
    @param feats - the array of the extracted features
    @param count - the length of the array 'feats'

    @return a negative number in case of error. 0 otherwise.
*/
int saveExtractedFeatures(SPConfig config ,SPPoint* feats, int count);


/*
    Get image features from its corresponding '.feats' file

    @param config - configuration file
    @param feats - the index of the image we want to get the features of
    @param pfeats - the feature array will be stored here.

    @return the number of features
*/
int getExtractedFeatures(SPConfig config, int indexOfImage ,SPPoint** pfeats);

#endif /* EXTRACTION_H_ */
