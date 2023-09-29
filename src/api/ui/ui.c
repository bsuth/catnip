#include "ui.h"
#include "api/refs.h"
#include "api/ui/box.h"
#include <lauxlib.h>

void
init_api_ui(lua_State* L)
{
  lua_newtable(L);
  api_catnip_ui = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip);
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_ui);
  lua_setfield(L, -2, "ui");
  lua_pop(L, 1);

  init_api_ui_box(L);
}
