#include <stdlib.h>
#include "simple_logger.h"
#include "gf2d_config.h"
#include "game_actions.h"

GameAction *action_new()
{
    GameAction *action;
    action = (GameAction*)malloc(sizeof(GameAction));
    if (!action)
    {
        slog("failed to allocate memory for a new action");
        return NULL;
    }
    memset(action,0,sizeof(GameAction));
    return action;
}

GameAction *action_parse(SJson *json)
{
    GameAction *action;
    int number;
    if (!json)return NULL;
    action = action_new();
    if (!action)return NULL;
    
    gf2d_line_cpy(action->name,sj_get_string_value(sj_object_get_value(json,"name")));
    sj_get_integer_value(sj_object_get_value(json,"leadInTime"),&number);
    if (number >= 0)action->leadInTime = number;
    sj_get_integer_value(sj_object_get_value(json,"cooldownTime"),&number);
    if (number >= 0)action->cooldownTime = number;
    sj_get_float_value(sj_object_get_value(json,"difficultyWeight"),&action->difficultyWeight);
    return action;
}

void action_free(GameAction *action)
{
    if (!action)return;
    free(action);
}

GameAction *action_list_get(List *list,char *name)
{
    int i,count;
    GameAction *action;
    if (!list)return NULL;
    count = gf2d_list_get_count(list);
    for (i = 0; i < count; i++)
    {
        action = (GameAction*)gf2d_list_get_nth(list,i);
        if (!action)continue;
        if (strcmp(action->name,name)==0)
        {
            return action;
        }
    }
    return NULL;
}

void action_list_free(List *list)
{
    int i, count;
    GameAction *action;
    if (!list)return;
    count = gf2d_list_get_count(list);
    for (i = 0; i < count;i++)
    {
        action = (GameAction *)gf2d_list_get_nth(list,i);
        if (!action)continue;
        action_free(action);
    }
    gf2d_list_delete(list);
}

List *action_list_load(char *filename)
{
    int i,count;
    List *list;
    GameAction *action;
    SJson *file,*data,*item;
    file = sj_load(filename);
    if (!file)
    {
        slog("failed to parse action file %s",filename);
        return NULL;
    }
    data = sj_object_get_value(file,"actionList");
    if (!data)
    {
        slog("action list file missing actionList!");
        sj_free(file);
        return NULL;
    }
    list = gf2d_list_new();
    if (!list)
    {
        sj_free(file);
        return NULL;
    }
    count = sj_array_get_count(data);
    for (i = 0; i < count; i++)
    {
        item = sj_array_get_nth(data,i);
        if (!item)continue;
        action = action_parse(item);
        if (!action)continue;
        list = gf2d_list_append(list,action);
    }
    sj_free(file);
    return list;
}


/*eol@eof*/
