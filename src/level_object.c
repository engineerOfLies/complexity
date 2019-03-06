#include "level_object.h"
#include "simple_logger.h"

LevelObject *level_object_new()
{
    LevelObject *lo;
    lo = (LevelObject*)malloc(sizeof(LevelObject));
    if (!lo)
    {
        slog("failed to allocate memory for a level object");
        return NULL;
    }
    memset(lo,0,sizeof(LevelObject));
    return lo;
}

LevelObject *level_object_parse(SJson *json)
{
    LevelObject * lo = NULL;
    if (!json)
    {
        slog("cannot parse a null object");
        return NULL;
    }
    lo = level_object_new();
    if (!lo)return NULL;
    // PARSE STUFF
    return lo;
}

void level_object_free(LevelObject *lo)
{
    int i,count;
    char *str;
    if (!lo)return;
    
    count = gf2d_list_get_count(lo->goodActions);
    for (i = 0; i < count; i++)
    {
        str = (char*)gf2d_list_get_nth(lo->goodActions,i);
        if (!str)continue;
        free(str);
    }
    gf2d_list_delete(lo->goodActions);
    count = gf2d_list_get_count(lo->badActions);
    for (i = 0; i < count; i++)
    {
        str = (char*)gf2d_list_get_nth(lo->badActions,i);
        if (!str)continue;
        free(str);
    }
    gf2d_list_delete(lo->badActions);
    free(lo);
}

/*eol@eof*/
