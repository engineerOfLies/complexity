#ifndef __WORLD_OBSTACLES_H__
#define __WORLD_OBSTACLES_H__

#include "gf2d_text.h"
#include "gf2d_line.h"
#include "player.h"

typedef struct
{
    TextLine    name;
    Uint32      index;      /**<universal id index for the obstacle*/
    Uint8       collides;   /**<mask of what statis the player will collide with it*/
}WorldObstacle;

/**
 * @brief cleanup
 * @param wo the obstacle to free
 */
void world_obstacle_free(WorldObstacle *wo);

/**
 * @brief allocate and initialize a new world obstacle
 * @returns NULL on memory error, otherwise a new, clear WO
 */
WorldObstacle *world_obstacle_new();

/**
 * @brief parse a list of data from JSON file
 * @param filename the file to parse
 * @returns NULL on error, or a list of obstacles available for the world
 */
List *world_obstacle_load(const char *filename);
#endif
