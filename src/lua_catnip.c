#include "lua_catnip.h"
#include "canvas/lua_canvas.h"
#include "canvas/lua_png.h"
#include "canvas/lua_svg.h"
#include "config.h"
#include "cursor/lua_cursor.h"
#include "display.h"
#include "keyboard/lua_keyboard_list.h"
#include "lua_events.h"
#include "output/lua_output_list.h"
#include "seat.h"
#include "utils/string.h"
#include "window/lua_window_list.h"
#include "window/window.h"
#include "window/windows.h"
#include <lauxlib.h>

static int
lua_catnip_subscribe(lua_State* L)
{
  const char* event = luaL_checkstring(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  lua_pushvalue(L, 2); // push callback to top in case of trailing args
  lua_catnip_events_subscribe(L, lua_catnip_subscriptions, event);

  return 1; // return callback for unsubscribe handle
}

static int
lua_catnip_unsubscribe(lua_State* L)
{
  const char* event = luaL_checkstring(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  lua_pushvalue(L, 2); // push callback to top in case of trailing args
  lua_catnip_events_unsubscribe(L, lua_catnip_subscriptions, event);
  lua_pop(L, 1);

  return 0;
}

static int
lua_catnip_publish(lua_State* L)
{
  const char* event = luaL_checkstring(L, 1);

  lua_catnip_events_publish(
    L,
    lua_catnip_subscriptions,
    event,
    lua_gettop(L) - 1
  );

  return 0;
}

static int
lua_catnip_reload(lua_State* L)
{
  if (catnip_config_loading) {
    lua_log_warning(L, "attempted to reload during startup, ignoring...");
    return 0;
  } else {
    catnip_config_request_reload = true;
    return 0;
  }
}

static int
lua_catnip_quit(lua_State* L)
{
  catnip_display_run = false;
  return 0;
}

static int
lua_catnip__index(lua_State* L)
{
  const char* key = luaL_checkstring(L, 2);

  if (streq(key, "cursor")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_cursor->ref);
  } else if (streq(key, "keyboards")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_keyboard_list->ref);
  } else if (streq(key, "outputs")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_output_list->ref);
  } else if (streq(key, "windows")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_window_list->ref);
  } else if (streq(key, "focused")) {
    struct catnip_window* focused_window = catnip_windows_get_focused();
    if (focused_window == NULL) {
      lua_pushnil(L);
    } else {
      lua_rawgeti(L, LUA_REGISTRYINDEX, focused_window->lua_resource->ref);
    }
  } else if (streq(key, "canvas")) {
    lua_pushcfunction(L, lua_catnip_canvas);
  } else if (streq(key, "png")) {
    lua_pushcfunction(L, lua_catnip_png);
  } else if (streq(key, "svg")) {
    lua_pushcfunction(L, lua_catnip_svg);
  } else if (streq(key, "subscribe")) {
    lua_pushcfunction(L, lua_catnip_subscribe);
  } else if (streq(key, "unsubscribe")) {
    lua_pushcfunction(L, lua_catnip_unsubscribe);
  } else if (streq(key, "publish")) {
    lua_pushcfunction(L, lua_catnip_publish);
  } else if (streq(key, "reload")) {
    lua_pushcfunction(L, lua_catnip_reload);
  } else if (streq(key, "quit")) {
    lua_pushcfunction(L, lua_catnip_quit);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip__newindex(lua_State* L)
{
  const char* key = luaL_checkstring(L, 2);

  if (streq(key, "focused")) {
    if (lua_type(L, 3) == LUA_TNIL) {
      wlr_seat_keyboard_notify_clear_focus(catnip_seat);
    } else {
      catnip_window_focus(lua_catnip_resource_checkname(L, 3, "window"));
    }
  }

  return 0;
}

static const struct luaL_Reg lua_catnip_mt[] = {
  {"__index", lua_catnip__index},
  {"__newindex", lua_catnip__newindex},
  {NULL, NULL}
};

void
lua_catnip_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip");
  luaL_setfuncs(L, lua_catnip_mt, 0);
  lua_pop(L, 1);
}
