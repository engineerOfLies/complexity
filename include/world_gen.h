#ifndef __WORLD_GEN_H__
#define __WORLD_GEN_H__

#include "world.h"

typedef struct
{
    Uint8 obstacles[FP_BMAX];   /**<corresponds to each ‘lane’ Index into obstacle definitions, with zero being no obstacle*/
    Uint8 Collectables[FP_MAX]; /**<corresponds to each possible position (bottom, then top)  Index into Collectables definition, with zero being empty*/
    float frameDelay;           /**<don’t use until at least this many frames*/
    float frameCap;             /**<stop using after this many frames*/
    float frequency;            /**<how often this frame should come up*/
    float frequencyDelta;       /**<how frequency should change over time*/
    float priority;             /**<tie breaker*/
    float weight;               /**<calculated*/
}WorldGenFrameConfig;

typedef struct
{
    List       *worldGenFrameConfigList;
    Uint32      randomSeed;
    TextLine    seedString;
}WorldGenConfig;

/**
 * @brief allocate and initialize a new world gen configuration
 * @return NULL on error, or a new zero'd WorldGenFrameConfig
 */
WorldGenFrameConfig *world_gen_frame_config_new();

/**
 * @brief free an allocated WorldGenFrameConfig
 * @param wgc the config to free
 */
void world_gen_frame_config_free(WorldGenFrameConfig *wgfc);

/**
 * @brief load a list of world generation configuration from file used to generate a world
 * @param filename the file to load
 * @returns Null on error or a loaded WorldGenConfig
 */
WorldGenConfig *world_gen_config_list_load(const char *filename);

/**
 * @brief free a loaded list of world gen configurations
 * @param worldGenConfigs the world gen to free
 */
void world_gen_config_list_free(WorldGenConfig *worldGenConfigs);

/**
 * @brief generate a seed string for the worldGenConfig provided
 * @param WorldGenConfig the worldGenConfig to set the seed for
 * @param seed the number used to generate the seed string
 */
void world_gen_set_seed_number(WorldGenConfig *worldGenConfig, Uint32 seed);

/**
 * @brief set the seed string for a worldGenConfig 
 * @param worldGenConfig the WGC to set the seed string for
 * @param seedString the seedString to use for the world
 */
void world_gen_set_seed_string(WorldGenConfig *worldGenConfigs, TextLine seedString);


#endif
