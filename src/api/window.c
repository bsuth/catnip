#include "window.h"
#include "api/api.h"
#include "config/config.h"
#include "server/seat.h"
#include "server/window.h"
#include "utils/log.h"
#include "utils/wayland.h"
#include <glib.h>
#include <lauxlib.h>
#include <string.h>
#include <wlr/types/wlr_scene.h>

// -----------------------------------------------------------------------------
// Types
// -----------------------------------------------------------------------------

struct api_window {
  struct server_window* server_window;

  int index;

  struct {
    struct wl_listener server_window_destroy;
  } listeners;
};

// -----------------------------------------------------------------------------
// State
// -----------------------------------------------------------------------------

lua_Ref api_catnip_windows = LUA_NOREF;

static struct {
  struct wl_listener new_server_window;
} listeners;

// Keep track of num_windows manually. keeps things simpler when manipulating
// Lua's catnip.windows
static int num_windows = 0;

// -----------------------------------------------------------------------------
// Metatable: catnip.window
// -----------------------------------------------------------------------------

static int
api_window__index(lua_State* L)
{
  struct api_window* api_window = lua_touserdata(L, 1);
  struct server_window* server_window = api_window->server_window;

  if (server_window == NULL) {
    log_warning("cannot get field of expired winow");
    lua_pushnil(L);
    return 1;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    lua_pushnil(L);
    return 1;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "lx")) {
    lua_pushnumber(L, server_window_get_lx(server_window));
  } else if (g_str_equal(key, "ly")) {
    lua_pushnumber(L, server_window_get_ly(server_window));
  } else if (g_str_equal(key, "gx")) {
    lua_pushnumber(L, server_window_get_gx(server_window));
  } else if (g_str_equal(key, "gy")) {
    lua_pushnumber(L, server_window_get_gy(server_window));
  } else if (g_str_equal(key, "width")) {
    lua_pushnumber(L, server_window_get_width(server_window));
  } else if (g_str_equal(key, "height")) {
    lua_pushnumber(L, server_window_get_height(server_window));
  } else if (g_str_equal(key, "focused")) {
    lua_pushboolean(L, server_window_get_focused(server_window));
  } else if (g_str_equal(key, "maximized")) {
    lua_pushboolean(L, server_window_get_maximized(server_window));
  } else if (g_str_equal(key, "fullscreen")) {
    lua_pushboolean(L, server_window_get_fullscreen(server_window));
  } else {
    lua_pushnil(L);
  }

  return 1;
}

static int
api_window__newindex(lua_State* L)
{
  struct api_window* api_window = lua_touserdata(L, 1);
  struct server_window* server_window = api_window->server_window;

  if (server_window == NULL) {
    log_warning("cannot set field of expired window");
    return 0;
  }

  int key_type = lua_type(L, 2);
  if (key_type != LUA_TSTRING) {
    log_warning("invalid key type: %s", lua_typename(L, key_type));
    return 0;
  }

  const char* key = lua_tostring(L, 2);

  if (g_str_equal(key, "lx")) {
    server_window_set_lx(server_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "ly")) {
    server_window_set_ly(server_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "gx")) {
    server_window_set_gx(server_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "gy")) {
    server_window_set_gy(server_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "width")) {
    server_window_set_width(server_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "height")) {
    server_window_set_height(server_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "focused")) {
    server_window_set_focused(server_window, lua_toboolean(L, 3));
  } else if (g_str_equal(key, "maximized")) {
    server_window_set_maximized(server_window, lua_toboolean(L, 3));
  } else if (g_str_equal(key, "fullscreen")) {
    server_window_set_fullscreen(server_window, lua_toboolean(L, 3));
  } else {
    log_warning("invalid key: %s", key);
  }

  return 0;
}

static int
api_window__gc(lua_State* L)
{
  struct api_window* api_window = lua_touserdata(L, 1);

  wl_list_remove(&api_window->listeners.server_window_destroy.link);

  return 0;
}

static const struct luaL_Reg api_window_metatable[] = {
  {"__gc", api_window__gc},
  {"__index", api_window__index},
  {"__newindex", api_window__newindex},
  {NULL, NULL}};

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void
on_server_window_destroy(struct wl_listener* listener, void* data)
{
  struct api_window* api_window =
    wl_container_of(listener, api_window, listeners.server_window_destroy);

  api_window->server_window = NULL;

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_windows);

  if (num_windows == 1) {
    lua_pushnil(L);
    lua_rawseti(L, -2, 1);
  } else {
    // Quick delete by moving last element
    lua_rawgeti(L, -1, num_windows);

    struct api_window* last_api_window = lua_touserdata(L, -1);
    last_api_window->index = api_window->index;

    lua_rawseti(L, -2, api_window->index);
    lua_pushnil(L);
    lua_rawseti(L, -2, num_windows);
  }

  --num_windows;

  lua_pop(L, 1);
}

static void
on_new_server_window(struct wl_listener* listener, void* data)
{
  struct server_window* server_window = data;

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_windows);

  struct api_window* api_window = lua_newuserdata(L, sizeof(struct api_window));
  luaL_setmetatable(L, "catnip.window");

  ++num_windows;
  api_window->server_window = server_window;
  api_window->index = num_windows;

  wl_setup_listener(
    &api_window->listeners.server_window_destroy,
    &server_window->events.destroy,
    on_server_window_destroy
  );

  lua_rawseti(L, -2, api_window->index);

  lua_pop(L, 1);
}

void
init_api_windows(lua_State* L)
{
  lua_newtable(L);
  api_catnip_windows = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip);
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_catnip_windows);
  lua_setfield(L, -2, "windows");
  lua_pop(L, 1);

  luaL_newmetatable(L, "catnip.window");
  luaL_setfuncs(L, api_window_metatable, 0);
  lua_pop(L, 1);

  wl_setup_listener(
    &listeners.new_server_window,
    &server_window_events.new_server_window,
    on_new_server_window
  );
}
