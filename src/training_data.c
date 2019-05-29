#include "training_data.h"
#include "simple_json.h"
#include "simple_logger.h"
#include <stdlib.h>

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
    return data;
}

void training_free(TrainingData *dataset)
{
    Moment *moment;
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

Moment *training_get_moment_by_similar_moment(TrainingData *dataset,Moment *moment,Sint64 timeThreshold)
{
    if (!dataset)return NULL;
    if (!moment)return NULL;
    //TODO
    return NULL;
}

Moment *training_get_moment_by_similar_moment_frame(TrainingData *dataset,List *momentframe,Sint64 timeThreshold,Uint8 matchBest);

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
