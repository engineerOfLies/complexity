#ifndef __WORLD_H__
#define __WORLD_H__

#include "rooftop.h"
#include "gf2d_list.h"
#include "gf2d_text.h"

enum FramePositions
{
    FP_BL = 0,
    FP_BC,
    FP_BR,
    FP_UL,
    FP_UC,
    FP_UR
};

typedef struct
{
    Uint32  timeIndex;      /**<milliseconds from level session start*/
    int     collectibles[6];/**<contents of collectibles slot, index by the above enum*/
    int     obstacles[3];   /**<contents of the obstacles slot, index by the bottom */
}WorldFrame;

typedef struct
{
    List *obstacleList;     /**<obstacle definitions for the world*/
    List *frames;           /**<A stream of moments that make up the game world*/
    float scrollSpeed;      /**<how fast the world moves at the player.  This affects reaction time windows*/
}World;

/**
 * @brief free a previously allocated world
 * @param world the world to free
 */
void world_free(World *world);

/**
 * @brief allocate and initialize a world
 * @returns NULL on error, or a pointer to a world
 */
World *world_new();

/**
 * @brief convert a list of rooftops into a world
 * @param rooftops a list of rooftops
 * @return NULL on error, or a world loaded with the data from the rooftops
 */
World *world_from_rooftop_list(List *rooftops);

/**
 * @brief load a JSON file with world data
 */
World *world_from_file(const char *filename);

/**
 * @brief free a world frame
 * @param wf the frame to free
 */
void world_frame_free(WorldFrame *wf);

/**
 * @brief allocate and initlialize a new world frame
 * @returns NULL on error or a new, zeroed WorldFrame
 */
WorldFrame *world_frame_new();

/**
 * @brief compare two world frames for similarity
 * @param a one frame
 * @param b another frame
 * @returns 1 if identical (excluding time stamp), 0 if no similarity, something in between for some level of similarity
 */
float world_frame_compare(WorldFrame *a, WorldFrame *b);

/**
 * @brief search for the nearest world frame by time index
 * @param world the world to search through
 * @param timeIndex the search target
 * @param threshold the +/- range from target that can yield a result
 * @note if threshold is too high, you may end up with infinite loops!
 * @param ignore  ignore this time index.
 * @returns NULL on error or no results found, a world frame otherwise
 */
WorldFrame *world_frame_get_by_time(World *world, Uint32 timeIndex, Uint32 threshold, Uint32 ignore);

/**
 * @brief print to logs the state of the frame
 * @param frame the frame to print
 */
void world_frame_slog(WorldFrame *frame);

#endif
