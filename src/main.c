#include "simple_logger.h"
#include "simple_json.h"

int main(int argc, char *argv[])
{
    init_logger("./complex.log");
    
    slog("---====BEGIN %s====---",argv[0]);
    
    
    
    slog("---====END %s====---",argv[0]);
    return 0;
}

/*eol@eof*/
