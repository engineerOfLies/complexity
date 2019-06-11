#include <stdlib.h>
#include "simple_json.h"
#include "simple_logger.h"
#include "training_data.h"

void action_choices_reset(ActionChoice *actionChoices)
{
    static ActionChoice baseActionChoices[]=
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

    if (!actionChoices)return;
    memcpy(actionChoices,baseActionChoices,sizeof(ActionChoice)*AC_MAX);
}

const char *action_choice_get_best(ActionChoice *actionChoices)
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

float *action_choice_get_weight_by_name(ActionChoice *actionChoices, const char *name)
{
    int i;
    if ((!actionChoices)||(!name))
    {
        slog("Missing data");
        return NULL;
    }
    for (i = 0; i < AC_MAX; i++)
    {
        if (gf2d_line_cmp(actionChoices[i].name,name) == 0)
        {
            return &actionChoices[i].weight;
        }
    }
    return NULL;
}

/**
 * build training database consisting of a set of moments, all with the same or similar setups
 * Build a moment association list (list of lists)
 * For each moment check if it matches an existing moment, if so, then add it to the associated list
 * else append it to a new entry
 */

ActionFrame *action_frame_new()
{
    ActionFrame *af;
    af = (ActionFrame*)malloc(sizeof(ActionFrame));
    if (!af)
    {
        slog("failed to allocate ActionFrame");
        return NULL;
    }
    memset(af,0,sizeof(ActionFrame));
    return af;
}

void associated_frames_free(AssociatedFrames *af)
{
    int c,i;
    ActionFrame *action;
    if (!af)return;
    c = gf2d_list_get_count(af->relatedFrames);
    for (i = 0; i < c;i++)
    {
        action = gf2d_list_get_nth(af->relatedFrames,i);
        if (!action)continue;
        free(action);
    }
    gf2d_list_delete(af->relatedFrames);
}

AssociatedFrames *training_data_get_similar_associate_frame(TrainingData *tdata,WorldFrame *frame)
{
    AssociatedFrames *aFrame = NULL;
    ActionFrame *actionFrame = NULL;
    int c,n,i,ci;
    if ((!tdata)||(!frame))
    {
        slog("missing data to find associatedFrames");
        return NULL;
    }
    c = gf2d_list_get_count(tdata->associatedData);
    for (n = 0; n < c; n++)
    {
        aFrame = gf2d_list_get_nth(tdata->associatedData,n);
        if (!aFrame)continue;
        ci = gf2d_list_get_count(aFrame->relatedFrames);
        for (i = 0; i < ci;i++)
        {
            actionFrame = gf2d_list_get_nth(aFrame->relatedFrames,i);
            if (!actionFrame)continue;
            if (world_frame_compare(&actionFrame->worldFrame, frame))
            {
                return aFrame;
            }
        }
    }
    return NULL;
}

AssociatedFrames *associated_frames_new()
{
    AssociatedFrames *af;
    af = (AssociatedFrames*)malloc(sizeof(AssociatedFrames));
    if (!af)
    {
        slog("failed to allocate AssociatedFrames");
        return NULL;
    }
    memset(af,0,sizeof(AssociatedFrames));
    af->relatedFrames = gf2d_list_new();
    return af;
}

ActionFrame *action_frame_build(Moment *moment, WorldFrame *wf)
{
    ActionFrame *af = NULL;
    if ((!moment) || (!wf))return NULL;
    
    af = action_frame_new();
    if (!af)return NULL;
    
    memcpy(&af->moment,moment,sizeof(Moment));
    memcpy(&af->worldFrame,wf,sizeof(WorldFrame));
    return af;
}

//TODO; expand this to include a variable sized window of future events

