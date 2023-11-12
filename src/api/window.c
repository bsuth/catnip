#include "window.h"
#include "api/api.h"
#include "config/config.h"
#include "config/events.h"
#include "server/window.h"
#include "utils/log.h"
#include "utils/wayland.h"
#include <glib.h>
#include <lauxlib.h>

struct api_window {
  struct server_window* window;

  int index;

  struct {
    struct wl_listener window_destroy;
  } listeners;
};

lua_Ref api_windows_ref = LUA_NOREF;

static struct {
  struct wl_listener new_server_window;
} listeners;

// -----------------------------------------------------------------------------
// Metatable: catnip.window
// -----------------------------------------------------------------------------

static int
api_window__index(lua_State* L)
{
  struct api_window* api_window = lua_touserdata(L, 1);
  struct server_window* server_window = api_window->window;

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

  if (g_str_equal(key, "x")) {
    lua_pushnumber(L, server_window_get_x(server_window));
  } else if (g_str_equal(key, "y")) {
    lua_pushnumber(L, server_window_get_y(server_window));
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
  struct server_window* server_window = api_window->window;

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

  if (g_str_equal(key, "x")) {
    server_window_set_x(server_window, luaL_checknumber(L, 3));
  } else if (g_str_equal(key, "y")) {
    server_window_set_y(server_window, luaL_checknumber(L, 3));
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
  wl_list_remove(&api_window->listeners.window_destroy.link);
  return 0;
}

static const struct luaL_Reg api_window_metatable[] = {
  {"__index", api_window__index},
  {"__newindex", api_window__newindex},
  {"__gc", api_window__gc},
  {NULL, NULL}
};

// -----------------------------------------------------------------------------
// API Window
// -----------------------------------------------------------------------------

static void
on_server_window_destroy(struct wl_listener* listener, void* data)
{
  struct api_window* api_window =
    wl_container_of(listener, api_window, listeners.window_destroy);

  api_window->window = NULL;

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_windows_ref);
  size_t api_windows_len = lua_objlen(L, -1);

  if (api_windows_len == 1) {
    lua_pushnil(L);
    lua_rawseti(L, -2, 1);
  } else {
    // Quick delete by moving last element
    lua_rawgeti(L, -1, api_windows_len);

    struct api_window* last_api_window = lua_touserdata(L, -1);
    last_api_window->index = api_window->index;

    lua_rawseti(L, -2, api_window->index);
    lua_pushnil(L);
    lua_rawseti(L, -2, api_windows_len);
  }

  lua_pop(L, 1);
}

static void
api_window_create(struct server_window* window)
{
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_windows_ref);

  struct api_window* api_window = lua_newuserdata(L, sizeof(struct api_window));
  luaL_setmetatable(L, "catnip.window");

  api_window->window = window;
  api_window->index = lua_objlen(L, -2) + 1;

  wl_setup_listener(
    &api_window->listeners.window_destroy,
    &window->events.destroy,
    on_server_window_destroy
  );

  lua_rawseti(L, -2, api_window->index);
  lua_pop(L, 1);
}

// -----------------------------------------------------------------------------
// Init
// -----------------------------------------------------------------------------

static void
on_new_server_window(struct wl_listener* listener, void* data)
{
  api_window_create(data);
  config_events_publish("window::new");
}

void
api_window_init(lua_State* L)
{
  lua_newtable(L);
  api_windows_ref = luaL_ref(L, LUA_REGISTRYINDEX);

  lua_rawgeti(L, LUA_REGISTRYINDEX, api_ref);
  lua_rawgeti(L, LUA_REGISTRYINDEX, api_windows_ref);
  lua_setfield(L, -2, "windows");
  lua_pop(L, 1);

  luaL_newmetatable(L, "catnip.window");
  luaL_setfuncs(L, api_window_metatable, 0);
  lua_pop(L, 1);

  struct server_window* window;
  wl_list_for_each(window, &server_windows, link)
  {
    api_window_create(window);
  }

  wl_setup_listener(
    &listeners.new_server_window,
    &server_window_events.new_server_window,
    on_new_server_window
  );
}
