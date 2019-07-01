#include <stdlib.h>
#include <openssl/sha.h>
#include "simple_logger.h"
#include "simple_json.h"
#include "world_gen.h"
#include "world_obstacles.h"

float world_time_index_to_position(World *world, Uint32 timeIndex)
{
    if (!world)
    {
        slog("cannot calculate position for NULL world");
        return -100;
    }
    if (world->ticksPerFrame == 0)
    {
        slog("cannot calculate position for world with zero ticksPerFrame");
        return -100;
    }
    return - (timeIndex / world->ticksPerFrame);
}

WorldFrame *world_frame_from_frame_config(WorldGenFrameConfig *frameConfig, Uint32 timeIndex);

World *world_gen_create(WorldGenConfig *wgc,char *obstacleListFile,int frames)
{
    World *world = NULL;
    WorldFrame *worldFrame = NULL;
    WorldGenFrameConfig *frameConfig = NULL;
    int i,j;
    Uint32 timeIndex;
    
    if ((!wgc)||(!obstacleListFile))
    {
        slog("missing data to generate a world");
        return NULL;
    }
    
    world = world_new();
    if (!world)return NULL;
    
    gf2d_line_cpy(world->obstacleListFile,obstacleListFile);
    world->obstacleList = world_obstacle_load(world->obstacleListFile);

    world_gen_config_reset(wgc, 0);
    for (i = 0; i < frames; i++)
    {
        timeIndex = (i*wgc->ticksPerFrame);
        world_gen_config_calculate_weights(wgc, timeIndex);
        frameConfig = world_gen_pick_next_frame(wgc);
        if (frameConfig)
        {
            for (j = 0; (j <= frameConfig->repeat)&&(i < frames);j++,i++)
            {
                timeIndex = (i*wgc->ticksPerFrame);
                worldFrame = world_frame_from_frame_config(frameConfig,timeIndex);
                if (worldFrame)
                {
                    world->frames = gf2d_list_append(world->frames,worldFrame);
                    slog("adding frame of %s to the world at time Index %i",frameConfig->name,timeIndex);
                }
            }
            frameConfig->lastUsed = timeIndex;
        }
    }
    return world;
}

WorldFrame *world_frame_from_frame_config(WorldGenFrameConfig *frameConfig,Uint32 timeIndex)
{
    WorldFrame *worldFrame = NULL;
    if (!frameConfig)return NULL;
    worldFrame = world_frame_new();
    if (!worldFrame)return NULL;
        
    worldFrame->timeIndex = timeIndex;
    memcpy(worldFrame->collectables,frameConfig->collectables,sizeof(int)*FP_MAX);
    memcpy(worldFrame->obstacles,frameConfig->obstacles,sizeof(int)*FP_BMAX);
    
    return worldFrame;
}


WorldGenFrameConfig *world_gen_pick_next_frame(WorldGenConfig *wgc)
{
    WorldGenFrameConfig *conf = NULL,*best = NULL;
    float bestWeight = -1;
    float bestPriority = -1;
    int n,c;
    if (!wgc)return NULL;
    c = gf2d_list_get_count(wgc->frameTypes);
    for (n = 0; n < c; n++)
    {
        conf = gf2d_list_get_nth(wgc->frameTypes,n);
        if (!conf)continue;
        if ((conf->weight > bestWeight)
            ||((conf->weight == bestWeight)&&(conf->priority > bestPriority)))
        {
            best = conf;
            bestWeight = conf->weight;
            bestPriority = conf->priority;
        }
    }
    return best;
}

void world_gen_frame_calculate_weight(WorldGenFrameConfig *conf, Uint32 now)
{
    Uint32 diff;
    Uint32 frequency;
    if (!conf)return;
    diff = now - conf->lastUsed;
    
    
    // check lower and upper bounds
    if (now < conf->frameDelay)
    {
        conf->weight = -1;//skip
//        slog("calculated skipping %s too soon!",conf->name,conf->weight);
        return;//skip
    }
    if ((conf->frameCap > 0)&&(now > conf->frameCap))
    {
        conf->weight = -1;//skip
//        slog("Skipping %s frame cap hit",conf->name);
        return;//skip
    }
    
    //calculate function frequency
    
    frequency = conf->frequency;
    //+ ((float)now * conf->frequencyDelta);
    
//    slog("calculated frequency for %s: %i",conf->name,frequency);
    
    if (diff < frequency)
    {
        conf->weight = -1;//skip
//        slog("Skipping %s, too soon to do again",conf->name);
        return;//skip
    }
    conf->weight = diff * conf->priority;
//    slog("calculated weight for %s at %f",conf->name,conf->weight);
}

void world_gen_config_calculate_weights(WorldGenConfig *wgc, Uint32 now)
{
    WorldGenFrameConfig *conf = NULL;
    int n,c;
    if (!wgc)return;
    c = gf2d_list_get_count(wgc->frameTypes);
    for (n = 0; n < c; n++)
    {
        conf = gf2d_list_get_nth(wgc->frameTypes,n);
        if (!conf)continue;
        world_gen_frame_calculate_weight(conf,now);
    }
}


void world_gen_config_reset(WorldGenConfig *wgc, Uint32 now)
{
    WorldGenFrameConfig *conf = NULL;
    int n,c;
    if (!wgc)return;
    c = gf2d_list_get_count(wgc->frameTypes);
    for (n = 0; n < c; n++)
    {
        conf = gf2d_list_get_nth(wgc->frameTypes,n);
        if (!conf)continue;
        conf->weight = 0;
        conf->lastUsed = now;
    }
}

