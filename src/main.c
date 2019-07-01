#include "simple_logger.h"
#include "simple_json.h"
#include "gf2d_audio.h"
#include "gf2d_input.h"
#include "gf2d_entity.h"
#include "gf2d_graphics.h"
#include "gf2d_windows.h"
#include "gf2d_actor.h"
#include "camera.h"
#include "player.h"
#include "game_actions.h"
#include "windows_common.h"
#include "rehab_bot.h"
#include "training_data.h"
#include "world_obstacles.h"
#include "world.h"
#include "world_gen.h"
#include "world_draw.h"

static int _done = 0;
static Window *_quit = NULL;

void onCancel(void *data)
{
    _quit = NULL;
}
void onExit(void *data)
{
    _done = 1;
    _quit = NULL;
}

int main(int argc, char *argv[])
{
    Entity *player;
    init_logger("./complex.log");
    World *world;
    ReBot *bot;
    int corrections = 0;
    int i = 0;
    Uint32 gamestart;
    TrainingData *tdata;
    WorldGenConfig *wgc;
    
    slog("---====BEGIN %s====---",argv[0]);
    
    
    gf2d_graphics_initialize(
        "complexity chase",
        1200,
        700,
        1200,
        700,
        vector4d(68,68,68,255),
        0
    );
    gf2d_graphics_set_frame_delay(30);
    camera_set_dimensions(0,0,1200,700);
    gf2d_sprite_init(1024);
    gf2d_audio_init(256,16,4,1,1,1);
    gf2d_action_list_init(128);
    gf2d_text_init("config/font.cfg");
    gf2d_input_init("config/input.cfg");
    gf2d_windows_init(128);
    gf2d_entity_system_init(1024);
    
    player = player_new(vector2d(600,600));
    player->velocity.y = -3;
    player_set_lane(player,1);
    
//    world = world_from_file("calibrations/test_world.json");
    bot = rebot_load("calibrations/bot_init.json");
    tdata = training_load("calibrations/training_data_test.json");
    wgc = world_gen_config_list_load("calibrations/world_gen_calibrations.json");
    world_gen_set_seed_number(wgc, rand());
    /*
    training_build_associate_data(tdata,world);

    
    rebot_assign_training(bot, tdata);

    for (i = 0; i < 10;i++)
    {
        corrections = rebot_calibrate_on_world(bot, world);
        slog("made %i corrections",corrections);
        if (corrections <= 0)
        {
            slog("bot calibrated! after %i iterations",i);
            break;
        }
    }
    */
    
    world = world_gen_create(wgc,"calibrations/obstacle_list.json",100);
    world_save_to_file(world,"generated_world.json");

    gamestart = SDL_GetTicks();
    while (!_done)
    {
//        slog("game frame");
        gf2d_input_update();
        gf2d_windows_update_all();
        gf2d_entity_think_all();
        gf2d_entity_update_all();
        gf2d_graphics_clear_screen();

        camera_move(vector2d(0,player->velocity.y));
        world_draw(world, SDL_GetTicks() - gamestart);
        gf2d_entity_draw_all();
        gf2d_windows_draw_all();
        
        gf2d_grahics_next_frame();
        if ((gf2d_input_command_down("exit"))&&(_quit == NULL))
        {
            _quit = window_yes_no("Exit?",onExit,onCancel,NULL,NULL);
        }
    }
    // cleanup
    world_gen_config_free(wgc);
    rebot_free(bot);
    world_free(world);
    
    slog("---====END %s====---",argv[0]);
    return 0;
}

/*eol@eof*/
