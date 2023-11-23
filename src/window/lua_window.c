#include "lua_window.h"
#include "config/events.h"
#include "properties.h"
#include <glib.h>
#include <lauxlib.h>

lua_Ref lua_catnip_windows = LUA_NOREF;

static int
lua_catnip_window__index(lua_State* L)
{
  struct catnip_window** lua_window = lua_touserdata(L, 1);
  struct catnip_window* window = *lua_window;

  if (window == NULL) {
    lua_log(L, "attempt to index outdated window");
    lua_pushnil(L);
    return 1;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    lua_pushnumber(L, catnip_window_get_x(window));
  } else if (g_str_equal(key, "y")) {
    lua_pushnumber(L, catnip_window_get_y(window));
  } else if (g_str_equal(key, "width")) {
    lua_pushnumber(L, catnip_window_get_width(window));
  } else if (g_str_equal(key, "height")) {
    lua_pushnumber(L, catnip_window_get_height(window));
  } else if (g_str_equal(key, "focused")) {
    lua_pushboolean(L, catnip_window_get_focused(window));
  } else if (g_str_equal(key, "maximized")) {
    lua_pushboolean(L, catnip_window_get_maximized(window));
  } else if (g_str_equal(key, "fullscreen")) {
    lua_pushboolean(L, catnip_window_get_fullscreen(window));
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
lua_catnip_window__newindex(lua_State* L)
{
  struct catnip_window** lua_window = lua_touserdata(L, 1);
  struct catnip_window* window = *lua_window;

  if (window == NULL) {
    lua_log(L, "attempt to index outdated window");
    return 0;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_log(L, "invalid key type: %s", lua_typename(L, key_type));
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "x")) {
    catnip_window_set_x(window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "y")) {
    catnip_window_set_y(window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "width")) {
    catnip_window_set_width(window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "height")) {
    catnip_window_set_height(window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "focused")) {
    catnip_window_set_focused(window, lua_toboolean(L, 3));
  } else if (g_str_equal(key, "maximized")) {
    catnip_window_set_maximized(window, lua_toboolean(L, 3));
  } else if (g_str_equal(key, "fullscreen")) {
    catnip_window_set_fullscreen(window, lua_toboolean(L, 3));
  } else {
    lua_log(L, "invalid key: %s", key);
  }

  return 0;
}

static int
lua_catnip_window__gc(lua_State* L)
{
  struct catnip_window** lua_window = lua_touserdata(L, 1);
  (*lua_window)->lua.userdata = NULL;
  return 0;
}

static const struct luaL_Reg lua_catnip_window_mt[] = {
  {"__index", lua_catnip_window__index},
  {"__newindex", lua_catnip_window__newindex},
  {"__gc", lua_catnip_window__gc},
  {NULL, NULL}
};

void
lua_catnip_window_destroy(lua_State* L, struct catnip_window* window)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_windows);
  size_t lua_catnip_windows_len = lua_objlen(L, -1);
  bool found_lua_window = false;

  for (int i = 1; i <= lua_catnip_windows_len; ++i) {
    if (found_lua_window) {
      lua_rawgeti(L, -1, i);
      lua_rawseti(L, -2, i - 1);
    } else {
      lua_rawgeti(L, -1, i);
      found_lua_window = window == *(struct catnip_window**) lua_popuserdata(L);
    }
  }

  lua_pushnil(L);
  lua_rawseti(L, -2, lua_catnip_windows_len);

  config_events_publish("window::destroy");

  *(window->lua.userdata) = NULL;
  lua_pop(L, 1);
}

void
lua_catnip_window_create(lua_State* L, struct catnip_window* window)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_windows);

  struct catnip_window** lua_window =
    lua_newuserdata(L, sizeof(struct catnip_window*));
  luaL_setmetatable(L, "catnip.window");

  *lua_window = window;
  window->lua.userdata = lua_window;

  lua_rawseti(L, -2, lua_objlen(L, -2) + 1);
  lua_pop(L, 1);

  config_events_publish("window::create");
}

void
lua_catnip_window_init(lua_State* L)
{
  // TODO: make this table read only from lua
  lua_newtable(L);
  lua_catnip_windows = luaL_ref(L, LUA_REGISTRYINDEX);

  luaL_newmetatable(L, "catnip.window");
  luaL_setfuncs(L, lua_catnip_window_mt, 0);
  lua_pop(L, 1);

  struct catnip_window* window = NULL;
  wl_list_for_each(window, &catnip_windows, link)
  {
    lua_catnip_window_create(L, window);
  }
}
