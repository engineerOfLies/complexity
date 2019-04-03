#ifndef __ROOFTOP_H__
#define __ROOFTOP_H__

#include "gf2d_vector.h"
#include "gf2d_sprite.h"
#include "gf2d_list.h"
#include "gf2d_shape.h"

typedef struct
{
    Vector2D position;
    Vector2D size;
    Sprite *image;
    List *objects;      /**<objects that appear on the rooftopsk*/
}Roof;

Roof *rooftop_new();
void rooftop_free(Roof *roof);
void rooftop_clear(Roof *roof);
Roof *rooftop_new_data(Vector2D position,char *image,Vector2D size);
Rect rooftop_get_bounds(Roof *roof);

void rooftop_set_position(Roof *roof,Vector2D position);

void rooftop_draw(Roof *roof,Vector2D offset);

void rooftop_clear(Roof *roof);

void rooftop_set_image(Roof *roof,char *image);

#endif
