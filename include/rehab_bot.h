#ifndef __REHAB_BOT_H__
#define __REHAB_BOT_H__

#include "gf2d_list.h"
#include "gf2d_text.h"
#include "world.h"
#include "training_data.h"

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
void rebot_save(ReBot *bot, const char *filename);

/**
 * @brief get the next action to take given the current state of the world
 */
const char *rebot_get_next_action(ReBot *bot, World *world, Uint32 w_position, TrainingData *dataset);

CMetric *cmetric_new();


#endif
