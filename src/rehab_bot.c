#include "rehab_bot.h"
#include "simple_json.h"
#include "simple_logger.h"
#include <stdlib.h>

void rebot_free(ReBot *bot)
{
    CMetric *metric;
    int count,n;
    if (!bot)return;
    
    count = gf2d_list_get_count(bot->calibrations);
    for (n = 0; n < count; n++)
    {
        metric = gf2d_list_get_nth(bot->calibrations,n);
        if (!metric)continue;
        free(metric);//TODO change if we make calibtration metrics more complicated
    }
    gf2d_list_delete(bot->calibrations);
    free(bot);
}

ReBot *rebot_new()
{
    ReBot *bot = NULL;
    bot = (ReBot *)malloc(sizeof(ReBot));
    if (!bot)
    {
        slog("failed to allocate new rehab bot");
        return NULL;
    }
    memset(bot,0,sizeof(ReBot));
    bot->calibrations = gf2d_list_new();
    return bot;
}

ReBot *rebot_load(const char *filename)
{
    SJson *json,*list,*item;
    CMetric *metric;
    ReBot *bot = NULL;
    int count, n;
    
    json = sj_load(filename);
    if (!json)return NULL;
    
    list = sj_object_get_value(json,"calibtrations");
    if (!list)
    {
        slog("bot file missing calibrations");
        sj_free(json);
        return NULL;
    }
    
    bot = rebot_new();
    if (!bot)
    {
        sj_free(json);
        return NULL;
    }
    
    count = sj_array_get_count(list);
    for (n = 0; n < count; n++)
    {
        item = sj_array_get_nth(list,n);
        if (!item)continue;
        metric = cmetric_new();
        if (!metric)continue;
        sj_get_float_value(sj_object_get_value(item,"value"),&metric->value);
        sj_get_float_value(sj_object_get_value(item,"weight"),&metric->weight);
        gf2d_line_cpy(metric->name,sj_get_string_value(sj_object_get_value(item,"name")));
        bot->calibrations = gf2d_list_append(bot->calibrations,metric);
    }
    slog("%i metrics loaded for bot",gf2d_list_get_count(bot->calibrations));
    sj_free(json);
    return bot;
}


void rebot_save(ReBot *bot, const char *filename);
const char *rebot_get_next_action(ReBot *bot, World *world, Uint32 w_position, TrainingData *dataset)
{
    /*
     World provided is the world doing work on.
     filter potential actions by world state (ie: cannot move left in the left lane, or right in the right lane.
     for each bot metric
        populate a weight for one of the potential actions
     Pick the action with the highest score
     
     Check
     
     */
    return NULL;
}

CMetric *cmetric_new()
{
    CMetric *metric;
    metric = (CMetric*)malloc(sizeof(CMetric));
    if (!metric)
    {
        slog("failed to allocate new CMetric");
        return NULL;
    }
    memset(metric,0,sizeof(CMetric));
    return metric;
}


/*eol@eof*/
