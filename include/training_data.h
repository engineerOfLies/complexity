#ifndef __TRAINING_DATA_H
#define __TRAINING_DATA_H

#include "gf2d_list.h"
#include "gf2d_text.h"

typedef struct
{
    Uint32      timeIndex;          /**<milliseconds since the session began*/
    Uint8       playerPosition;     /**<which lane the player is in*/
    TextLine    actionTaken;        /**<name of action taken by the test run*/
}Moment; 

typedef struct
{
    List   *data;                   /**<list of moments and actions taken*/
    Uint32  sessionLength;
    Uint32  obstaclesAvoided;
    Uint32  collectablesObtained;
    Uint32  rawScore;
    Uint32  displayScore;
    Uint32  trialCount;
}TrainingData;

/**
 * @brief free allocated training data
 */
void training_free(TrainingData *dataset);

/**
 * @brief load training data from a JSON file
 * @param filename the name of the file to parse
 * @returns NULL on error or training data set otherwise
 */
TrainingData *training_load(const char *filename);

/**
 * @brief get a moment of data from the training set, by the target time index
 * @param dataset the set to search
 * @param target the target timeIndex to search for
 * @param ignore do not return a moment if the result would have this time stamp
 * @returns NULL on error, or if the target is not available and the next best would be ignore
 */
Moment *training_get_moment_by_time(TrainingData *dataset,Uint32 target,Uint32 ignore);

/**
 * @brief search the dataset for a similar moment to a search one.
 * @param dataset the set to search
 * @param world the world associated with the data set
 * @param targetFrame the current situtation being evaluated.  For the current world this is supported on.
 * @returns NULL on error or no matches found, a pointer to a moment otherwise
 */
Moment *training_get_moment_by_similar_moment(TrainingData *dataset,World *world, WorldFrame *targetFrame)

/**
 * @brief search the dataset foa similar sequence of moments
 * @param dataset the set to search
 * @param momentFrame a list of moments to match on.  The longer the list, the harder to make an exact match
 * @param timeThreshold a time range to match on.  -1 will ignore time in making a match
 * @param matchBest if true, it will return the BEST match if a perfect match is not available.  
 * @returns NULL on error or no matches found, a pointer to a moment otherwise
 */
Moment *training_get_moment_by_similar_moment_frame(TrainingData *dataset,List *momentframe,Sint64 timeThreshold,Uint8 matchBest);

/**
 * @brief iterate through the training data, returning the next moment after the last
 * @param dataset the set to iterate through
 * @param last if NULL, returns the start of the list, else returns the next moment in the set
 * @returns NULL on error or end of list, the next moment in the list otherwise.
 */
Moment *training_get_next(TrainingData *dataset,Moment *last);

#endif
