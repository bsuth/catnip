#include "lua_window.h"
#include "events/lua_events.h"
#include "window/lua_window_methods.h"
#include "window/window_properties.h"
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

  if (g_str_equal(key, "id")) {
    lua_pushnumber(L, window->id);
  } else if (g_str_equal(key, "x")) {
    lua_pushnumber(L, catnip_window_get_x(window));
  } else if (g_str_equal(key, "y")) {
    lua_pushnumber(L, catnip_window_get_y(window));
  } else if (g_str_equal(key, "z")) {
    lua_pushnumber(L, catnip_window_get_z(window));
  } else if (g_str_equal(key, "width")) {
    lua_pushnumber(L, catnip_window_get_width(window));
  } else if (g_str_equal(key, "height")) {
    lua_pushnumber(L, catnip_window_get_height(window));
  } else if (g_str_equal(key, "visible")) {
    lua_pushboolean(L, catnip_window_get_visible(window));
  } else if (g_str_equal(key, "focused")) {
    lua_pushboolean(L, catnip_window_get_focused(window));
  } else if (g_str_equal(key, "fullscreen")) {
    lua_pushboolean(L, catnip_window_get_fullscreen(window));
  } else if (g_str_equal(key, "maximized")) {
    lua_pushboolean(L, catnip_window_get_maximized(window));
  } else if (g_str_equal(key, "subscribe")) {
    lua_pushcfunction(L, lua_catnip_window_method_subscribe);
  } else if (g_str_equal(key, "unsubscribe")) {
    lua_pushcfunction(L, lua_catnip_window_method_unsubscribe);
  } else if (g_str_equal(key, "publish")) {
    lua_pushcfunction(L, lua_catnip_window_method_publish);
  } else if (g_str_equal(key, "destroy")) {
    lua_pushcfunction(L, lua_catnip_window_method_destroy);
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
  } else if (g_str_equal(key, "z")) {
    catnip_window_set_z(window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "width")) {
    catnip_window_set_width(window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "height")) {
    catnip_window_set_height(window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "visible")) {
    catnip_window_set_visible(window, lua_toboolean(L, 3));
  } else if (g_str_equal(key, "focused")) {
    catnip_window_set_focused(window, lua_toboolean(L, 3));
  } else if (g_str_equal(key, "fullscreen")) {
    catnip_window_set_fullscreen(window, lua_toboolean(L, 3));
  } else if (g_str_equal(key, "maximized")) {
    catnip_window_set_maximized(window, lua_toboolean(L, 3));
  } else {
    lua_log(L, "invalid key: %s", key);
  }

  return 0;
}

static const struct luaL_Reg lua_catnip_window_mt[] = {
  {"__index", lua_catnip_window__index},
  {"__newindex", lua_catnip_window__newindex},
  {NULL, NULL}
};

void
lua_catnip_window_destroy(lua_State* L, struct catnip_window* window)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, lua_catnip_windows);
  lua_pushnil(L);
  lua_rawseti(L, -2, window->id);
  lua_pop(L, 1);

  lua_catnip_window_publish(L, window, "destroy", 0);

  *(window->lua.userdata) = NULL;
  luaL_unref(L, LUA_REGISTRYINDEX, window->lua.ref);
  luaL_unref(L, LUA_REGISTRYINDEX, window->lua.subscriptions);
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

  lua_pushvalue(L, -1);
  window->lua.ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_newtable(L);
  window->lua.subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_rawseti(L, -2, window->id);
  lua_pop(L, 1);

  lua_catnip_window_publish(L, window, "create", 0);
}

void
lua_catnip_window_publish(
  lua_State* L,
  struct catnip_window* window,
  const char* event,
  int nargs
)
{
  lua_catnip_events_publish(L, window->lua.subscriptions, event, nargs);

  gchar* global_event = g_strconcat("window::", event, NULL);
  lua_rawgeti(L, LUA_REGISTRYINDEX, window->lua.ref);
  lua_insert(L, -1 - nargs);

  lua_catnip_events_publish(
    L,
    lua_catnip_subscriptions,
    global_event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
  g_free(global_event);
}

void
lua_catnip_window_init(lua_State* L)
{
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
