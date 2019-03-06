#ifndef __LEVEL_H__
#define __LEVEL_H__

#include "gf2d_list.h"
#include "gf2d_vector.h"
#include "level_object.h"

typedef struct
{
    List *objectList;     /**<list of obstacles in the game level*/
    Uint32 timespan;       /**<how long this level should take to complete*/
}Level;


/**
 * @brief allocate memory for a new level
 * @returns NULL on error, or a pointer to an initialized level otherwise
 */
Level *level_new();

/**
 * @brief loat a level from the file provided
 * @param filename path to the file to parse
 * @return NULL on failure, a pointer to the file loaded otherwise
 */
Level *level_load(const char *filename);

/**
 * @brief free a level from memory
 * @param level the level to free
 */
void level_free(Level *level);
#endif
