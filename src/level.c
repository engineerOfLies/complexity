#include "level.h"
#include "simple_logger.h"
#include "simple_json.h"
#include "level_object.h"
#include <stdlib.h>

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
    if (!level)return;
    
    count = gf2d_list_get_count(level->objectList);
    for (i = 0; i < count; i++)
    {
        lo = (LevelObject*)gf2d_list_get_nth(level->objectList,i);
        if (!lo)continue;
        level_object_free(lo);
    }
    gf2d_list_delete(level->objectList);
    free(level);
}

/*eol@eof*/
