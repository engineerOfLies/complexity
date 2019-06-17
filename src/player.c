#include "gf2d_input.h"
#include "player.h"

static Entity *_player = NULL;

void player_think(Entity *self);
void player_update(Entity *self);
float get_lane_x(Uint8 lane);

Entity *player_new(Vector2D position)
{
    Entity *self;
    self = gf2d_entity_new();
    if (!self)return NULL;
    
    gf2d_line_cpy(self->name,"player");
    self->parent = NULL;
    
    self->shape = gf2d_shape_rect(-16, -16, 30, 60);
    gf2d_body_set(
        &self->body,
        "player",
        1,
        WORLD_LAYER,
        0,
        1,
        position,
        vector2d(0,0),
        10,
        1,
        0,
        &self->shape,
        self,
        NULL);

    gf2d_actor_load(&self->actor,"actors/player.actor");
    gf2d_actor_set_action(&self->actor,"idle");

    self->sound[0] = gf2d_sound_load("sounds/jump_10.wav",1,-1);

    
    vector2d_copy(self->position,position);
    
    vector2d_copy(self->scale,self->actor.al->scale);
    vector2d_set(self->scaleCenter,64,64);
    vector3d_set(self->rotation,64,64,0);
    vector2d_set(self->flip,0,0);
    vector2d_set(self->facing,1,0);
    
    self->think = player_think;
    self->update = player_update;
/*
    self->draw = player_draw;
    self->touch = player_touch;
    self->damage = player_damage;
    self->die = player_die;
    self->free = level_remove_entity;
    self->activate = player_activate;

    self->data = (void*)&playerData;
  */  
    self->health = self->maxHealth = 100;
    _player = self;
//    level_add_entity(self);
    return self;
}


void player_set_lane(Entity *player,Uint8 lane)
{
    player->position.x = get_lane_x(lane);
    player->lane = lane;
}

void player_update(Entity *self)
{
    switch (self->state)
    {
        case ES_Ducking:
            // set scale
            self->scale.x = self->scale.y = (fabs(self->cooldown -20)/20);
            if (self->cooldown == 0)
            {
                self->state = ES_Idle;
            }
            break;
        case ES_Jumping:
            //set scale
            self->scale.x = self->scale.y = 2-(fabs(self->cooldown -20)/20);
            if (self->cooldown == 0)
            {
                self->state = ES_Idle;
            }
            break;
        default:
            break;
    }
    if (self->cooldown > 0)self->cooldown--;
}

void player_think(Entity *self)
{
    if ((gf2d_input_command_pressed("left"))&&(self->lane > 0))
    {
        player_set_lane(self,self->lane - 1);
    }
    else if ((gf2d_input_command_pressed("right"))&&(self->lane < 2))
    {
        player_set_lane(self,self->lane + 1);
    }
    else if ((gf2d_input_command_pressed("jump"))&&(self->cooldown <= 0))
    {
        self->cooldown = 40;
        self->state = ES_Jumping;
    }
    else if ((gf2d_input_command_pressed("duck"))&&(self->cooldown <= 0))
    {
        self->cooldown = 40;
        self->state = ES_Ducking;
    }
}

float get_lane_x(Uint8 lane)
{
    if (lane == 0)
    {
        return 400;
    }
    if (lane == 1)
    {
        return 600;
    }
    if (lane == 2)
    {
        return 800;
    }
    return 600;
}
/*eol@eof*/
