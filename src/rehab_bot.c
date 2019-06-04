#include <stdlib.h>
#include "simple_json.h"
#include "simple_logger.h"
#include "world_obstacles.h"
#include "rehab_bot.h"

typedef struct
{
    TextLine    name;
    float       weight;
}ActionChoice;

enum ActionChoiceS
{
    AC_Idle  = 0,
    AC_Left  = 1,
    AC_Right = 2,
    AC_Jump  = 3,
    AC_Duck  = 4,
    AC_MAX   = 5
};

ActionChoice actionChoices[]=
{
    {
        "idle",1
    },
    {
        "left",1
    },
    {
        "right",1
    },
    {
        "jump",1
    },
    {
        "duck",1
    }  
};

void action_choices_reset()
{
    int i;
    for (i = 0; i < AC_MAX; i++)
    {
        actionChoices[i].weight = 1;
    }
}

const char *action_choice_get_best()
{
    int i;
    float bestWeight = -1;
    int bestIndex = -1;
    slog("choice weights:");
    for (i = 0; i < AC_MAX; i++)
    {
        slog("%s : %f",actionChoices[i].name,actionChoices[i].weight);
        if (actionChoices[i].weight > bestWeight)
        {
            bestIndex = i;
            bestWeight = actionChoices[i].weight;
        }
    }
    if (bestIndex == -1)return NULL;
    return actionChoices[bestIndex].name;
}

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
        free(metric);//TODO change if we make calibration metrics more complicated
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
    
    list = sj_object_get_value(json,"calibrations");
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


void rebot_save(ReBot *bot, char *filename)
{
    SJson *json = NULL;
    SJson *array = NULL;
    SJson *item = NULL;
    CMetric *metric;
    int c,n;
    if (!bot)
    {
        slog("cannot save bot training data, no bot provided");
        return;
    }
    json = sj_object_new();
    array = sj_array_new();
    if (!json)return;
    if (!array)
    {
        sj_free(json);
        return;
    }
    
    c = gf2d_list_get_count(bot->calibrations);
    for (n = 0; n < c; n++)
    {
        metric = gf2d_list_get_nth(bot->calibrations,n);
        if (!metric)continue;
        item = sj_object_new();
        if (item)continue;
        sj_object_insert(item,"name",sj_new_str(metric->name));
        sj_object_insert(item,"value",sj_new_float(metric->value));
        sj_object_insert(item,"weight",sj_new_float(metric->weight));
        sj_array_append(array,item);
    }

    sj_object_insert(json,"calibrations",array);
    sj_save(json,filename);    
    sj_free(json);
}



