#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "gf2d_text.h"
#include "gf2d_list.h"
#include "simple_json.h"

typedef struct
{
    TextLine name;              /**<the unique name for an action (Jump, duck, speed_up, slow_down, etc)*/
    Uint32   leadInTime;        /**<how much time it takes before this action takes effect*/
    Uint32   cooldownTime;      /**<how much time before the player can take a follow up action*/
    float    difficultyWeight;  /**<if there is a weight to be evaluated with the completion of this action*/
}Action;

/**
 * @brief allocate and initialize an action
 * @returns NULL on error or a pointer to a new action
 */
Action *action_new();

/**
 * @brief parse a json object into an action
 * @param json the json objec to parse
 * @returns NULL on error or a parsed action
 */
Action *action_parse(SJson *json);

/**
 * @brief free an action;
 */
void action_free(Action *action);

/**
 * @brief load a list of actions from a json file
 * @param filename the json file to parse
 * @return NULL on error, a list filled with parsed actions
 */
List *action_list_load(char *filename);

/**
 * @brief free an action list previously loaded
 * @param list the list to free
 */
void action_list_free(List *list);

/**
 * @brief search for an action by name in an action list
 * @param list the list to search
 * @param name the search key
 * @returns NULL on error or not found, a pointer to the action data otherwise
 */
Action *action_list_get(List *list,char *name);

#endif
