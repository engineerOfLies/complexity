#include "world_obstacles.h"
#include "simple_json.h"
#include "simple_logger.h"
#include <stdlib.h>

Uint8 world_obstacle_check_collision(WorldObstacle wo,Uint8 playerstate)
{
    if (wo.collides & playerstate)return 1;
    return 0;
}

void world_obstacle_free(WorldObstacle *wo)
{
    if (!wo)return;
    free(wo);
}

WorldObstacle *world_obstacle_new()
{
    WorldObstacle *wo = NULL;
    wo = (WorldObstacle *)malloc(sizeof(WorldObstacle));
    if (!wo)
    {
        slog("failed to allocate new WorldObstacle");
        return NULL;
    }
    memset(wo,0,sizeof(WorldObstacle));
    return wo;
}

WorldObstacle *world_obstacle_list_get_by_index(List *olist, Uint32 index)
{
    int count,n;
    WorldObstacle *wo;
    if (!olist)return NULL;
    count = gf2d_list_get_count(olist);
    for (n = 0; n < count;n++)
    {
        wo = gf2d_list_get_nth(olist,n);
        if (!wo)continue;
        if (index == wo->index)return wo;
    }
    return NULL;
}

WorldObstacle *world_obstacle_list_get_by_name(List *olist, const char *name)
{
    int count,n;
    WorldObstacle *wo;
    if (!olist)return NULL;
    count = gf2d_list_get_count(olist);
    for (n = 0; n < count;n++)
    {
        wo = gf2d_list_get_nth(olist,n);
        if (!wo)continue;
        if (gf2d_line_cmp(wo->name,name) == 0)return wo;
    }
    return NULL;
}

void world_obstacle_list_free(List *olist)
{
    int count,n;
    WorldObstacle *wo;
    if (!olist)return;
    count = gf2d_list_get_count(olist);
    for (n = 0; n < count;n++)
    {
        wo = gf2d_list_get_nth(olist,n);
        if (!wo)continue;
        world_obstacle_free(wo);
    }
    gf2d_list_delete(olist);
}

List *world_obstacle_load(const char *filename)
{
    List *olist = NULL;
    SJson *json,*jlist,*item;
    int count,n;
    WorldObstacle *wo;
    int temp = 0;
    
    json = sj_load(filename);
    if (!json)return NULL;
    
    jlist = sj_object_get_value(json,"obstacle_list");
    if (!jlist)
    {
        slog("missing vital list");
        sj_free(json);
        return NULL;
    }
    
    olist = gf2d_list_new();
    if (!olist)return NULL;
    count = sj_array_get_count(jlist);
    for (n = 0; n < count; n++)
    {
        item = sj_array_get_nth(jlist,n);
        if (!item)continue;
        wo = world_obstacle_new();
        if (!wo)continue;
        gf2d_line_cpy(wo->name,sj_get_string_value(sj_object_get_value(item,"name")));
        sj_get_integer_value(sj_object_get_value(item,"index"),&temp);
        wo->index = (Uint32)temp;
        sj_get_integer_value(sj_object_get_value(item,"collides"),&temp);
        wo->collides= (Uint8)temp;
        olist = gf2d_list_append(olist,wo);
    }
    slog("loaded %i world obstacles",gf2d_list_get_count(olist));
    sj_free(json);
    return olist;
}

/*eol@eof*/