AssociatedFrames *training_associated_data_get_similar(List *associatedData,WorldFrame *wf,float threshold)
{
    int count,n;
    int fcount,i;
    AssociatedFrames *af;
    ActionFrame *action;
    if ((!associatedData)||(!wf))return NULL;
    count = gf2d_list_get_count(associatedData);
    for (n = 0; n < count; n++)
    {
        af = gf2d_list_get_nth(associatedData,n);
        if (!af)continue;
        fcount = gf2d_list_get_count(af->relatedFrames);
        for (i = 0; i < fcount; i++)
        {
            action = gf2d_list_get_nth(af->relatedFrames,i);
            if (!action)continue;
            if (world_frame_compare(&action->worldFrame, wf) >= threshold)
            {
                return af;
            }
        }
    }
    return NULL;
}

void training_associated_data_slog(TrainingData *tdata)
{
    int count,n,i;
    AssociatedFrames *frame;
    if (!tdata)return;
    count = gf2d_list_get_count(tdata->associatedData);
    for (n = 0; n < count; n++)
    {
        frame = gf2d_list_get_nth(tdata->associatedData,n);
        if (!frame)continue;
        slog("associate frame %i choice weights:",n);
        for (i = 0; i < AC_MAX; i++)
        {
            slog("%s : %f",frame->actionChoices[i].name,frame->actionChoices[i].weight);
        }
    }
}

void training_build_associate_data(TrainingData *tdata,World *world)
{
    ActionFrame *af;
    WorldFrame *wf;
    AssociatedFrames *associatedFrame;
    Moment *moment;
    int count,n;
    float *weight;
    
    if ((!tdata)||(!world))return;
    
        /*for each moment....
         *  get associated world frame
         *      check if similar frame exists in the associatedData already
         *          if so, insert action frame there
         *          else append a new entry with this as the head.
         * */
    count = gf2d_list_get_count(tdata->data);
    for  (n = 0; n < count; n++)
    {
        moment = gf2d_list_get_nth(tdata->data,n);
        if (!moment)continue;
        wf = world_frame_get_by_time(world, moment->timeIndex, 1, moment->timeIndex - 1);
        if (!wf)
        {
            slog("failed to find a world frame to match index %i",moment->timeIndex);
            continue;
        }
        af = action_frame_build(moment, wf);
        associatedFrame = training_associated_data_get_similar(tdata->associatedData,wf,0.9);
        if (associatedFrame != NULL)
        {
            // insert action frame here
            weight = action_choice_get_weight_by_name(associatedFrame->actionChoices, moment->actionTaken);
            if (weight)*weight += 1;
            associatedFrame->relatedFrames = gf2d_list_append(associatedFrame->relatedFrames,af);
        }
        else
        {
            //append a new entry with this as the head
            associatedFrame = associated_frames_new();
            if (!associatedFrame)
            {
                free(af);
                continue;
            }
            action_choices_reset(associatedFrame->actionChoices);
            weight = action_choice_get_weight_by_name(associatedFrame->actionChoices, moment->actionTaken);
            if (weight)*weight += 1;
            
            associatedFrame->relatedFrames = gf2d_list_append(associatedFrame->relatedFrames,af); 
            tdata->associatedData = gf2d_list_append(tdata->associatedData,associatedFrame);
        }
    }
    slog("built associated frame data with %i sets of associated moments",gf2d_list_get_count(tdata->associatedData));
    training_associated_data_slog(tdata);
}

Moment *moment_new()
{
    Moment *data;
    data = (Moment*)malloc(sizeof(Moment));
    if (!data)
    {
        slog("failed to allocate training data moment");
        return NULL;
    }
    memset(data,0,sizeof(Moment));
    return data;
}


TrainingData *training_new_set()
{
    TrainingData *data;
    data = (TrainingData*)malloc(sizeof(TrainingData));
    if (!data)
    {
        slog("failed to allocate training data");
        return NULL;
    }
    memset(data,0,sizeof(TrainingData));
    data->data = gf2d_list_new();
    data->associatedData = gf2d_list_new();
    return data;
}

