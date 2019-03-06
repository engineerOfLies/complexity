#include "simple_logger.h"
#include "simple_json.h"
#include "level.h"
#include "actions.h"

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
    
    
    level_free(level);
    
    slog("---====END %s====---",argv[0]);
    return 0;
}

/*eol@eof*/
