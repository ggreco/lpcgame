/*
** $Id: lapi.h 110 2006-04-11 17:08:37Z ggreco $
** Auxiliary functions from Lua API
** See Copyright Notice in lua.h
*/

#ifndef lapi_h
#define lapi_h


#include "lobject.h"


LUAI_FUNC void luaA_pushobject (lua_State *L, const TValue *o);

#endif
