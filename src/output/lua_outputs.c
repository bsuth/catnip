#include "lua_outputs.h"
#include "output/lua_output.h"
#include <lauxlib.h>

lua_Ref lua_catnip_outputs = LUA_NOREF;

static int
lua_catnip_outputs__index(lua_State* L)
{
  int id = luaL_checknumber(L, 2);

  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    if (output->id == id) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua.ref);
      return 1;
    }
  }

  lua_pushnil(L);
  return 1;
}

static int
lua_catnip_outputs__call(lua_State* L)
{
  struct wl_list* link = lua_type(L, 3) == LUA_TNIL
    ? catnip_outputs.next
    : (*(struct catnip_output**) luaL_checkudata(L, 3, "catnip.output"))
        ->link.next;

  if (link == &catnip_outputs) {
    lua_pushnil(L);
    return 1;
  }

  struct catnip_output* output = wl_container_of(link, output, link);
  lua_rawgeti(L, LUA_REGISTRYINDEX, output->lua.ref);
  return 1;
}

static const struct luaL_Reg lua_catnip_outputs_mt[] = {
  {"__index", lua_catnip_outputs__index},
  {"__call", lua_catnip_outputs__call},
  {NULL, NULL}
};

void
lua_catnip_outputs_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.outputs");
  luaL_setfuncs(L, lua_catnip_outputs_mt, 0);
  lua_pop(L, 1);

  lua_newuserdata(L, 0);
  luaL_setmetatable(L, "catnip.outputs");
  lua_catnip_outputs = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_catnip_output_init(L);

  struct catnip_output* output = NULL;
  wl_list_for_each(output, &catnip_outputs, link)
  {
    lua_catnip_output_create(L, output);
  }
}
