#ifndef __LEVEL_OBJECT_H__
#define __LEVEL_OBJECT_H__

#include "gf2d_list.h"
#include "gf2d_vector.h"
#include "simple_json.h"

typedef struct
{
    Uint32 timestamp;       /**<when the object is encountered in the level*/
    Vector3D position;      /**<abstract position of the obstacle*/
    Uint8   collectable;    /**<if this object is a collectable or not*/
    List *  goodActions;    /**<actions that can be taken that will overcome this object if it is an obstacle*/
    List *  badActions;     /**<actions that can be taken that will fail if this is an obstacle*/
    float   scoreAdjust;    /**<completion of this task could result in the change of an overall score*/
    float   avoidanceAdjust;/**<avoidance of this obstacle results in a score change of this*/
}LevelObject;

/**
 * @brief allocate and initliaze a new levelobject
 * @returns NULL on error or a pointer to a new level object otherwise
 */
LevelObject *level_object_new();

/**
 * @brief parse data froma  json object and return the levelobject it described
 * @param json a pointer to a json object contain levelobject data
 * @returns NULL on error (See logs) or a newly create levelobject pointer
 */
LevelObject *level_object_parse(SJson *json);

/**
 * @brief free a previously allocated level object
 * @param lo the level object to free
 */
void level_object_free(LevelObject *lo);

#endif
