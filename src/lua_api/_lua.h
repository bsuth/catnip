#ifndef LUA_API_H
#define LUA_API_H

#include <lauxlib.h>

#define luaL_loadlib(L, name, lib)                                             \
  (lua_getglobal(L, "package"),                                                \
   lua_getfield(L, -1, "loaded"),                                              \
   luaL_newlib(L, lib),                                                        \
   lua_setfield(L, -2, name),                                                  \
   lua_pop(L, 2))

#endif
