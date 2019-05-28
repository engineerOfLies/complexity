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
    Uint32  collectibles[6];/**<contents of collectibles slot, index by the above enum*/
    Uint32  obstacles[3];   /**<contents of the obstacles slot, index by the bottom */
}WorldFrame;

typedef struct
{
    List *frames;           /**<A stream of moments that make up the game world*/
    float scrollSpeed;     /**<how fast the world moves at the player.  This affects reaction time windows*/
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

#endif
