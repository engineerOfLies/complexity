#include <stdlib.h>
#include "rooftop.h"
#include "simple_logger.h"
#include "camera.h"
#include "gf2d_shape.h"

Rect rooftop_get_bounds(Roof *roof)
{
    Rect rect = {0};
    if (!roof)return rect;
    rect.x = roof->position.x;
    rect.y = roof->position.y;
    rect.w = roof->size.x;
    rect.h = roof->size.y;
    return rect;
}

void rooftop_set_position(Roof *roof,Vector2D position)
{
    if (!roof)
    {
        slog("no roof provided");
        return;
    }
    vector2d_copy(roof->position,position);
}

void rooftop_set_image(Roof *roof,char *image)
{
    if (!roof)
    {
        slog("no roof provided");
        return;
    }
    roof->image = gf2d_sprite_load_image(image);
}

void rooftop_draw(Roof *roof,Vector2D offset)
{
    Vector2D drawPosition;
    if (!roof)
    {
        slog("no roof provided");
        return;
    }
    vector2d_add(drawPosition,roof->position,offset);
    gf2d_sprite_draw_image(roof->image,drawPosition);
}

Roof *rooftop_new_data(Vector2D position,char *image,Vector2D size)
{
    Roof *roof;
    roof = rooftop_new();
    if (!roof)return NULL;
    rooftop_set_image(roof,image);
    rooftop_set_position(roof,position);
    vector2d_copy(roof->size,size);
    return roof;
}

void rooftop_clear(Roof *roof)
{
    if (!roof)
    {
        slog("no roof provided");
        return;
    }
    gf2d_sprite_free(roof->image);
}

Roof *rooftop_new()
{
    Roof *roof;
    roof = (Roof*)malloc(sizeof(Roof));
    if (!roof)
    {
        slog("failed to allocate a rooftop");
        return NULL;
    }
    memset(roof,0,sizeof(Roof));
    return roof;
}

void rooftop_free(Roof *roof)
{
    if (!roof)return;
    rooftop_clear(roof);
    free(roof);
}

/*eol@eof*/