const char *rebot_pick_action(ReBot *bot,Uint8 position, WorldFrame *frame, List *olist)
{
    WorldObstacle *obstacle = NULL;
    CMetric *metric = NULL;
    float avoidance = 1;
    float collection = 1;
    if ((!bot)||(!frame))
    {
        slog("missing data");
        return NULL;
    }
    if (position > 2)
    {
        slog("player position out of range");
        return NULL;
    }
    action_choices_reset();
    /*
     * make list of possible actions, paired with a weight.
     * Assign -1 is the action if not possible
     * 
     */
    metric = cmetric_get_by_name(bot->calibrations,"avoidance");
    if (metric)avoidance = metric->value * metric->weight;
    
    if (position == 0)actionChoices[AC_Left].weight = -1;
    if (position == 2)actionChoices[AC_Right].weight = -1;
    
    // check current position
    slog("current position: %i",position);
    if ((position > 0) && (frame->obstacles[position -1]))
    {
        obstacle = world_obstacle_list_get_by_index(olist, frame->obstacles[position - 1]);
        if (!obstacle)
        {
            slog("failed to find obstacle with index %i",frame->obstacles[position - 1]);
            return NULL;
        }
        if (obstacle->collides & WO_Idle)
        {// moving to the left would cause a clash
            metric = cmetric_get_by_name(bot->calibrations,"idle");
            if (metric)
            {
                actionChoices[AC_Idle].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"right");
            if (metric)
            {
                actionChoices[AC_Right].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"jump");
            if (metric)
            {
                actionChoices[AC_Jump].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"duck");
            if (metric)
            {
                actionChoices[AC_Duck].weight *= metric->value *metric->weight *avoidance;
            }
        }
    }
    if ((position < 2) && (frame->obstacles[position + 1]))
    {
        obstacle = world_obstacle_list_get_by_index(olist, frame->obstacles[position + 1]);
        if (!obstacle)
        {
            slog("failed to find obstacle with index %i",frame->obstacles[position + 1]);
            return NULL;
        }
        if (obstacle->collides & WO_Idle)
        {// moving to the left would cause a clash
            metric = cmetric_get_by_name(bot->calibrations,"idle");
            if (metric)
            {
                actionChoices[AC_Idle].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"left");
            if (metric)
            {
                actionChoices[AC_Left].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"jump");
            if (metric)
            {
                actionChoices[AC_Jump].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"duck");
            if (metric)
            {
                actionChoices[AC_Duck].weight *= metric->value *metric->weight *avoidance;
            }
        }
    }
    if (frame->obstacles[position])
    {
        obstacle = world_obstacle_list_get_by_index(olist, frame->obstacles[position]);
        if (!obstacle)
        {
            slog("failed to find obstacle with index %i",frame->obstacles[position]);
            return NULL;
        }
        // check to avoid obstacles
        if (obstacle->collides & WO_Idle)
        {
            //can't remain idle, increase weights for non Idle actions
            metric = cmetric_get_by_name(bot->calibrations,"left");
            if (metric)
            {
                actionChoices[AC_Left].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"right");
            if (metric)
            {
                actionChoices[AC_Right].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"jump");
            if (metric)
            {
                actionChoices[AC_Jump].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"duck");
            if (metric)
            {
                actionChoices[AC_Duck].weight *= metric->value *metric->weight *avoidance;
            }
        }
        if (obstacle->collides & WO_Jump)
        {
            //can't remain idle, increase weights for non Idle actions
            metric = cmetric_get_by_name(bot->calibrations,"left");
            if (metric)
            {
                actionChoices[AC_Left].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"right");
            if (metric)
            {
                actionChoices[AC_Right].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"idle");
            if (metric)
            {
                actionChoices[AC_Idle].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"duck");
            if (metric)
            {
                actionChoices[AC_Duck].weight *= metric->value *metric->weight *avoidance;
            }
        }
        if (obstacle->collides & WO_Duck)
        {
            //can't remain idle, increase weights for non Idle actions
            metric = cmetric_get_by_name(bot->calibrations,"left");
            if (metric)
            {
                actionChoices[AC_Left].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"right");
            if (metric)
            {
                actionChoices[AC_Right].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"jump");
            if (metric)
            {
                actionChoices[AC_Jump].weight *= metric->value *metric->weight *avoidance;
            }
            metric = cmetric_get_by_name(bot->calibrations,"idle");
            if (metric)
            {
                actionChoices[AC_Idle].weight *= metric->value *metric->weight *avoidance;
            }
        }
    }
    return action_choice_get_best();
}

/**
 * @note: This does not yet take into account time to execute a move. still need to work that in
 * This is just roughing out the broad strokes
 * @note instead of the world, make a world similation that keeps track of player position in the world and overall score
 */
void rebot_calibrate_on_world(ReBot *bot, World *world)
{
    WorldFrame *frame;
    int c,n;
    const char *action = NULL;
    Uint8 position = 1;
    if ((!bot)||(!world))
    {
        slog("missing calibrate bot");
        return;
    }
    /*
     * For each frame in the world
     *      make a decision based on bot calibrations
     *      find associated moments from training data
     *      calculate error
     *      
     */
    c = gf2d_list_get_count(world->frames);
    for (n = 0; n < c;n++)
    {
        frame = gf2d_list_get_nth(world->frames,n);
        if (!frame)continue;
        world_frame_slog(frame);
        action = rebot_pick_action(bot,position, frame, world->obstacleList);
        slog("Chosen action : %s",action);
        if (gf2d_line_cmp(action,"left")==0)position = MAX(0,position -1);
        if (gf2d_line_cmp(action,"right")==0)position = MIN(2,position +1);
    }
}


void rebot_assign_training(ReBot *bot, TrainingData *tdata)
{
    if ((!bot)||(!tdata))
    {
        slog("missing data to make assignment");
        return;
    }
    bot->tdata = tdata;
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

CMetric *cmetric_get_by_name(List *calibrations,char *name)
{
    CMetric *metric = NULL;
    int n,c;
    c = gf2d_list_get_count(calibrations);
    for (n = 0; n < c; n++)
    {
        metric = gf2d_list_get_nth(calibrations,n);
        if (!metric)continue;
        if (gf2d_line_cmp(metric->name,name) == 0)
        {
            return metric;
        }
    }
    return NULL;
}

/*eol@eof*/
