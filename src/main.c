#include "simple_logger.h"
#include "simple_json.h"
#include "level.h"
#include "game_actions.h"
#include "gf2d_input.h"
#include "gf2d_graphics.h"
#include "gf2d_windows.h"
#include "gf2d_actor.h"
#include "windows_common.h"

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
    Level *level;
    List *actionList;
    init_logger("./complex.log");
    
    slog("---====BEGIN %s====---",argv[0]);
    
    level = level_load("test_level.json");
    if (!level)slog("failed to parse test level file");
    else slog("parsed level file");
    
    actionList = action_list_load("actionlist.json");
    if (!actionList) slog("failed to parse action list");
    else slog("action list loaded");
    
    gf2d_graphics_initialize(
        "complexity chase",
        1200,
        700,
        1200,
        700,
        vector4d(128,128,128,255),
        0
    );
    gf2d_graphics_set_frame_delay(30);
    gf2d_sprite_init(1024);
    gf2d_action_list_init(128);
    gf2d_text_init("config/font.cfg");
    gf2d_input_init("config/input.cfg");
    gf2d_windows_init(128);
    
    while (!_done)
    {
        gf2d_input_update();
        gf2d_windows_update_all();

        gf2d_graphics_clear_screen();
        
        
        gf2d_windows_draw_all();
        
        gf2d_grahics_next_frame();
        if ((gf2d_input_command_down("exit"))&&(_quit == NULL))
        {
            _quit = window_yes_no("Exit?",onExit,onCancel,NULL,NULL);
        }

    }
    
    level_free(level);
    
    slog("---====END %s====---",argv[0]);
    return 0;
}

/*eol@eof*/
