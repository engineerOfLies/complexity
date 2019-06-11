#include <stdlib.h>
#include "world_gen.h"

WorldGenFrameConfig *world_gen_frame_config_new();

void world_gen_frame_config_free(WorldGenFrameConfig *wgfc);

WorldGenConfig *world_gen_config_list_load(const char *filename);

void world_gen_config_list_free(WorldGenConfig *worldGenConfigs);

void world_gen_set_seed_number(WorldGenConfig *worldGenConfig, Uint32 seed);

void world_gen_set_seed_string(WorldGenConfig *worldGenConfigs, TextLine seedString);


/*eol@eof*/
