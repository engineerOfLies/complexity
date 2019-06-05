#ifndef __TRAINING_DATA_H
#define __TRAINING_DATA_H

#include "gf2d_list.h"
#include "gf2d_text.h"
#include "world.h"

typedef struct
{
    Uint32      timeIndex;          /**<milliseconds since the session began*/
    Uint8       playerPosition;     /**<which lane the player is in*/
    Uint8       actionResult;       /**<placeholder for now, Pass / Fail? penalty? score?*/
    TextLine    actionTaken;        /**<name of action taken by the test run*/
}Moment; 

typedef struct
{
    WorldFrame  worldFrame;
    Moment      moment;
}ActionFrame;

/**
 * @brief used for calculating weights to make a choice
 */
enum ActionChoiceS
{
    AC_Idle  = 0,
    AC_Left  = 1,
    AC_Right = 2,
    AC_Jump  = 3,
    AC_Duck  = 4,
    AC_MAX   = 5
};

typedef struct
{
    TextLine    name;
    float       weight;
}ActionChoice;

typedef struct
{
    List *relatedFrames;                /**<list of related frames*/
    ActionChoice actionChoices[AC_MAX]; /**<counts for what choice the training data made given the situation*/
}AssociatedFrames;

typedef struct
{
    List   *data;                   /**<list of moments and actions taken*/
    List   *associatedData;         /**<list of associated frames*/
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
 * @brief correlate moments with their frame counterpart from the reference world.
 * @param tdata the training data to correlate
 * @param world the refernce world that the training data was associated with
 */
void training_build_associate_data(TrainingData *tdata,World *world);

/**
 * @brief search the training data for a similar situation as the frame provided
 * @param tdata the training data to search
 * @param frame the reference frame to search for
 * @return NULL on error, or the associated frame data
 */
AssociatedFrames *training_data_get_similar_associate_frame(TrainingData *tdata,WorldFrame *frame);

/**
 * @brief get a moment of data from the training set, by the target time index
 * @param dataset the set to search
 * @param target the target timeIndex to search for
 * @param ignore do not return a moment if the result would have this time stamp
 * @returns NULL on error, or if the target is not available and the next best would be ignore
 */
Moment *training_get_moment_by_time(TrainingData *dataset,Uint32 target,Uint32 ignore);

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

/**
 * @brief Reset the weights for an action choice list
 * @param actionChoices the list of actions to reset
 */
void action_choices_reset(ActionChoice *actionChoices);

/**
 * @brief grab the name of the choice with the highest weight
 * @param actionChoices the choices list to search
 * @return NULL on error, the name of the best choice otherwise
 */
const char *action_choice_get_best(ActionChoice *actionChoices);

/**@brief get a pointer to the weight for a given action choice by name
 * @param actionChoices the set of action choices to search
 * @param name the name to search for
 * @return NULL on error, a pointer to the weight otherwise
 */
float *action_choice_get_weight_by_name(ActionChoice *actionChoices, const char *name);
#endif
