#ifndef __PLAYER_H__
#define __PLAYER_H__

#include "gf2d_entity.h"

Entity *player_new(Vector2D position);
void player_set_lane(Entity *player,Uint8 lane);

#endif