void world_gen_config_free(WorldGenConfig *wgc)
{
    WorldGenFrameConfig *conf = NULL;
    int n,c;
    if (!wgc)return;
    c = gf2d_list_get_count(wgc->frameTypes);
    for (n = 0; n < c; n++)
    {
        conf = gf2d_list_get_nth(wgc->frameTypes,n);
        if (!conf)continue;
        world_gen_frame_config_free(conf);
    }
    gf2d_list_delete(wgc->frameTypes);
    free(wgc);
}

WorldGenConfig *world_gen_config_new()
{
    WorldGenConfig *wgc = NULL;
    wgc = (WorldGenConfig*)malloc(sizeof(WorldGenConfig));
    if (!wgc)
    {
        slog("failed to allocate WorldGenConfig");
        return NULL;
    }
    memset(wgc,0,sizeof(WorldGenConfig));
    wgc->frameTypes = gf2d_list_new();
    return wgc;
}

WorldGenFrameConfig *world_gen_frame_config_new()
{
    WorldGenFrameConfig *wgfc = NULL;
    wgfc = (WorldGenFrameConfig*)malloc(sizeof(WorldGenFrameConfig));
    if (!wgfc)
    {
        slog("failed to allocate WorldGenFrameConfig");
        return NULL;
    }
    memset(wgfc,0,sizeof(WorldGenFrameConfig));
    return wgfc;
}

void world_gen_frame_config_free(WorldGenFrameConfig *wgfc)
{
    if (!wgfc)return;
    free(wgfc);
}

WorldGenConfig *world_gen_config_list_load(const char *filename)
{
    WorldGenConfig *wgc = NULL;
    WorldGenFrameConfig *wgfc = NULL;
    SJson *json,*array,*item;
    const char *temps= NULL;
    int c,n,i;
    int tempi;
    json = sj_load(filename);
    if (!json)
    {
        slog("failed to parse %s",filename);
        return NULL;
    }
    wgc = world_gen_config_new();
    if (!wgc)
    {
        sj_free(json);
        return NULL;
    }
    
    sj_get_float_value(sj_object_get_value(json,"ticksPerFrame"),&wgc->ticksPerFrame);
        
    if (sj_get_integer_value(sj_object_get_value(json,"randomSeed"),&tempi))
        wgc->randomSeed = (Uint32)tempi;
    temps = sj_get_string_value(sj_object_get_value(json,"seedString"));
    if (temps != NULL)gf2d_line_cpy(wgc->seedString,temps);
    
    array = sj_object_get_value(json,"frameTypes");
    if (!array)
    {
        slog("missing frameTypes from config file!");
        sj_free(json);
        world_gen_config_free(wgc);
        return NULL;
    }
    c = sj_array_get_count(array);
    for (n = 0; n < c; n++)
    {
        item = sj_array_get_nth(array,n);
        if (!item)continue;
        wgfc = world_gen_frame_config_new();
        if (!wgfc)continue;
        temps = sj_get_string_value(sj_object_get_value(item,"name"));
        if (temps)gf2d_line_cpy(wgfc->name,temps);

        for (i = 0; i < FP_BMAX;i++)
        {
            sj_get_integer_value(
                sj_array_get_nth(
                    sj_object_get_value(item,"obstacles"),i),
                    &tempi);
            wgfc->obstacles[i] = (Uint32)tempi;
        }
        for (i = 0; i < FP_MAX;i++)
        {
            sj_get_integer_value(
                sj_array_get_nth(
                    sj_object_get_value(item,"collectables"),i),&tempi);
            wgfc->collectables[i] = (Uint32)tempi;
        }
        if (sj_get_integer_value(sj_object_get_value(item,"repeat"),&tempi))
            wgfc->repeat = (Uint32)tempi;
        sj_get_float_value(sj_object_get_value(item,"frameDelay"),&wgfc->frameDelay);
        sj_get_float_value(sj_object_get_value(item,"frameCap"),&wgfc->frameCap);
        sj_get_float_value(sj_object_get_value(item,"frequency"),&wgfc->frequency);
        sj_get_float_value(sj_object_get_value(item,"frequencyDelta"),&wgfc->frequencyDelta);
        sj_get_float_value(sj_object_get_value(item,"priority"),&wgfc->priority);
        gf2d_list_append(wgc->frameTypes,wgfc);
    }
    sj_free(json);
    slog("loaded %s with %i frameTypes",filename,gf2d_list_get_count(wgc->frameTypes));
    return wgc;
}

void world_gen_set_seed_number(WorldGenConfig *worldGenConfig, Uint32 seed)
{
    TextLine seedLine = "";
    if (!worldGenConfig)return;
    snprintf(seedLine,GF2DLINELEN,"%u",seed);
    slog("seedLine: %s",seedLine);
    SHA256((const unsigned char *)seedLine,GF2DLINELEN,(unsigned char *)worldGenConfig->seedString);
    slog("generated seed string of: \"%us\"",worldGenConfig->seedString);
}

void world_gen_set_seed_string(WorldGenConfig *worldGenConfig, TextLine seedString)
{
    if (!worldGenConfig)return;
    gf2d_line_cpy(worldGenConfig->seedString,seedString);
}


/*eol@eof*/
