#include <stdlib.h>
#include "simple_json.h"
#include "simple_logger.h"
#include "world.h"

WorldFrame *world_frame_get_by_time(World *world, Uint32 timeIndex, Uint32 threshold, Uint32 ignore)
{
    WorldFrame *wf = NULL;
    int count,n;
    if (!world)return NULL;
    count = gf2d_list_get_count(world->frames);
    for (n = 0; n < count;n++)
    {
        wf = gf2d_list_get_nth(world->frames,n);
        if (!wf)continue;
        if (wf->timeIndex == ignore)continue;
        if ((wf->timeIndex >= timeIndex - threshold)&&(wf->timeIndex <= timeIndex + threshold))return wf;
    }
    return NULL;
}

float world_frame_compare(WorldFrame *a, WorldFrame *b)
{
    float numerator = 0, denominator;
    int i;
    if ((!a)||(!b))
    {
        slog("missing world frame to compare");
        return 0;
    }
    denominator = 9.0;
    for (i = 0; i < 3;i++)
    {
        if (a->obstacles[i] == b->obstacles[i])
        {
            numerator++;
        }
    }
    for (i = 0; i < 6;i++)
    {
        if (a->collectibles[i] == b->collectibles[i])
        {
            numerator++;
        }
    }
    slog("comparing time index %i to index %i and found %f common elements with %f similarity",a->timeIndex,b->timeIndex,numerator, numerator/denominator);
    return numerator/denominator;
}

void world_free(World *world)
{
    int count,n;
    WorldFrame *wf;
    if (!world)return;
    count = gf2d_list_get_count(world->frames);
    for (n = 0; n < count; n++)
    {
        wf = gf2d_list_get_nth(world->frames,n);
        if (!wf)continue;
        world_frame_free(wf);
    }
    gf2d_list_delete(world->frames);
    free(world);
}

World *world_new()
{
    World *world;
    world = (World*)malloc(sizeof(World));
    if (!world)
    {
        slog("failed to allocate a new world");
        return NULL;
    }
    memset(world,0,sizeof(World));
    world->frames = gf2d_list_new();
    return world;
}

void world_frame_free(WorldFrame *wf)
{
    if (!wf)return;
    free(wf);
}

WorldFrame *world_frame_new()
{
    WorldFrame *wf;
    wf = (WorldFrame*)malloc(sizeof(WorldFrame));
    if (!wf)
    {
        slog("failed to allocate world frame");
        return NULL;
    }
    memset(wf,0,sizeof(WorldFrame));
    return wf;
}

WorldFrame *world_get_next_frame(World *world,WorldFrame *target,Uint32 timeThreshold)
{
    int count,n;
    WorldFrame *frame = NULL;
    if (!world)return NULL;
    if (!target)return gf2d_list_get_nth(world->frames,0);
    
    count = gf2d_list_get_count(world->frames);
    for (n = 0; n < count; n++)
    {
        frame = gf2d_list_get_nth(world->frames,n);
        if (!frame)continue;
        if (frame == target)
        {
            frame = gf2d_list_get_nth(world->frames,n+1);
            if (frame->timeIndex > target->timeIndex + timeThreshold)return NULL;
            return frame;
        }
        return frame;
    }
    return NULL;
}

WorldFrame *world_get_frame_by_time(World *world,Uint32 timeIndex,Uint32 Threshold, Uint32 ignore)
{
    int count,n;
    WorldFrame *frame = NULL;
    if (!world)return NULL;
    count = gf2d_list_get_count(world->frames);
    for (n = 0; n < count; n++)
    {
        frame = gf2d_list_get_nth(world->frames,n);
        if (!frame)continue;
        if (frame->timeIndex < (timeIndex-Threshold))continue;
        if (frame->timeIndex == ignore)continue;
        if (frame->timeIndex > (timeIndex+Threshold))return NULL;//gone to far, abort
        return frame;
    }
    return NULL;
}

World *world_from_file(const char *filename)
{
    SJson *json,*list,*item;
    World *world = NULL;
    WorldFrame *wf = NULL;
    int count, n;
    int i;
    int tempi;
    
    json = sj_load(filename);
    if (!json)return NULL;
    
    world = world_new();
    if (!world)
    {
        sj_free(json);
        return NULL;
    }
    sj_get_float_value(sj_object_get_value(json,"scrollSpeed"),&world->scrollSpeed);
    list = sj_object_get_value(json,"frames");
    if (!list)
    {
        slog("world missing frames data");
        world_free(world);
        sj_free(json);
        return NULL;
    }
    count = sj_array_get_count(list);
    for (n = 0; n < count; n++)
    {
        item = sj_array_get_nth(list,n);
        if (!item)continue;
        wf = world_frame_new();
        sj_get_integer_value(sj_object_get_value(item,"timeIndex"),&tempi);
        wf->timeIndex = (Uint32)tempi;
        for (i = 0; i < 6;i++)
        {
            sj_get_integer_value(
                sj_array_get_nth(
                    sj_object_get_value(item,"collectibles"),
                                        i),
                    &wf->collectibles[i]);
        }
        for (i = 0; i < 3;i++)
        {
            sj_get_integer_value(
                sj_array_get_nth(
                    sj_object_get_value(item,"obstacles"),
                                        i),
                    &wf->obstacles[i]);
        }
        world->frames = gf2d_list_append(world->frames,wf);
    }
    sj_free(json);
    slog("loaded a world with %i frames",gf2d_list_get_count(world->frames));
    return world;
}

/*eol@eof*/
