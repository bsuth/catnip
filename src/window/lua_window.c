#include "lua_window.h"
#include "lua_events.h"
#include "utils/string.h"
#include "window/lua_window_methods.h"
#include "window/window_properties.h"
#include <lauxlib.h>
#include <stdlib.h>

static int
lua_catnip_window__index(lua_State* L)
{
  struct catnip_window** lua_window = lua_touserdata(L, 1);
  struct catnip_window* window = *lua_window;

  if (window == NULL) {
    lua_log_error(L, "attempt to index outdated userdata");
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "id")) {
    lua_pushnumber(L, window->id);
  } else if (streq(key, "x")) {
    lua_pushnumber(L, catnip_window_get_x(window));
  } else if (streq(key, "y")) {
    lua_pushnumber(L, catnip_window_get_y(window));
  } else if (streq(key, "z")) {
    lua_pushnumber(L, catnip_window_get_z(window));
  } else if (streq(key, "width")) {
    lua_pushnumber(L, catnip_window_get_width(window));
  } else if (streq(key, "height")) {
    lua_pushnumber(L, catnip_window_get_height(window));
  } else if (streq(key, "visible")) {
    lua_pushboolean(L, catnip_window_get_visible(window));
  } else if (streq(key, "focused")) {
    lua_pushboolean(L, catnip_window_get_focused(window));
  } else if (streq(key, "fullscreen")) {
    lua_pushboolean(L, catnip_window_get_fullscreen(window));
  } else if (streq(key, "maximized")) {
    lua_pushboolean(L, catnip_window_get_maximized(window));
  } else if (streq(key, "subscribe")) {
    lua_pushcfunction(L, lua_catnip_window_method_subscribe);
  } else if (streq(key, "unsubscribe")) {
    lua_pushcfunction(L, lua_catnip_window_method_unsubscribe);
  } else if (streq(key, "publish")) {
    lua_pushcfunction(L, lua_catnip_window_method_publish);
  } else if (streq(key, "destroy")) {
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
    lua_log_error(L, "attempt to index outdated userdata");
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (streq(key, "x")) {
    catnip_window_set_x(window, luaL_checknumber(L, 3));
  } else if (streq(key, "y")) {
    catnip_window_set_y(window, luaL_checknumber(L, 3));
  } else if (streq(key, "z")) {
    catnip_window_set_z(window, luaL_checknumber(L, 3));
  } else if (streq(key, "width")) {
    catnip_window_set_width(window, luaL_checknumber(L, 3));
  } else if (streq(key, "height")) {
    catnip_window_set_height(window, luaL_checknumber(L, 3));
  } else if (streq(key, "visible")) {
    catnip_window_set_visible(window, lua_toboolean(L, 3));
  } else if (streq(key, "focused")) {
    catnip_window_set_focused(window, lua_toboolean(L, 3));
  } else if (streq(key, "fullscreen")) {
    catnip_window_set_fullscreen(window, lua_toboolean(L, 3));
  } else if (streq(key, "maximized")) {
    catnip_window_set_maximized(window, lua_toboolean(L, 3));
  } else {
    lua_log_error(L, "unknown userdata field (%s)", key);
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
  lua_catnip_window_publish(L, window, "destroy", 0);

  *(window->lua.userdata) = NULL;
  luaL_unref(L, LUA_REGISTRYINDEX, window->lua.ref);
  luaL_unref(L, LUA_REGISTRYINDEX, window->lua.subscriptions);
}

void
lua_catnip_window_create(lua_State* L, struct catnip_window* window)
{
  struct catnip_window** lua_window =
    lua_newuserdata(L, sizeof(struct catnip_window*));
  luaL_setmetatable(L, "catnip.window");

  *lua_window = window;
  window->lua.userdata = lua_window;
  window->lua.ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_newtable(L);
  window->lua.subscriptions = luaL_ref(L, LUA_REGISTRYINDEX);

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

  char* global_event = strfmt("window::%s", event);
  lua_rawgeti(L, LUA_REGISTRYINDEX, window->lua.ref);
  lua_insert(L, -1 - nargs);

  lua_catnip_events_publish(
    L,
    lua_catnip_subscriptions,
    global_event,
    nargs + 1
  );

  lua_remove(L, -1 - nargs);
  free(global_event);
}

void
lua_catnip_window_init(lua_State* L)
{
  luaL_newmetatable(L, "catnip.window");
  luaL_setfuncs(L, lua_catnip_window_mt, 0);
  lua_pop(L, 1);
}
