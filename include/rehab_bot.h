#ifndef __REHAB_BOT_H__
#define __REHAB_BOT_H__

#include "gf2d_list.h"
#include "gf2d_text.h"
#include "training_data.h"
#include "world.h"


typedef struct
{
    TextLine name;
    float    value;
    float    weight;
}CMetric;

typedef struct ReBot_s
{
    List *calibrations;      /**<tuples of calibrations and weights*/
    TrainingData *tdata;    /**<pointer to training data for validation*/
}ReBot;

/**
 * @brief free a previously allocated bot
 * @param bot the bot to free
 */
void rebot_free(ReBot *bot);

/**
 * @brief allocate and initialize a rebot
 * @returns NULL on error or a pointer to an initliazed rebot
 */
ReBot *rebot_new();

/**
 * @brief load the calibrations for a bot from file
 * @param filename the JSON file containing the data to load
 * @returns NULL on error or a newly initialized and configured bot
 */
ReBot *rebot_load(const char *filename);

/**
 * @brief save a bot calibrations to file in JSON format
 * @param bot the bot to save
 * @param filename the file to save the calibrations to
 */
void rebot_save(ReBot *bot, char *filename);

/**
 * @brief assign a training data set to a bot
 * @param bot the bot to assign data to
 * @param tdata the training data to calibrate to
 */
void rebot_assign_training(ReBot *bot, TrainingData *tdata);

/**
 * @brief calibrate a bot based on the world provided
 * @note bot should have training data assigned already
 * @param bot the bot to train
 * @param world the world to train the bot on
 */
void rebot_calibrate_on_world(ReBot *bot, World *world);

/**
 * @brief allocate and inilialize a new calibration metric
 * @returns NULL on memory error, or a new CMetric otherwise
 */
CMetric *cmetric_new();

/**
 * @brief get the metric data from the calibration set by name
 * @param calibrations training calibrations to search
 * @param name the name to find
 * @returns NULL on error or no results, a pointer to the metric otherwise
 */
CMetric *cmetric_get_by_name(List *calibrations,char *name);

#endif
