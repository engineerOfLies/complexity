#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gf2d_entity.h"

enum PlayerWorldStates
{
        PWS_Idle = 1,
        PWS_Jump = 2,
        PWS_Duck = 4
};

Entity *player_new(Vector2D position);
void player_set_lane(Entity *player,Uint8 lane);

#endif
