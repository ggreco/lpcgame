#include "scripting.h"
#include "engine.h"
#include "char.h"
#include <stdlib.h>

ScriptEngine::
ScriptEngine()
{
    if (!(state_ = lua_open()))
        return;

    luaL_openlibs(state_);

    lua_register(state_, "load_map", ScriptEngine::load_map);
    lua_register(state_, "load_hero", ScriptEngine::load_hero);
    lua_register(state_, "engine_iteration", ScriptEngine::iteration);
    lua_register(state_, "engine_start", ScriptEngine::start);
}

int ScriptEngine::
load_map(lua_State *s)
{
    const char *map = luaL_checkstring(s,1);

    instance().engine().load_map(map);
    return 0;
}

bool ScriptEngine::
run(std::string name)
{
  if (name.find(".lua") == std::string::npos)
      name.append(".lua");

  if (luaL_loadfile(state_, name.c_str())) {
      std::cerr << "Unable to load " << name << ": " << lua_tostring(state_, 1) << '\n';
      lua_pop(state_, 1);      
      return false;
  }

  if (lua_pcall(state_, 0, 0, 0)) {
      std::cerr << "Error in LUA script " << name << ": " << lua_tostring(state_, -1) << '\n';  
      lua_pop(state_, 1);      
      return false;
  }
  return true;
}

int ScriptEngine::
load_hero(lua_State *s)
{
    const char *hero = luaL_checkstring(s,1);
    if (!hero)
        return 0;

    int x  = luaL_optint(s, 2, -1), y  = luaL_optint(s, 3, -1);

    try {
        Character *ch = new Character(hero);
        
        if (!x == -1 || !y == -1)
            instance().engine().map().SpawnPoint(x, y);

        if (x == -1)
            x = 100;
        if (y == -1)
            y = 100;

        ch->position(x, y);
        instance().engine().hero(ch);
        instance().engine().add(ch);
    }
    catch (...) {
    }
    return 0;
}

int ScriptEngine::
iteration(lua_State *s) {
    if (instance().engine().iteration())
        lua_pushboolean(s, 1);
    else
        lua_pushboolean(s, 0);
    return 1;
}

int ScriptEngine::
start(lua_State *) {
    instance().engine().start();
    return 0;
}
