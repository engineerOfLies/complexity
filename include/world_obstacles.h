#ifndef __WORLD_OBSTACLES_H__
#define __WORLD_OBSTACLES_H__

#include "gf2d_text.h"
#include "gf2d_list.h"
#include "player.h"
enum WO_COLLISIONS 
{
    WO_Idle = 1,
    WO_Jump = 2,
    WO_Duck = 4
};

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

/**
 * @brief free a list of previously loaded world obstacles
 * @param olist the list of obstacles to free
 */
void world_obstacle_list_free(List *olist);

/**
 * @brief search the world obstacle list for an obstacle by name
 * @param olist the list to search
 * @param name the name to search for
 * @returns NULL on error or not found, the WorldObstacle otherwise
 */
WorldObstacle *world_obstacle_list_get_by_name(List *olist, const char *name);

/**
 * @brief search the world obstacle list for an obstacle by its index
 * @param olist the list to search
 * @param index the index to search for
 * @returns NULL on error or not found, the WorldObstacle otherwise
 */
WorldObstacle *world_obstacle_list_get_by_index(List *olist, Uint32 index);

/**
 * @brief check if there is any overlap between the obstacle and the player state
 * @param wo the world obstacle to check against
 * @param playerstate the player current state (Idle, Duck, Jump)
 * @returns 1 if there is a collision, 0 otherwise
 */
Uint8 world_obstacle_check_collision(WorldObstacle wo,Uint8 playerstate);

#endif
