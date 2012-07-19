#ifndef SCRIPTING_H

#define SCRIPTING_H

extern "C" {
#include "lualib.h"
#include "lauxlib.h"
}
#include <string>

class Engine;

class ScriptEngine
{
    lua_State *state_;
    Engine *engine_;
    static int load_map(lua_State *s);
    static int load_hero(lua_State *s);
    static int iteration(lua_State *s);
    static int start(lua_State *s);
    ScriptEngine();
public:
    void engine(Engine &e) { engine_ = &e; }
    Engine &engine() { return *engine_; }
    static ScriptEngine &instance() {
        static ScriptEngine *engine = NULL;
        if (!engine)
            engine = new ScriptEngine();
        return *engine;
    }
    bool run(std::string name);
};

#endif
