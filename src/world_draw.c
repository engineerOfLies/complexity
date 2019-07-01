#include "simple_logger.h"
#include "gf2d_graphics.h"
#include "world.h"
#include "world_gen.h"

void world_draw_obstacle(Vector2D position)
{
    //switch on the world obstacle to draw
}

void world_draw_frame(WorldFrame *frame, float y)
{
    slog("drawing frame at height %f",y);
    // if any gaps, draw the building gap
        // if any of the gaps are filled, draw bridge
    // for each obstacle, draw the obstacle
        // if its a gap, skip
    // for each collectable, draw the collectable
}

void world_draw(World *world, Uint32 timeIndex)
{
}

/*eol@eof*/
