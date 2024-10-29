#include "lua_catnip.h"
#include "compositor/display.h"
#include "compositor/seat.h"
#include "config.h"
#include "desktop/lua_cursor.h"
#include "desktop/lua_keyboards.h"
#include "desktop/lua_outputs.h"
#include "desktop/lua_window.h"
#include "desktop/lua_windows.h"
#include "desktop/window.h"
#include "desktop/windows.h"
#include "extensions/string.h"
#include "lua_events.h"
#include "lua_keybindings.h"
#include "widget/lua_widget.h"
#include <lauxlib.h>

// -----------------------------------------------------------------------------
// Lua Methods
// -----------------------------------------------------------------------------

static int
catnip_lua_catnip_lua_on(lua_State* L)
{
  const char* event = luaL_checkstring(L, 1);
  luaL_checktype(L, 2, LUA_TFUNCTION);

  lua_pushvalue(L, 2); // push callback to top in case of trailing args
  catnip_lua_events_subscribe(L, catnip_lua_subscriptions, event);

  return 1;
}

static int
catnip_lua_catnip_lua_emit(lua_State* L)
{
  const char* event = luaL_checkstring(L, 1);

  catnip_lua_events_publish(
    L,
    catnip_lua_subscriptions,
    event,
    lua_gettop(L) - 1
  );

  return 0;
}

static int
catnip_lua_catnip_lua_reload(lua_State* L)
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
catnip_lua_catnip_lua_quit(lua_State* L)
{
  catnip_display_run = false;
  return 0;
}

// -----------------------------------------------------------------------------
// Metatable
// -----------------------------------------------------------------------------

static int
catnip_lua_catnip__index(lua_State* L)
{
  const char* key = luaL_checkstring(L, 2);

  if (streq(key, "cursor")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, catnip_lua_cursor->ref);
  } else if (streq(key, "keyboards")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, catnip_lua_keyboards->ref);
  } else if (streq(key, "outputs")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, catnip_lua_outputs->ref);
  } else if (streq(key, "windows")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, catnip_lua_windows->ref);
  } else if (streq(key, "widget")) {
    lua_rawgeti(L, LUA_REGISTRYINDEX, catnip_lua_widget);
  } else if (streq(key, "focused")) {
    struct catnip_window* focused_window = catnip_windows_get_focused();

    if (focused_window == NULL) {
      lua_pushnil(L);
    } else {
      lua_rawgeti(L, LUA_REGISTRYINDEX, focused_window->lua_window->ref);
    }
  } else if (streq(key, "bind")) {
    lua_pushcfunction(L, catnip_lua_keybindings_lua_bind);
  } else if (streq(key, "unbind")) {
    lua_pushcfunction(L, catnip_lua_keybindings_lua_unbind);
  } else if (streq(key, "on")) {
    lua_pushcfunction(L, catnip_lua_catnip_lua_on);
  } else if (streq(key, "emit")) {
    lua_pushcfunction(L, catnip_lua_catnip_lua_emit);
  } else if (streq(key, "reload")) {
    lua_pushcfunction(L, catnip_lua_catnip_lua_reload);
  } else if (streq(key, "quit")) {
    lua_pushcfunction(L, catnip_lua_catnip_lua_quit);
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
catnip_lua_catnip__newindex(lua_State* L)
{
  const char* key = luaL_checkstring(L, 2);

  if (streq(key, "focused")) {
    if (lua_type(L, 3) == LUA_TNIL) {
      wlr_seat_keyboard_notify_clear_focus(catnip_seat);
    } else {
      struct catnip_lua_window* lua_window =
        luaL_checkudata(L, 3, "catnip.window");
      catnip_window_focus(lua_window->window);
    }
  }

  return 0;
}

static const struct luaL_Reg catnip_lua_catnip_mt[] = {
  {"__index", catnip_lua_catnip__index},
  {"__newindex", catnip_lua_catnip__newindex},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// Core
// -----------------------------------------------------------------------------

void
catnip_lua_catnip_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip");
  luaL_setfuncs(L, catnip_lua_catnip_mt, 0);
  lua_pop(L, 1);
}
