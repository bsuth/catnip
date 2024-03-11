#include "lua_windows.h"
#include "window/lua_window.h"
#include <lauxlib.h>

lua_Ref lua_catnip_windows = LUA_NOREF;

static int
lua_catnip_windows__index(lua_State* L)
{
  int id = luaL_checknumber(L, 2);

  struct catnip_window* window = NULL;
  wl_list_for_each(window, &catnip_windows, link)
  {
    if (window->id == id) {
      lua_rawgeti(L, LUA_REGISTRYINDEX, window->lua.ref);
      return 1;
    }
  }

  lua_pushnil(L);
  return 1;
}

static int
lua_catnip_windows__call(lua_State* L)
{
  struct wl_list* link = lua_type(L, 3) == LUA_TNIL
    ? catnip_windows.next
    : (*(struct catnip_window**) luaL_checkudata(L, 3, "catnip.window"))
        ->link.next;

  if (link == &catnip_windows) {
    lua_pushnil(L);
    return 1;
  }

  struct catnip_window* window = wl_container_of(link, window, link);
  lua_rawgeti(L, LUA_REGISTRYINDEX, window->lua.ref);
  return 1;
}

static const struct luaL_Reg lua_catnip_windows_mt[] = {
  {"__index", lua_catnip_windows__index},
  {"__call", lua_catnip_windows__call},
  {NULL, NULL}
};

void
lua_catnip_windows_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.windows");
  luaL_setfuncs(L, lua_catnip_windows_mt, 0);
  lua_pop(L, 1);

  lua_newuserdata(L, 0);
  luaL_setmetatable(L, "catnip.windows");
  lua_catnip_windows = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_catnip_window_init(L);

  struct catnip_window* window = NULL;
  wl_list_for_each(window, &catnip_windows, link)
  {
    lua_catnip_window_create(L, window);
  }
}
