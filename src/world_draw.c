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
    int i,c;
    WorldFrame *frame;
    Vector2D screen;
    float y;
    Uint32 ignore = 0 - 1;
    if (!world)
    {
        slog("no world provided to draw");
        return;
    }
    if (world->scrollSpeed == 0)
    {
        slog("world scroll speed cannot be 0!");
        return;
    }

    screen = gf2d_graphics_get_resolution();
    c = (screen.y/128.0)+1;

    for (frame = world_frame_get_by_time(world, timeIndex, 5, ignore);
         frame != NULL;
         frame = world_get_next_frame(world,frame,screen.y/world->scrollSpeed))
    {
        y = screen.y - ((frame->timeIndex - timeIndex) * world->scrollSpeed);
        ignore = frame->timeIndex;
        world_draw_frame(frame,y);
    }
}

/*eol@eof*/
