#ifndef __ACTIONS_H__
#define __ACTIONS_H__

#include "gf2d_text.h"

typedef struct
{
    TextLine name;              /**<the unique name for an action (Jump, duck, speed_up, slow_down, etc)*/
    Uint32   leadInTime;        /**<how much time it takes before this action takes effect*/
    Uint32   cooldownTime;      /**<how much time before the player can take a follow up action*/
    float    difficultyWeight;  /**<if there is a weight to be evaluated with the completion of this action*/
}Action;

#endif
