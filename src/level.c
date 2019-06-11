#include <stdlib.h>
#include "simple_logger.h"
#include "simple_json.h"
#include "level.h"
#include "camera.h"
#include "level_object.h"

Level *level_new()
{
    Level *level;
    level = (Level*)malloc(sizeof(Level));
    if (!level)
    {
        slog("failed to allocate memory for a new level");
        return NULL;
    }
    memset(level,0,sizeof(Level));
    level->objectList = gf2d_list_new();
    level->roofList = gf2d_list_new();
    return level;
}

Level *level_load(const char *filename)
{
    Level *level;
    LevelObject *lo;
    SJson *file,*data,*list,*item;
    int timespan = -1;
    int i,count;
    file = sj_load(filename);
    if (!file)
    {
        slog("failed to load level file %s",filename);
        return NULL;
    }
    data = sj_object_get_value(file,"level");
    if (!data)
    {
        slog("json file (%s) missing level object!",filename);
        sj_free(file);
        return NULL;
    }
    level = level_new();
    if (!level)
    {
        sj_free(file);
        return NULL;
    }
    
    sj_get_integer_value(sj_object_get_value(data,"timespan"),&timespan);
    if (timespan >= 0)level->timespan = timespan;
    
    list = sj_object_get_value(data,"objectList");
    count = sj_array_get_count(list);
    for (i = 0; i < count; i++)
    {
        item = sj_array_get_nth(list,i);
        if (!item)continue;
        lo = level_object_parse(item);
        if (!lo)continue;
        level->objectList = gf2d_list_append(level->objectList,lo);
    }
    sj_free(file);
    return level;
}

void level_free(Level *level)
{
    int i,count;
    LevelObject *lo;
    Roof *roof;
    if (!level)return;
    
    count = gf2d_list_get_count(level->objectList);
    for (i = 0; i < count; i++)
    {
        lo = (LevelObject*)gf2d_list_get_nth(level->objectList,i);
        if (!lo)continue;
        level_object_free(lo);
    }
    gf2d_list_delete(level->objectList);
    count = gf2d_list_get_count(level->roofList);
    for (i = 0; i < count; i++)
    {
        roof = (Roof*)gf2d_list_get_nth(level->roofList,i);
        if (!roof)continue;
        rooftop_free(roof);
    }
    gf2d_list_delete(level->roofList);
    free(level);
}

void level_draw(Level *level,Vector2D offset)
{
    int i,count;
    Roof *roof;
    count = gf2d_list_get_count(level->roofList);
    for (i = 0; i < count; i++)
    {
        roof = (Roof*)gf2d_list_get_nth(level->roofList,i);
        if (!roof)continue;
        if (camera_rect_on_screen(rooftop_get_bounds(roof)))
        {
            rooftop_draw(roof,offset);
        }
    }
}

Level *level_generate(Uint32 count)
{
    int i;
    Level *level;
    level = level_new();
    if (!level)return NULL;
    
    for (i = 0; i < count; i++)
    {
        level->roofList = gf2d_list_append(level->roofList,rooftop_new_data(vector2d(300,i * -832),"images/backgrounds/rooftop1.png",vector2d(600,800)));
    }
    
    return level;
}

float level_get_lane_x(Uint8 lane)
{
    if (lane == 0)
    {
        return 400;
    }
    if (lane == 1)
    {
        return 600;
    }
    if (lane == 2)
    {
        return 800;
    }
    return 600;
}


/*eol@eof*/
