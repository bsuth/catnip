#include "lua_window_methods.h"
#include "events/lua_events.h"
#include "window.h"
#include "window/lua_window.h"
#include <lauxlib.h>
#include <wlr/types/wlr_xdg_shell.h>

int
lua_catnip_window_method_subscribe(lua_State* L)
{
  struct catnip_window** lua_window = luaL_checkudata(L, 1, "catnip.window");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_subscribe(L, (*lua_window)->lua.subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

int
lua_catnip_window_method_unsubscribe(lua_State* L)
{
  struct catnip_window** lua_window = luaL_checkudata(L, 1, "catnip.window");
  const char* event = luaL_checkstring(L, 2);
  luaL_checktype(L, 3, LUA_TFUNCTION);

  lua_pushvalue(L, 3); // push callback to top in case of trailing args
  lua_catnip_events_unsubscribe(L, (*lua_window)->lua.subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

int
lua_catnip_window_method_publish(lua_State* L)
{
  struct catnip_window** lua_window = luaL_checkudata(L, 1, "catnip.window");
  const char* event = luaL_checkstring(L, 2);
  lua_catnip_window_publish(L, *lua_window, event, 0);
  return 0;
}

int
lua_catnip_window_method_destroy(lua_State* L)
{
  struct catnip_window** lua_window = luaL_checkudata(L, 1, "catnip.window");
  wlr_xdg_toplevel_send_close((*lua_window)->xdg_toplevel);
  return 0;
}
