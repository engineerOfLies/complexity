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
    return level;
}

Level *level_load(const char *filename)
{
    Level *level;
    SJson *file;
    file = sj_load(filename);
    if (!file)
    {
        slog("failed to load level file %s",filename);
        return NULL;
    }
    level = level_new();
    if (!level)return NULL;
    // parse
    
    
    sj_free(file);
    return level;
}

void level_free(Level *level)
{
    int i,count;
    LevelObject *lo;
    if (!level)return;
    
    count = gf2d_list_get_count(level->obstacleList);
    for (i = 0; i < count; i++)
    {
        lo = (LevelObject*)gf2d_list_get_nth(level->obstacleList,i);
        if (!lo)continue;
        level_object_free(lo);
    }
    gf2d_list_delete(level->obstacleList);
    free(level);
}

/*eol@eof*/
