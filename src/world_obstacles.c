#include "world_obstacles.h"

#include "simple_logger.h"

void world_obstacle_free(WorldObstacle *wo)
{
    if (!wo)return;
    free(wo);
}

WorldObstacle *world_obstacle_new()
{
    WorldObstacle *wo = NULL;
    wo = (WorldObstacle *)malloc(sizeof(WorldObstacle));
    if (!wo)
    {
        slog("failed to allocate new WorldObstacle");
        return NULL;
    }
    memset(wo,0,sizeof(WorldObstacle));
    return wo;
}


/*eol@eof*/
