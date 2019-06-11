#include <stdlib.h>
#include "simple_logger.h"
#include "level_object.h"
#include "gf2d_config.h"

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
    lo->goodActions = gf2d_list_new();
    lo->badActions = gf2d_list_new();
    return lo;
}

List *level_object_parse_action_list(List *list, SJson *json)
{
    int i,count;
    SJson *item;
    if ((!list)||(!json))return list;
    count = sj_array_get_count(json);
    for (i = 0; i < count; i++)
    {
        item = sj_array_get_nth(json,i);
        if (!item)continue;
        list = gf2d_list_append(list,strdup(sj_get_string_value(item)));
    }
    return list;
}

LevelObject *level_object_parse(SJson *json)
{
    int number;
    LevelObject * lo = NULL;
    if (!json)
    {
        slog("cannot parse a null object");
        return NULL;
    }
    lo = level_object_new();
    if (!lo)return NULL;
    
    sj_get_integer_value(sj_object_get_value(json,"timetamp"),&number);
    if (number >= 0)lo->timestamp = number;
    sj_value_as_vector3d(json,&lo->position);
    sj_get_integer_value(sj_object_get_value(json,"collectable"),&number);
    if (number >= 0)lo->collectable = number;
    sj_get_float_value(sj_object_get_value(json,"scoreAdjust"),&lo->scoreAdjust);
    sj_get_float_value(sj_object_get_value(json,"avoidanceAdjust"),&lo->avoidanceAdjust);
    lo->goodActions = level_object_parse_action_list(lo->goodActions, sj_object_get_value(json,"goodActions"));
    lo->badActions = level_object_parse_action_list(lo->badActions, sj_object_get_value(json,"badActions"));
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