void training_free(TrainingData *dataset)
{
    Moment *moment;
    AssociatedFrames *af;
    int count,n;
    if (!dataset)return;
    
    count = gf2d_list_get_count(dataset->data);
    for (n = 0; n < count; n++)
    {
        moment = gf2d_list_get_nth(dataset->data,n);
        if (!moment)continue;
        free(moment);//TODO change if we make moments more complicated
    }
    gf2d_list_delete(dataset->data);

    count = gf2d_list_get_count(dataset->associatedData);
    for (n = 0; n < count; n++)
    {
        af = gf2d_list_get_nth(dataset->associatedData,n);
        if (!af)continue;
        associated_frames_free(af);//TODO change if we make moments more complicated
    }
    gf2d_list_delete(dataset->associatedData);

    free(dataset);
}

TrainingData *training_load(const char *filename)
{
    SJson *json,*list,*item;
    Moment *moment;
    TrainingData *data = NULL;
    int count, n;
    int tempi = 0;
    
    json = sj_load(filename);
    if (!json)return NULL;
    
    list = sj_object_get_value(json,"moments");
    if (!list)
    {
        slog("bot file missing calibrations");
        sj_free(json);
        return NULL;
    }
    
    data = training_new_set();
    if (!data)
    {
        sj_free(json);
        return NULL;
    }
    
    if (sj_get_integer_value(sj_object_get_value(json,"sessionLength"),&tempi))
        data->sessionLength = (Uint32)tempi;
    if (sj_get_integer_value(sj_object_get_value(json,"obstaclesAvoided"),&tempi))
        data->obstaclesAvoided = (Uint32)tempi;
    if (sj_get_integer_value(sj_object_get_value(json,"collectablesObtained"),&tempi))
        data->collectablesObtained = (Uint32)tempi;
    if (sj_get_integer_value(sj_object_get_value(json,"rawScore"),&tempi))
        data->rawScore = (Uint32)tempi;
    if (sj_get_integer_value(sj_object_get_value(json,"displayScore"),&tempi))
        data->displayScore = (Uint32)tempi;
    if (sj_get_integer_value(sj_object_get_value(json,"trialCount"),&tempi))
        data->trialCount = (Uint32)tempi;
    
    count = sj_array_get_count(list);
    for (n = 0; n < count; n++)
    {
        item = sj_array_get_nth(list,n);
        if (!item)continue;
        moment = moment_new();
        if (!moment)continue;
        if (sj_get_integer_value(sj_object_get_value(item,"timeIndex"),&tempi))
            moment->timeIndex = (Uint32)tempi;
        if (sj_get_integer_value(sj_object_get_value(item,"playerPosition"),&tempi))
            moment->playerPosition = (Uint8)tempi;
        gf2d_line_cpy(moment->actionTaken,sj_get_string_value(sj_object_get_value(item,"actionTaken")));
        data->data = gf2d_list_append(data->data,moment);
    }
    slog("%i moments loaded for training data",gf2d_list_get_count(data->data));
    sj_free(json);
    return data;
}

Moment *training_get_moment_by_time(TrainingData *dataset,Uint32 target,Uint32 ignore)
{
    int count,n;
    Moment *moment;
    if (!dataset)return NULL;
    count = gf2d_list_get_count(dataset->data);
    for (n = 0; n < count; n++)
    {
        moment = gf2d_list_get_nth(dataset->data,n);
        if (!moment)continue;
        if (moment->timeIndex == ignore)continue;
        if (moment->timeIndex >= target)return moment;
    }
    return NULL;
}

Moment *training_get_next(TrainingData *dataset,Moment *last)
{
    int count,n;
    if (!dataset)return NULL;
    if (!last)return gf2d_list_get_nth(dataset->data,0);
    count = gf2d_list_get_count(dataset->data);
    for (n = 0; n < count; n++)
    {
        if (gf2d_list_get_nth(dataset->data,n) == (void*)last)
        {
            return gf2d_list_get_nth(dataset->data,n+1);
        }
    }
    return NULL;
}

/*eol@eof*/
