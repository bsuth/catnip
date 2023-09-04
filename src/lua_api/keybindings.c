#include "keybindings.h"
#include "_lua.h"
#include <lauxlib.h>
#include <lua.h>

static int
lua_echo(lua_State* L)
{
  const char* s = luaL_checkstring(L, 1);
  lua_pushstring(L, s);
  return 1;
}

static const struct luaL_Reg lib_keybindings[] = { { "echo", lua_echo },
                                                   { NULL, NULL } };

void
load_lib_keybindings(lua_State* L)
{
  luaL_loadlib(L, "bwc.keybindings", lib_keybindings);
}
