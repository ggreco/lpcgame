#include "game_map.h"
#include "video.h"
#include "scroller.h"
#include "char.h"
#include <iostream>
#include "engine.h"
#include "scripting.h"

int main(int argc, char *argv[])
{
    int x = 200, y = 200;
    if (argc < 2)
        return -1;


    try
    {
        Engine engine;
        ScriptEngine::instance().engine(engine);
   
        ScriptEngine::instance().run(argv[1]);
    }
    catch (std::string &err) {
        std::cerr << "Terminated with exception: " << err << "\n";
    }
    catch (std::exception &err) {
        std::cerr << "Terminated STD with exception: " << err.what() << "\n";
    }
    catch (...) {
        std::cerr << "Terminated with unknown exception!\n";
    }
}

