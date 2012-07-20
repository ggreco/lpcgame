#include "scripting.h"
#include "engine.h"
#include "char.h"
#include <stdlib.h>

// Character bindings
static int L_newChar(lua_State *s) {
    const char *name = lua_tostring(s, 1);
    int x  = luaL_optint(s, 2, -1), y  = luaL_optint(s, 3, -1);
    std::cerr << "Creating new character with lua from " << name << " at (" << x << ',' << y << ")\n";
    Character **ch = (Character **)lua_newuserdata(s, sizeof(Character *));

    *ch = new Character(name);

    if (x != -1 && y != -1)
        (*ch)->position(x, y);

    ScriptEngine::instance().engine().add(*ch);
    lua_getglobal(s, "Character"); 
    lua_setmetatable(s, -2); 
    return 1;
}

static int L_charPosition(lua_State *s) {
    Character** p = (Character**) lua_touserdata(s, 1);
    if (*p) (*p)->position(lua_tonumber(s, 2), lua_tonumber(s, 3));
    return 0;
}
static int L_charMoving(lua_State *s) {
    Character** p = (Character**) lua_touserdata(s, 1);
    lua_pushboolean(s, *p ? (*p)->moving() : false);
    return 1;
}
static int L_charX(lua_State *s) {
    Character** p = (Character**) lua_touserdata(s, 1);
    lua_pushnumber(s, *p ? (*p)->x() : -1);
    return 1;
}
static int L_charY(lua_State *s) {
    Character** p = (Character**) lua_touserdata(s, 1);
    lua_pushnumber(s, *p ? (*p)->y() : -1);
    return 1;
}
static int L_charGoto(lua_State *s) {
    Character** p = (Character**) lua_touserdata(s, 1);
    if (*p) (*p)->go_to(lua_tonumber(s, 2), lua_tonumber(s, 3), SDL_GetTicks());
    return 0;
}
static int L_charDistance2(lua_State *s) {
    Character** p = (Character**) lua_touserdata(s, 1);
    int x  = luaL_optint(s, 2, -1), y  = luaL_optint(s, 3, -1);
    lua_pushnumber(s, (*p && x != -1 && y != -1) ? (*p)->distance(x, y) : -1);
    return 1;
}
static int L_charDistance(lua_State *s) {
    Character** p = (Character**) lua_touserdata(s, 1);
    Character** j = (Character**) lua_touserdata(s, 2);
    lua_pushnumber(s, (*p && *j) ? (*p)->distance(*j) : -1);
    return 1;
}
// Engine bindings
static int L_iteration(lua_State *s) {
    lua_pushboolean(s, ScriptEngine::instance().engine().iteration());
    return 1;
}

static int L_start(lua_State *) {
    ScriptEngine::instance().engine().start();
    return 0;
}

static int L_map_width(lua_State *s) {
    lua_pushnumber(s, ScriptEngine::instance().engine().map().map_width());
    return 1;
}
static int L_map_height(lua_State *s) {
    lua_pushnumber(s, ScriptEngine::instance().engine().map().map_height());
    return 1;
}

static int L_load_map(lua_State *s) {
    lua_pushboolean(s, ScriptEngine::instance().engine().load_map(luaL_checkstring(s,1)));
    return 1;
}

static int L_set_hero(lua_State *s) {
    Character** p = (Character**) lua_touserdata(s, 1);
    ScriptEngine::instance().engine().hero(*p);
    return 0;
}
static const luaL_Reg h_methods[] = {
    {"new", L_newChar},
    {"set_position", L_charPosition},
    {"x", L_charX},
    {"y", L_charY},
    {"moving", L_charMoving},
    {"goto", L_charGoto},
    {"distance_from", L_charDistance},
    {"distance", L_charDistance2},
    {NULL, NULL}
};

static const luaL_Reg e_methods[] = {
    {"load_map", L_load_map},
    {"start", L_start},
    {"iteration", L_iteration}, 
    {"set_hero", L_set_hero},     
    {"map_width", L_map_width},
    {"map_height", L_map_height},
    {NULL, NULL}
};

ScriptEngine::
ScriptEngine()
{
    if (!(state_ = lua_open()))
        return;

    luaL_openlibs(state_);

    register_class("Character", h_methods);
    register_class("Engine", e_methods);
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


