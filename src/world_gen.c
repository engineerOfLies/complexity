#include <stdlib.h>
#include <openssl/sha.h>
#include "simple_logger.h"
#include "simple_json.h"
#include "world_gen.h"


void world_get_frame_calculate_weight(WorldGenFrameConfig *conf, Uint32 now)
{
    Uint32 diff;
    Uint32 frequency;
    if (!conf)return;
    diff = now - conf->lastUsed;
    
    // check lower and upper bounds
    if (diff < conf->frameDelay)
    {
        conf->weight = -1;//skip
        slog("calculated weight for %s at %f",conf->name,conf->weight);
        return;//skip
    }
    if ((conf->frameCap > 0)&&(diff > conf->frameCap))
    {
        conf->weight = -1;//skip
        slog("calculated weight for %s at %f",conf->name,conf->weight);
        return;//skip
    }
    frequency = (float)conf->frequency - ((float)diff * conf->frequencyDelta);
    conf->weight = conf->priority *diff + conf->priority * frequency * 0.00001;
    slog("calculated weight for %s at %f",conf->name,conf->weight);
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
        world_get_frame_calculate_weight(conf,now);
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
